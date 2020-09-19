/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "AutomationCurveEventComponent.h"

#include "AutomationCurveClipComponent.h"
#include "AutomationCurveHelper.h"
#include "AutomationCurveEventsConnector.h"
#include "AutomationSequence.h"
#include "MidiTrack.h"

#include "App.h"
#include "TempoDialog.h"

AutomationCurveEventComponent::AutomationCurveEventComponent(AutomationCurveClipComponent &parent,
    const AutomationEvent &event) :
    event(event),
    anchor(event),
    editor(parent),
    controllerNumber(event.getTrackControllerNumber())
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseCursor(MouseCursor::PointingHandCursor);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->recreateConnector();
}

bool AutomationCurveEventComponent::isTempoCurve() const noexcept
{
    return this->controllerNumber == MidiTrack::tempoController;
}

void AutomationCurveEventComponent::paint(Graphics &g)
{
    static constexpr auto circleMargin = 2.f;
    const auto centre = this->getLocalBounds().getCentre();

    if (this->draggingState)
    {
        // indicate dragging direction:
        if (this->dragger.getMode() == FineTuningComponentDragger::Mode::DragOnlyX)
        {
            // top/down dots
            g.fillRect(0.f, centre.y - 1.f, 1.f, 2.f);
            g.fillRect(float(this->getWidth() - 1), centre.y - 1.f, 1.f, 2.f);
        }
        else if (this->dragger.getMode() == FineTuningComponentDragger::Mode::DragOnlyY)
        {
            // left/right dots
            g.fillRect(centre.x - 1.f, 0.f, 2.f, 1.f);
            g.fillRect(centre.x - 1.f, float(this->getHeight() - 1), 2.f, 1.f);
        }
        else
        {
            g.fillEllipse(circleMargin, circleMargin,
                float(this->getWidth()) - circleMargin * 2.f,
                float(this->getHeight()) - circleMargin * 2.f);
        }
    }
    else if (this->hoveredState)
    {
        // top/down dots
        g.fillRect(0.f, centre.y - 1.f, 1.f, 2.f);
        g.fillRect(float(this->getWidth() - 1), centre.y - 1.f, 1.f, 2.f);
        // left/right dots
        g.fillRect(centre.x - 1.f, 0.f, 2.f, 1.f);
        g.fillRect(centre.x - 1.f, float(this->getHeight() - 1), 2.f, 1.f);
    }

    g.fillEllipse(centre.x - 2.f, centre.y - 2.f, 4.f, 4.f);
    g.fillEllipse(circleMargin, circleMargin,
        float(this->getWidth()) - circleMargin * 2.f,
        float(this->getHeight()) - circleMargin * 2.f);
}

bool AutomationCurveEventComponent::hitTest(int x, int y)
{
    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;

    const int dx = x - xCenter;
    const int dy = y - yCenter;
    const int r = (this->getWidth() + this->getHeight()) / 4;

    return (dx * dx) + (dy * dy) < (r * r);
}

void AutomationCurveEventComponent::mouseEnter(const MouseEvent &e)
{
    this->hoveredState = true;
    this->repaint();
}

void AutomationCurveEventComponent::mouseExit(const MouseEvent &e)
{
    this->hoveredState = false;
    this->repaint();
}

void AutomationCurveEventComponent::mouseDown(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        this->event.getSequence()->checkpoint();

        this->dragger.startDraggingComponent(this, e, this->event.getControllerValue(),
            0.f, 1.f, AutomationEvent::curveInterpolationThreshold);

        this->startDragging();
        this->repaint();
    }
}

void AutomationCurveEventComponent::mouseDrag(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            float deltaBeat = 0.f;
            float deltaValue = 0.f;
            bool beatChanged = false;
            bool valueChanged = false;
            this->getDraggingDeltas(e, deltaBeat, deltaValue, beatChanged, valueChanged);

            if (valueChanged && this->tuningIndicator == nullptr)
            {
                const float cv = this->event.getControllerValue();
                this->tuningIndicator = make<FineTuningValueIndicator>(cv, this->isTempoCurve() ? " bpm" : "");
                this->editor.getParentComponent()->addAndMakeVisible(this->tuningIndicator.get());
                this->fader.fadeIn(this->tuningIndicator.get(), 200);
            }

            if (beatChanged || valueChanged)
            {
                this->setMouseCursor(MouseCursor::DraggingHandCursor);
                auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());
                sequence->change(this->event, this->continueDragging(deltaBeat, deltaValue), true);
            }
            else
            {
                this->editor.updateCurveComponent(this); // update position anyway
            }

            if (this->tuningIndicator != nullptr)
            {
                const float cv = this->event.getControllerValue();
                if (this->isTempoCurve())
                {
                    this->tuningIndicator->setValue(cv, this->event.getControllerValueAsBPM());
                }
                else
                {
                    this->tuningIndicator->setValue(cv);
                }
                this->tuningIndicator->repositionToTargetAt(this, this->editor.getPosition());
            }

            this->repaint();
        }
    }
}

void AutomationCurveEventComponent::mouseUp(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::PointingHandCursor);
            this->editor.updateCurveComponent(this);

            if (this->tuningIndicator != nullptr)
            {
                this->fader.fadeOut(this->tuningIndicator.get(), 200);
                this->tuningIndicator = nullptr;
            }

            this->dragger.endDraggingComponent(this, e);
            this->endDragging();

            // todo:
            // if no dragging was done and this is a tempo track,
            // show the tempo dialog with a proper callback
            if (this->isTempoCurve())
            {
                this->hoveredState = false;
                this->repaint();

                App::showModalComponent(make<TempoDialog>(this->event.getControllerValueAsBPM()));
            }
        }

        this->repaint();
    }
    else if (e.mods.isRightButtonDown())
    {
        this->editor.removeEventIfPossible(this->event);
    }
}

void AutomationCurveEventComponent::recreateConnector()
{
    this->connector = make<AutomationCurveEventsConnector>(this, this->nextEventHolder);
    this->editor.addAndMakeVisible(this->connector.get());
    this->updateConnector();
}

void AutomationCurveEventComponent::recreateHelper()
{
    this->helper = make<AutomationCurveHelper>(this->event, this->editor, this, this->nextEventHolder);
    this->editor.addAndMakeVisible(this->helper.get());
    this->updateHelper();
}

void AutomationCurveEventComponent::updateConnector()
{
    this->connector->resizeToFit(this->event.getCurvature());
}

void AutomationCurveEventComponent::updateHelper()
{
    if (this->helper && this->nextEventHolder)
    {
        const float d = AutomationCurveClipComponent::helperComponentDiameter;
        const Point<int> linePos(this->connector->getPosition());
        const Point<float> lineCentre(this->connector->getCentrePoint());
        Rectangle<int> bounds(linePos.getX() + int(lineCentre.getX()) - int(d / 2),
            linePos.getY() + int(lineCentre.getY() + 0.5f - (d / 2.f)), int(d), int(d));
        this->helper->setBounds(bounds);
    }
}

void AutomationCurveEventComponent::setNextNeighbour(AutomationCurveEventComponent *next)
{
    if (next == this->nextEventHolder)
    {
        this->updateConnector();
        this->updateHelper();
        return;
    }

    this->nextEventHolder = next;
    this->recreateConnector();

    if (this->nextEventHolder == nullptr)
    {
        this->helper = nullptr;
    }
    else
    {
        this->recreateHelper();
    }
}

//===----------------------------------------------------------------------===//
// Editing
//===----------------------------------------------------------------------===//

void AutomationCurveEventComponent::startDragging()
{
    this->draggingState = true;
    this->anchor = this->event;
}

bool AutomationCurveEventComponent::isDragging() const
{
    return this->draggingState;
}

void AutomationCurveEventComponent::getDraggingDeltas(const MouseEvent &e,
    float &deltaBeat, float &deltaValue, bool &beatChanged, bool &valueChanged)
{
    this->dragger.dragComponent(this, e);

    float newBeat = -1.f;
    float newValue = -1.f; // shouldn't be used here
    this->editor.getRowsColsByMousePosition(this->getX(), this->getY(), newValue, newBeat);

    deltaValue = (this->dragger.getValue() - this->anchor.getControllerValue());
    deltaBeat = (newBeat - this->anchor.getBeat());

    valueChanged = this->getControllerValue() != this->dragger.getValue();
    beatChanged = this->getBeat() != newBeat;
}

AutomationEvent AutomationCurveEventComponent::continueDragging(const float deltaBeat, const float deltaValue)
{
    const float &newValue = this->anchor.getControllerValue() + deltaValue;
    const float &newBeat = this->anchor.getBeat() + deltaBeat;
    return this->event.withParameters(newBeat, newValue);
}

void AutomationCurveEventComponent::endDragging()
{
    this->draggingState = false;
}

int AutomationCurveEventComponent::compareElements(const AutomationCurveEventComponent *first, const AutomationCurveEventComponent *second)
{
    if (first == second) { return 0; }

    const float beatDiff = first->getBeat() - second->getBeat();
    const int beatResult = (beatDiff > 0.f) - (beatDiff < 0.f);
    if (beatResult != 0) { return beatResult; }

    const float cvDiff = first->getControllerValue() - second->getControllerValue();
    const int cvResult = (cvDiff > 0.f) - (cvDiff < 0.f); // sorted by cv, if beats are the same
    if (cvResult != 0) { return cvResult; }

    return first->event.getId()- second->event.getId();
}

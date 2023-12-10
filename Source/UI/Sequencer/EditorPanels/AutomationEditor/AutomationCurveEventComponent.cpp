/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
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

AutomationCurveEventComponent::AutomationCurveEventComponent(AutomationEditorBase &editor,
    const AutomationEvent &event, const Clip &clip) :
    editor(editor),
    event(event),
    clip(clip),
    anchor(event),
    controllerNumber(event.getTrackControllerNumber())
{
    this->setWantsKeyboardFocus(false);
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setMouseCursor(MouseCursor::PointingHandCursor);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->updateColour();
}

bool AutomationCurveEventComponent::isTempoCurve() const noexcept
{
    return this->controllerNumber == MidiTrack::tempoController;
}

void AutomationCurveEventComponent::updateColour()
{
    this->colour = this->editor.getColour(this->event)
        .withMultipliedSaturation(this->isEditable ? 1.f : 0.4f)
        .withMultipliedAlpha(this->isEditable ? 1.f : 0.2f);
}

void AutomationCurveEventComponent::paint(Graphics &g)
{
    static constexpr auto circleMargin = 2.f;
    const auto centre = this->getLocalBounds().getCentre();

    g.setColour(this->colour);

    const float w = float(this->getWidth());
    const float h = float(this->getHeight());

    if (this->isDragging)
    {
        // indicate dragging direction:
        if (this->dragger.getMode() == FineTuningComponentDragger::Mode::DragOnlyX)
        {
            // top/down dots
            g.fillRect(0.f, centre.y - 1.f, 1.f, 2.f);
            g.fillRect(w - 1.f, centre.y - 1.f, 1.f, 2.f);
        }
        else if (this->dragger.getMode() == FineTuningComponentDragger::Mode::DragOnlyY)
        {
            // left/right dots
            g.fillRect(centre.x - 1.f, 0.f, 2.f, 1.f);
            g.fillRect(centre.x - 1.f, h - 1.f, 2.f, 1.f);
        }
        else
        {
            g.fillEllipse(circleMargin, circleMargin,
                w - circleMargin * 2.f, h - circleMargin * 2.f);
        }
    }
    else if (this->isHighlighted)
    {
        // top/down dots
        g.fillRect(0.f, centre.y - 1.f, 1.f, 2.f);
        g.fillRect(w - 1.f, centre.y - 1.f, 1.f, 2.f);
        // left/right dots
        g.fillRect(centre.x - 1.f, 0.f, 2.f, 1.f);
        g.fillRect(centre.x - 1.f, h - 1.f, 2.f, 1.f);
    }

    g.fillEllipse(centre.x - 3.f, centre.y - 3.f, 6.f, 6.f);
    g.fillEllipse(circleMargin, circleMargin,
        w - circleMargin * 2.f, h - circleMargin * 2.f);
}

bool AutomationCurveEventComponent::hitTest(int x, int y)
{
    if (!this->isEditable)
    {
        return false;
    }

    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;

    const int dx = x - xCenter;
    const int dy = y - yCenter;
    const int r = (this->getWidth() + this->getHeight()) / 4;

    return (dx * dx) + (dy * dy) < (r * r);
}

void AutomationCurveEventComponent::parentHierarchyChanged()
{
    if (this->getParentComponent() != nullptr)
    {
        this->recreateConnector();
    }
}

void AutomationCurveEventComponent::mouseEnter(const MouseEvent &e)
{
    jassert(this->isEditable);
    this->isHighlighted = true;
    this->repaint();
}

void AutomationCurveEventComponent::mouseExit(const MouseEvent &e)
{
    jassert(this->isEditable);
    this->isHighlighted = false;
    this->repaint();
}

void AutomationCurveEventComponent::mouseDown(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

    jassert(this->isEditable);
    if (e.mods.isLeftButtonDown())
    {
        this->dragger.startDraggingComponent(this, e, this->event.getControllerValue(),
            0.f, 1.f, 1.f / 100.f);

        this->startDragging();
        this->repaint();
    }
}

void AutomationCurveEventComponent::mouseDrag(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        if (this->tuningIndicator != nullptr)
        {
            this->tuningIndicator->repositionAtTargetCenter(this);
        }

        return;
    }

    jassert(this->isEditable);
    if (this->isDragging)
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

            // adding it to grandparent to avoid clipping
            jassert(this->getParentComponent() != nullptr);
            jassert(this->getParentComponent()->getParentComponent() != nullptr);
            auto *grandParent = this->getParentComponent()->getParentComponent();

            grandParent->addAndMakeVisible(this->tuningIndicator.get());
            this->fader.fadeIn(this->tuningIndicator.get(), Globals::UI::fadeInLong);
        }

        if (beatChanged || valueChanged)
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());

            if (!this->anyChangeDone)
            {
                this->event.getSequence()->checkpoint();
                this->anyChangeDone = true;

                // drag-and-copy:
                if (e.mods.isShiftDown())
                {
                    sequence->insert(this->event.withNewId(), true);
                }
            }

            sequence->change(this->event, this->continueDragging(deltaBeat, deltaValue), true);
        }
        else
        {
            this->setFloatBounds(this->editor.getEventBounds(this->event, this->clip));
            this->updateChildrenBounds();
        }

        if (this->tuningIndicator != nullptr)
        {
            const float cv = this->event.getControllerValue();
            if (this->isTempoCurve())
            {
                this->tuningIndicator->setValue(cv,
                    String(this->event.getControllerValueAsBPM()));
            }
            else
            {
                this->tuningIndicator->setValue(cv);
            }

            this->tuningIndicator->repositionAtTargetCenter(this);
        }

        this->repaint();
    }
}

void AutomationCurveEventComponent::mouseUp(const MouseEvent &e)
{
    jassert(this->isEditable);
    if (this->isDragging)
    {
        this->setMouseCursor(MouseCursor::PointingHandCursor);
        this->setFloatBounds(this->editor.getEventBounds(this->event, this->clip));
        this->updateChildrenBounds();

        if (this->tuningIndicator != nullptr)
        {
            this->fader.fadeOut(this->tuningIndicator.get(), Globals::UI::fadeOutLong);
            this->tuningIndicator = nullptr;
        }

        this->dragger.endDraggingComponent(this, e);
        this->endDragging();

        this->repaint();
    }

    // either deleting this event or showing the edit dialog
    const auto isPenMode = this->editor.hasEditMode(RollEditMode::drawMode);
        
    if (e.mods.isRightButtonDown() ||
        (e.source.isTouch() && isPenMode && !this->anyChangeDone))
    {
        auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());
        if (sequence->size() > 1) // no empty automation tracks please
        {
            sequence->checkpoint();
            sequence->remove(this->event, true);
        }
        return;
    }
    
    if (!this->anyChangeDone && this->isTempoCurve())
    {
        this->isHighlighted = false;
        this->repaint();

        auto dialog = make<TempoDialog>(this->event.getControllerValueAsBPM());
        dialog->onOk = [this](int newBpmValue)
        {
            auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());
            sequence->change(this->event, this->event.withTempoBpm(newBpmValue), true);
        };

        App::showModalComponent(move(dialog));
    }
}

void AutomationCurveEventComponent::recreateConnector()
{
    if (this->nextEventHolder)
    {
        this->connector = make<AutomationCurveEventsConnector>(this->editor, this, this->nextEventHolder);
        jassert(this->getParentComponent() != nullptr);
        this->getParentComponent()->addAndMakeVisible(this->connector.get());
        this->updateConnector();
    }
}

void AutomationCurveEventComponent::recreateHelper()
{
    if (this->nextEventHolder)
    {
        jassert(this->getParentComponent() != nullptr);
        this->helper = make<AutomationCurveHelper>(this->event, this, this->nextEventHolder);
        this->getParentComponent()->addAndMakeVisible(this->helper.get());
        this->updateHelper();
    }
}

void AutomationCurveEventComponent::updateConnector()
{
    if (this->connector)
    {
        this->connector->resizeToFit(this->event.getCurvature());
        this->connector->repaint();
    }
}

void AutomationCurveEventComponent::updateHelper()
{
    if (this->helper && this->connector && this->nextEventHolder)
    {
        const auto d = float(this->getWidth()) * 0.65f;
        const auto linePos = this->connector->getPosition();
        const auto lineCentre = this->connector->getCentrePoint();
        const Rectangle<int> bounds(linePos.getX() + int(lineCentre.getX() - (d / 2.f)),
            linePos.getY() + int(lineCentre.getY() - (d / 2.f) + 0.5f),
            int(d), int(d));

        this->helper->setEditable(this->isEditable);
        this->helper->setBounds(bounds);
        this->helper->repaint();
    }
}

//===----------------------------------------------------------------------===//
// EventComponentBase
//===----------------------------------------------------------------------===//

void AutomationCurveEventComponent::setNextNeighbour(EventComponentBase *next)
{
    if (next == this->nextEventHolder)
    {
        this->updateChildrenBounds();
        return;
    }

    this->nextEventHolder = next;

    if (this->nextEventHolder == nullptr)
    {
        this->connector = nullptr;
        this->helper = nullptr;
    }
    else
    {
        this->recreateConnector();
        this->recreateHelper();
    }
}

void AutomationCurveEventComponent::setPreviousNeighbour(EventComponentBase *prev)
{
    if (prev == this->prevEventHolder)
    {
        return;
    }

    this->prevEventHolder = prev;
}

void AutomationCurveEventComponent::setEditable(bool shouldBeEditable)
{
    if (this->isEditable == shouldBeEditable)
    {
        return;
    }

    this->isEditable = shouldBeEditable;

    this->updateColour();
    this->updateHelper();
    // the connector is not interactive, just skip it

    if (this->isEditable)
    {
        this->toFront(false);
    }
}

//===----------------------------------------------------------------------===//
// Editing
//===----------------------------------------------------------------------===//

void AutomationCurveEventComponent::startDragging()
{
    this->isDragging = true;
    this->anyChangeDone = false;
    this->anchor = this->event;
}

bool AutomationCurveEventComponent::isInEditMode() const
{
    return this->isDragging;
}

void AutomationCurveEventComponent::getDraggingDeltas(const MouseEvent &e,
    float &deltaBeat, float &deltaValue, bool &beatChanged, bool &valueChanged)
{
    this->dragger.dragComponent(this, e);

    float newBeat = -1.f;
    float newValue = -1.f;

    // aligned by center:
    this->editor.getBeatValueByPosition(this->getX() + this->getWidth() / 2,
        this->getY() + this->getHeight() / 2,
        this->clip, newValue, newBeat);

    deltaValue = (this->dragger.getValue() - this->anchor.getControllerValue());
    deltaBeat = (newBeat - this->anchor.getBeat());

    valueChanged = this->getControllerValue() != this->dragger.getValue();
    beatChanged = this->getEvent().getBeat() != newBeat;
}

AutomationEvent AutomationCurveEventComponent::continueDragging(const float deltaBeat, const float deltaValue)
{
    const float newValue = this->anchor.getControllerValue() + deltaValue;
    const float newBeat = this->anchor.getBeat() + deltaBeat;
    return this->event.withParameters(newBeat, newValue);
}

void AutomationCurveEventComponent::endDragging()
{
    this->isDragging = false;
}

MouseCursor AutomationCurveEventComponent::startEditingNewEvent(const MouseEvent &e)
{
    this->dragger.startDraggingComponent(this, e,
        this->event.getControllerValue(),
        0.f, 1.f, 1.f / 100.f);

    this->startDragging();
    this->repaint();

    // set this to true to avoid setting a checkpoint between
    // inserting an event and dragging it, both actions
    // should be done in a single transaction:
    this->anyChangeDone = true;

    return MouseCursor::DraggingHandCursor;
}

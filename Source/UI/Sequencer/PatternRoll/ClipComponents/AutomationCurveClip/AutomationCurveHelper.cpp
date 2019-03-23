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
#include "AutomationCurveHelper.h"
#include "AutomationSequence.h"
#include "AutomationCurveClipComponent.h"

AutomationCurveHelper::AutomationCurveHelper(const AutomationEvent &event,
    const AutomationCurveClipComponent &editor, Component *target1, Component *target2) :
    event(event),
    editor(editor),
    component1(target1),
    component2(target2),
    draggingState(false)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseCursor(MouseCursor::UpDownResizeCursor);

    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
}

void AutomationCurveHelper::paint(Graphics &g)
{
    g.fillEllipse(0.f, 0.f, float(this->getWidth()), float(this->getHeight()));

    if (this->draggingState)
    {
        g.fillEllipse (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0));
    }
}

bool AutomationCurveHelper::hitTest(int x, int y)
{
    const int xCenter = this->getWidth() / 2;
    const int yCenter = this->getHeight() / 2;

    const int dx = x - xCenter;
    const int dy = y - yCenter;
    const int r = (this->getWidth() + this->getHeight()) / 4;

    return (dx * dx) + (dy * dy) < (r * r);
}

void AutomationCurveHelper::mouseDown(const MouseEvent &e)
{
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        this->event.getSequence()->checkpoint();
        this->dragger.startDraggingComponent(this, e, this->getCurvature(),
            0.f, 1.f, CURVE_INTERPOLATION_THRESHOLD, FineTuningComponentDragger::DragOnlyY);
        this->anchor = this->getBounds().getCentre();
        this->curveAnchor = this->getCurvature();
        this->draggingState = true;
        this->repaint();
    }
}

void AutomationCurveHelper::mouseDrag(const MouseEvent &e)
{
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->dragger.dragComponent(this, e);
            const float newCurvature = this->dragger.getValue();
            if (newCurvature != this->getCurvature())
            {
                if (this->tuningIndicator == nullptr)
                {
                    this->tuningIndicator = new FineTuningValueIndicator(this->event.getCurvature(), "");
                    this->editor.getParentComponent()->addAndMakeVisible(this->tuningIndicator);
                    this->fader.fadeIn(this->tuningIndicator, 200);
                }

                auto *sequence = static_cast<AutomationSequence *>(this->event.getSequence());
                sequence->change(this->event, this->event.withCurvature(newCurvature), true);

                if (this->tuningIndicator != nullptr)
                {
                    const float cv = this->event.getCurvature();
                    this->tuningIndicator->setValue(cv, cv);
                    this->tuningIndicator->repositionToTargetAt(this, this->editor.getPosition());
                }
            }
        }
    }
}

void AutomationCurveHelper::mouseUp(const MouseEvent &e)
{
    if (this->component1 == nullptr || this->component2 == nullptr)
    {
        return;
    }

    if (this->tuningIndicator != nullptr)
    {
        this->fader.fadeOut(this->tuningIndicator, 200);
        this->tuningIndicator = nullptr;
    }

    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            //this->constrainPosition();
            this->draggingState = false;
        }

        this->repaint();
    }
}

float AutomationCurveHelper::getCurvature() const
{
    return this->event.getCurvature();
}

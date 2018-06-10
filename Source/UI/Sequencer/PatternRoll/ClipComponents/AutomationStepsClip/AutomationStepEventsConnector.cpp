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
#include "AutomationStepEventsConnector.h"
#include "AutomationStepEventComponent.h"
#include "AutomationStepsClipComponent.h"

AutomationStepEventsConnector::AutomationStepEventsConnector(AutomationStepEventComponent *c1,
    AutomationStepEventComponent *c2, bool isEventTriggered) :
    component1(c1),
    component2(c2),
    isTriggered(isEventTriggered),
    draggingState(false),
    anchorBeat(0.f),
    anchorBeatChild1(0.f),
    anchorBeatChild2(0.f)
{
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setMouseCursor(MouseCursor::CopyingCursor);
}

void AutomationStepEventsConnector::getPoints(float &x1, float &x2, float &y1, float &y2) const
{
    if (this->component1 != nullptr)
    {
        x1 = float(this->component1->getRealBounds().getRight());
        y1 = float(this->component1->getRealBounds().getY());
    }

    if (this->component2 != nullptr)
    {
        x2 = float(this->component2->getRealBounds().getX());
        y2 = float(this->component2->getRealBounds().getBottom());
    }
}

void AutomationStepEventsConnector::retargetAndUpdate(AutomationStepEventComponent *c1,
    AutomationStepEventComponent *c2, bool isEventTriggered)
{
    this->component1 = c1;
    this->component2 = c2;
    this->resizeToFit(isEventTriggered);
}

void AutomationStepEventsConnector::resizeToFit(bool isEventTriggered)
{
    if (this->component1 == nullptr && this->component2 == nullptr)
    {
        return;
    }

    const bool shouldRepaint = (this->isTriggered != isEventTriggered);
    this->isTriggered = isEventTriggered;

    const float r = STEP_EVENT_POINT_OFFSET;
    float x1 = 0.f, x2 = 0.f, y1 = 0.f, y2 = 0.f;
    this->getPoints(x1, x2, y1, y2);

    const bool compact = this->anyAliveChild()->hasCompactMode();
    const float top = r + STEP_EVENT_MARGIN + 1.f;
    const float bottom = y2 - r - STEP_EVENT_MARGIN + 1.f;
    this->realBounds = { jmin(x1, x2) + (compact ? (r + 1.f) : (r - 1.f)),
        this->isTriggered ? bottom : top,
        fabsf(x1 - x2) - (compact ? r * 2.f : 1.f),
        this->isTriggered ? top : bottom };
    
    this->setBounds(this->realBounds.toType<int>());
    
    if (shouldRepaint)
    {
        this->repaint();
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationStepEventsConnector::paint(Graphics &g)
{
    if (this->realBounds.getWidth() > STEP_EVENT_POINT_OFFSET)
    {
        g.setColour(this->anyAliveChild()->getEditor()->getEventColour());
        const float left = this->realBounds.getX() - float(this->getX());
        g.drawHorizontalLine(0, left, this->realBounds.getWidth());
#if STEP_EVENT_THICK_LINES
        g.drawHorizontalLine(1, left, this->realBounds.getWidth() - 1.f);
#endif
    }
}

void AutomationStepEventsConnector::mouseMove(const MouseEvent &e)
{
    this->applyCursorForEvent(e);
}

void AutomationStepEventsConnector::mouseDown(const MouseEvent &e)
{
    this->applyCursorForEvent(e);

    if (this->hasDraggingMode(e))
    {
        this->anchorBeat =  this->anyAliveChild()->getEditor()->getBeatByXPosition(this->anyAliveChild()->getX());
        this->anchorBeatChild1 = this->component1 ? this->component1->getBeat() : 0.f;
        this->anchorBeatChild2 = this->component2 ? this->component2->getBeat() : 0.f;
        this->dragger.startDraggingComponent(this->anyAliveChild(), e);
        this->draggingState = true;
    }
    else
    {
        this->anyAliveChild()->getEditor()->mouseDown(e.getEventRelativeTo(this->anyAliveChild()->getEditor()));
    }
}

void AutomationStepEventsConnector::mouseDrag(const MouseEvent &e)
{
    this->applyCursorForEvent(e);

    if (this->draggingState)
    {
        this->dragger.dragComponent(this->anyAliveChild(), e, nullptr);
        float newRoundBeat = this->anyAliveChild()->getEditor()->getBeatByXPosition(this->anyAliveChild()->getX());
        float deltaBeat = (newRoundBeat - this->anchorBeat);
        float newBeatChild1 = (this->anchorBeatChild1 + deltaBeat);
        float newBeatChild2 = (this->anchorBeatChild2 + deltaBeat);

        if (this->component1)
        {
            this->component1->drag(newBeatChild1);
            this->component1->updateConnector();
        }
        
        if (this->component2)
        {
            this->component2->drag(newBeatChild2);
            this->component2->updateConnector();
        }
    }
}

void AutomationStepEventsConnector::mouseUp(const MouseEvent &e)
{
    this->applyCursorForEvent(e);

    if (this->draggingState)
    {
        this->draggingState = false;
    }
}

bool AutomationStepEventsConnector::hasDraggingMode(const MouseEvent &e) const
{
    return ! e.mods.isAnyModifierKeyDown();
}

void AutomationStepEventsConnector::applyCursorForEvent(const MouseEvent &e)
{
    if (this->hasDraggingMode(e))
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
    }
    else
    {
        this->setMouseCursor(MouseCursor::CopyingCursor);
    }
}

AutomationStepEventComponent *AutomationStepEventsConnector::anyAliveChild() const
{
    jassert(this->component1 || this->component2);
    return (this->component1 ? this->component1 : this->component2);
}

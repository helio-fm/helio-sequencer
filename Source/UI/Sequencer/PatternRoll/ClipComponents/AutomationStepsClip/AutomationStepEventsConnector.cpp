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
                                             AutomationStepEventComponent *c2,
                                             bool isEventTriggered)
    : component1(c1),
      component2(c2),
      isTriggered(isEventTriggered),
      draggingState(false),
      anchorBeat(0.f),
      anchorBeatChild1(0.f),
      anchorBeatChild2(0.f)
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setMouseCursor(MouseCursor::CopyingCursor);
    
    this->setSize(600, 32);
}

void AutomationStepEventsConnector::getPoints(float &x1, float &x2, float &y1, float &y2) const
{
    jassert(this->component1 || this->component2);

    if (this->component1 == nullptr)
    {
        x1 = 0;
        y1 = float(this->component2->getRealBounds().getY());
    }
    else
    {
        x1 = float(this->component1->getRealBounds().getRight());
        y1 = float(this->component1->getRealBounds().getY());
    }

    if (this->component2 == nullptr)
    {
        if (this->component1->getParentComponent())
        {
            x2 = float(this->component1->getParentWidth());
            y2 = float(this->component1->getRealBounds().getBottom());
        }
    }
    else
    {
        x2 = float(this->component2->getRealBounds().getX());
        y2 = float(this->component2->getRealBounds().getBottom());
    }
}

void AutomationStepEventsConnector::retargetAndUpdate(AutomationStepEventComponent *c1,
                                              AutomationStepEventComponent *c2,
                                              bool isEventTriggered)
{
    this->component1 = c1;
    this->component2 = c2;
    this->resizeToFit(isEventTriggered);
}

#define SUSTAIN_PEDAL_CONNECTOR_HEIGHT 12

void AutomationStepEventsConnector::resizeToFit(bool isEventTriggered)
{
    if (this->component1 == nullptr && this->component2 == nullptr)
    {
        return;
    }

    const bool shouldRepaint = (this->isTriggered != isEventTriggered);

    this->isTriggered = isEventTriggered;

    float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    this->getPoints(x1, x2, y1, y2);

    this->realBounds = Rectangle<float>(jmin(x1, x2) - 1.f,
                                        this->isTriggered ? y2 : 0,
                                        fabsf(x1 - x2) + 2.f,
                                        this->isTriggered ? 0.f : float(this->getHeight()));
    
    this->setBounds(this->realBounds.toType<int>());
    
    if (shouldRepaint)
    {
        this->repaint();
    }
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationStepEventsConnector::paint(Graphics& g)
{
    g.setColour(Colours::white.withAlpha(0.1f));
    g.fillAll();
}

void AutomationStepEventsConnector::resized()
{
    const float delta = this->realBounds.getX() - float(this->getBounds().getX());
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
    return (this->component1 ? this->component1 : this->component2);
}

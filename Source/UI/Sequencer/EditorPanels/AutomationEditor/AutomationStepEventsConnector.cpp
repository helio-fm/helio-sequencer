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
#include "AutomationStepEventsConnector.h"
#include "AutomationStepEventComponent.h"
#include "AutomationStepsClipComponent.h"

AutomationStepEventsConnector::AutomationStepEventsConnector(AutomationEditorBase::EventComponentBase *c1,
    AutomationEditorBase::EventComponentBase *c2, float eventControllerValue) :
    component1(c1),
    component2(c2),
    eventControllerValue(eventControllerValue)
{
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setAccessible(false);

    jassert(this->firstAliveEventComponent() != nullptr);
    this->setMouseCursor(this->firstAliveEventComponent()->getMouseCursor());
}

void AutomationStepEventsConnector::getPoints(float &x1, float &x2, float &y1, float &y2) const
{
    if (this->component1 != nullptr)
    {
        const auto bounds1 = component1->getFloatBounds();
        x1 = bounds1.getRight();
        y1 = bounds1.getY();
    }

    if (this->component2 != nullptr)
    {
        const auto bounds2 = component2->getFloatBounds();
        x2 = bounds2.getX();
        y2 = bounds2.getBottom();
    }
}

void AutomationStepEventsConnector::resizeToFit(float newControllerValue)
{
    if (this->component1 == nullptr && this->component2 == nullptr)
    {
        return;
    }

    const bool shouldRepaint = this->eventControllerValue != newControllerValue;
    this->eventControllerValue = newControllerValue;

    constexpr auto marginTop = AutomationStepEventComponent::marginTop;
    constexpr auto marginBottom = AutomationStepEventComponent::marginBottom;
    constexpr auto r = AutomationStepEventComponent::pointRadius;
    constexpr auto d = r * 2.f;

    float x1 = 0.f, x2 = 0.f, y1 = 0.f, y2 = 0.f;
    this->getPoints(x1, x2, y1, y2);

    if (x1 >= x2)
    {
        this->setBounds({});
        return;
    }

    const float h = y2 - d - marginTop - marginBottom;
    const float y = r + marginTop + (h * this->eventControllerValue);

    this->realBounds = { jmin(x1, x2) + r, y, fabsf(x1 - x2), y2 - y };
    
    this->setBounds(this->realBounds.toNearestInt());
    
    if (shouldRepaint)
    {
        this->repaint();
    }
}

void AutomationStepEventsConnector::setEditable(bool shouldBeEditable)
{
    if (this->isEditable == shouldBeEditable)
    {
        return;
    }

    this->isEditable = shouldBeEditable;
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void AutomationStepEventsConnector::paint(Graphics &g)
{
    if (this->realBounds.getWidth() > AutomationStepEventComponent::pointRadius)
    {
        const auto *child = this->firstAliveEventComponent();
        g.setColour(child->getColour());

        const float left = this->realBounds.getX() - float(this->getX());
        g.drawHorizontalLine(0, left, this->realBounds.getWidth());

        if (this->isHighlighted)
        {
            g.fillRect(0, this->getHeight() - 6, this->getWidth(), 4);
        }
    }
}

bool AutomationStepEventsConnector::hitTest(int x, int y)
{
    if (!this->isEditable)
    {
        return false;
    }

    return Component::hitTest(x, y);
}

void AutomationStepEventsConnector::mouseDown(const MouseEvent &e)
{
    jassert(this->isEditable);
    BailOutChecker bailOutChecker(this);
    auto *eventComponent = this->firstAliveEventComponent();
    eventComponent->mouseDown(e.getEventRelativeTo(eventComponent));
    if (!bailOutChecker.shouldBailOut())
    {
        this->setMouseCursor(eventComponent->getMouseCursor());
    }
}

void AutomationStepEventsConnector::mouseDrag(const MouseEvent &e)
{
    jassert(this->isEditable);
    auto *eventComponent = this->firstAliveEventComponent();
    eventComponent->mouseDrag(e.getEventRelativeTo(eventComponent));
    this->setMouseCursor(eventComponent->getMouseCursor());
}

void AutomationStepEventsConnector::mouseUp(const MouseEvent &e)
{
    jassert(this->isEditable);
    BailOutChecker bailOutChecker(this);
    auto *eventComponent = this->firstAliveEventComponent();
    eventComponent->mouseUp(e.getEventRelativeTo(eventComponent));
    if (!bailOutChecker.shouldBailOut())
    {
        this->setMouseCursor(eventComponent->getMouseCursor());
    }
}

void AutomationStepEventsConnector::mouseEnter(const MouseEvent &e)
{
    jassert(this->isEditable);
    this->isHighlighted = true;
    this->repaint();

    // highlights the pivot component as well to give a cue
    // that dragging this helper will also drag the pivot component
    auto *eventComponent = this->firstAliveEventComponent();
    eventComponent->mouseEnter(e.getEventRelativeTo(eventComponent));
}

void AutomationStepEventsConnector::mouseExit(const MouseEvent &e)
{
    jassert(this->isEditable);
    this->isHighlighted = false;
    this->repaint();

    auto *eventComponent = this->firstAliveEventComponent();
    eventComponent->mouseExit(e.getEventRelativeTo(eventComponent));
}

AutomationEditorBase::EventComponentBase *AutomationStepEventsConnector::firstAliveEventComponent() const
{
    jassert(this->component1 || this->component2);
    return (this->component1 ? this->component1 : this->component2);
}

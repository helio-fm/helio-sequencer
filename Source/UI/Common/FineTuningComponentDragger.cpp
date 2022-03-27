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
#include "FineTuningComponentDragger.h"

void FineTuningComponentDragger::startDraggingComponent(Component *const component,    
    const MouseEvent &e, float currentValue, float lowerBound, float upperBound,
    float interval, Mode mode)
{
    jassert(component != nullptr);
    jassert(e.mods.isAnyMouseButtonDown()); // The event has to be a drag event!
    jassert(lowerBound <= upperBound);

    if (component != nullptr)
    {
        this->dragMode = mode;
        this->range = { lowerBound, upperBound, interval };
        this->mouseDownWithinTarget = e.getEventRelativeTo(component).getMouseDownPosition();
        this->mousePosWhenLastDragged = e.position;
        this->valueWhenLastDragged = jlimit(lowerBound, upperBound, currentValue);
        this->mousePositionChanged = false;
        this->dragComponent(component, e);
    }
}

void FineTuningComponentDragger::dragComponent(Component *const component, const MouseEvent &e)
{
    jassert(component != nullptr);
    jassert(e.mods.isAnyMouseButtonDown()); // The event has to be a drag event!

    if (component != nullptr)
    {
        const auto mouseDiffX = e.position.x - this->mousePosWhenLastDragged.x;
        const auto mouseDiffY = e.position.y - this->mousePosWhenLastDragged.y;
        const auto absDiffX = double(std::abs(mouseDiffX));
        const auto absDiffY = double(std::abs(mouseDiffY));

        auto speedX = jlimit(0.0, FineTuningComponentDragger::dragMaxSpeed, absDiffX);
        auto speedY = jlimit(0.0, FineTuningComponentDragger::dragMaxSpeed, absDiffY);

        this->mousePositionChanged = this->mousePositionChanged ||
            (absDiffX > 1.5 || absDiffY > 1.5);

        // Once we have enough data to guess, try to auto-detect dragging mode:
        if (this->dragMode == Mode::AutoSelect && this->mousePositionChanged)
        {
            this->dragMode = (absDiffX - absDiffY) > 1.0 ? Mode::DragOnlyX : Mode::DragOnlyY;
        }

        // Adjust X position (simple linear drag)
        if ((this->dragMode == Mode::DragOnlyX) && speedX != 0.0)
        {
            const auto shift = e.getEventRelativeTo(component).getPosition() - this->mouseDownWithinTarget;
            component->setTopLeftPosition(component->getPosition().translated(shift.getX(), 0));
        }

        // Adjust Y position (velocity-based drag)
        if ((this->dragMode == Mode::DragOnlyY) && speedY != 0.0)
        {
            speedY = -FineTuningComponentDragger::dragSpeedSensivity *
                (1.0 + std::sin(MathConstants<double>::pi * (1.5 + jmin(0.5, jmax(0.0,
                (speedY - FineTuningComponentDragger::dragSpeedThreshold)) / FineTuningComponentDragger::dragMaxSpeed))));

            if (mouseDiffY < 0)
            {
                speedY = -speedY;
            }

            const auto currentPos = this->range.convertTo0to1(this->valueWhenLastDragged);
            this->valueWhenLastDragged = this->range.convertFrom0to1(jlimit(0.0, 1.0, currentPos + speedY));

            e.source.enableUnboundedMouseMovement(true, false);
        }

        this->valueWhenLastDragged = jlimit(this->range.start, this->range.end, this->valueWhenLastDragged);
        this->mousePosWhenLastDragged = e.position;
    }
}

void FineTuningComponentDragger::endDraggingComponent(Component *const component, const MouseEvent &e)
{
    auto ms = Desktop::getInstance().getMainMouseSource();
    if (ms.isUnboundedMouseMovementEnabled())
    {
        ms.enableUnboundedMouseMovement(false);
        const auto mousePos = component->localPointToGlobal(this->mouseDownWithinTarget);
        ms.setScreenPosition(mousePos.toFloat());
    }
}

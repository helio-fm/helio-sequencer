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

#pragma once

class FineTuningComponentDragger final
{
public:

    enum class Mode : int8
    {
        AutoSelect,
        DragOnlyX,
        DragOnlyY
    };

    FineTuningComponentDragger() = default;

    void startDraggingComponent(Component *const component, const MouseEvent &e,
        float currentValue, float lowerBound = 0.f, float upperBound = 1.f,
        float interval = 0.01f, Mode dragMode = Mode::AutoSelect)
    {
        jassert(component != nullptr);
        jassert(e.mods.isAnyMouseButtonDown());
        jassert(lowerBound <= upperBound);

        if (component != nullptr)
        {
            this->dragMode = dragMode;
            this->range = { lowerBound, upperBound, interval };
            this->mouseDownWithinTarget = e.getEventRelativeTo(component).getMouseDownPosition();
            this->value = jlimit(lowerBound, upperBound, currentValue);
            this->anchor = this->range.convertTo0to1(this->value);
            this->dragComponent(component, e);
        }
    }

    void dragComponent(Component *const component, const MouseEvent &e)
    {
        jassert(component != nullptr);
        jassert(e.mods.isAnyMouseButtonDown()); // The event has to be a drag event!

        if (component != nullptr)
        {
            const auto shift = e.getEventRelativeTo(component).getPosition() - this->mouseDownWithinTarget;

            // once we have enough data to guess, try to auto-detect the dragging mode:
            if (this->dragMode == Mode::AutoSelect && (shift.getDistanceFromOrigin() > 1.5f))
            {
                this->dragMode = (fabs(shift.getX()) - fabs(shift.getY())) > 1.f ? Mode::DragOnlyX : Mode::DragOnlyY;
            }

            // horizontal dragging, e.g. for changing beat position
            if ((this->dragMode == Mode::DragOnlyX) && shift.getX() != 0.f)
            {
                component->setTopLeftPosition(component->getPosition().translated(shift.getX(), 0));
            }

            // vertical dragging, e.g. for adjusting velocity
            if ((this->dragMode == Mode::DragOnlyY) && shift.getY() != 0.f)
            {
                const double mouseRangeY =
                    this->range.getRange().getLength() / this->range.interval * 12.0;
                const auto delta = double(-shift.getY()) / mouseRangeY;
                this->value = this->range.convertFrom0to1(jlimit(0.0, 1.0, this->anchor + delta));
                e.source.enableUnboundedMouseMovement(true, false);
            }

            this->value = jlimit(this->range.start, this->range.end, this->value);
        }
    }

    void endDraggingComponent(Component *const component, const MouseEvent &e)
    {
        auto ms = Desktop::getInstance().getMainMouseSource();
        if (ms.isUnboundedMouseMovementEnabled())
        {
            ms.enableUnboundedMouseMovement(false);
            const auto mousePos = component->localPointToGlobal(this->mouseDownWithinTarget);
            ms.setScreenPosition(mousePos.toFloat());
        }
    }

    inline Mode getMode() const noexcept
    {
        return this->dragMode;
    }

    inline float getValue() const noexcept
    {
        return float(this->value);
    }

private:

    Mode dragMode = Mode::AutoSelect;

    double value = 0.0;
    double anchor = 0.0;

    NormalisableRange<double> range;
    Point<int> mouseDownWithinTarget;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FineTuningComponentDragger)
};

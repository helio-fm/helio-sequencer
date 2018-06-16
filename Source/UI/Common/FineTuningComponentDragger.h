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

#pragma once

class FineTuningComponentDragger final
{
public:

    enum Mode
    {
        DragOnlyX,
        DragOnlyY,
        AutoSelect,
    };

    FineTuningComponentDragger();
    ~FineTuningComponentDragger();

    void startDraggingComponent(Component *const component, const MouseEvent &e,
        float currentValue, float lowerBound = 0.f, float upperBound = 1.f,
        float interval = 0.01f, Mode dragMode = AutoSelect);
    void dragComponent(Component *const component, const MouseEvent &e);
    void endDraggingComponent(Component *const component, const MouseEvent &e);

    float getValue() const noexcept
    {
        return float(this->valueWhenLastDragged);
    }

private:

    Mode dragMode = AutoSelect;

    double valueOnMouseDown = 0.0;
    double valueWhenLastDragged = 0.0;

    NormalisableRange<double> range;
    Point<int> mouseDownWithinTarget;
    Point<float> mousePosWhenLastDragged;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FineTuningComponentDragger)
};

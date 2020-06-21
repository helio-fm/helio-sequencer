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

#include "SelectableComponent.h"

class SelectionComponent final : public Component, private Timer
{
public:

    SelectionComponent();

    void beginLasso(const Point<float> &position,
        LassoSource<SelectableComponent *> *lassoSource);
    void dragLasso(const MouseEvent &e);
    void endLasso();
    bool isDragging() const;
    void updateBounds();

    void paint(Graphics &g) override;

private:

    Array<SelectableComponent *> originalSelection;
    LassoSource<SelectableComponent *> *source = nullptr;

    Point<double> startPosition;
    Point<double> endPosition;

    const Point<double> getParentSize() const;

private:

    // some helpers for the fancy animation stuff

    void timerCallback() override;
    void fadeIn();
    void fadeOut();

    const Colour fill;
    const Colour outline;

    Colour currentFill;
    Colour currentOutline;

    Colour targetFill;
    Colour targetOutline;

private:

    Array<SelectableComponent *> itemsInLasso;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionComponent)
};

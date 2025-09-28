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

#include "Lasso.h"
#include "SelectableComponent.h"

class SelectionComponent final :
    public Component,
    public ChangeBroadcaster,
    private Timer
{
public:

    enum class LassoType { Rectangle, Path };

    SelectionComponent();

    void beginLasso(const Point<float> &position,
        DrawableLassoSource<SelectableComponent *> *lassoSource,
        LassoType lassoType = LassoType::Rectangle);
    void dragLasso(const MouseEvent &e);
    void dragLasso(const Point<float> &cursorPosition, ModifierKeys mods = {});
    void endLasso();
    bool isDragging() const;

    bool updateBounds();

    void paint(Graphics &g) override;

private:

    LassoType lassoType = LassoType::Rectangle;

    Array<SelectableComponent *> originalSelection;
    DrawableLassoSource<SelectableComponent *> *source = nullptr;

    bool dragging = false;

    // rectangle mode, the start and the end
    // in absolute values, as a fraction of the parent size:
    Point<float> startPosition;
    Point<float> endPosition;

    // drawing mode, the raw positions in absolute values,
    // the reduced points in pixels, and paths for painting:
    Array<Point<float>> drawnAreaRaw;
    Array<Point<float>> drawnArea;
    Path drawnPathFill;
    Path drawnPathOutline;

private:

    // some helpers for the fancy animations
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

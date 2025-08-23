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

#include "HighlightedComponent.h"

class DraggingListBoxComponent : public HighlightedComponent
{
public:
    
    explicit DraggingListBoxComponent(Viewport *parent, bool disablesAllChildren = true);
    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void childrenChanged() override;
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &event,
        const MouseWheelDetails &wheel) override;
    
protected:
    
    bool listCanBeScrolled() const;
    
    virtual void setSelected(bool shouldBeSelected) = 0;
    
    SafePointer<Viewport> parentViewport;

private:

    int maxDragDistance = 0;
    int viewportStartPosY = 0;
    const bool shouldDisableAllChildren = false;
    
private:

    double dragStartMilliseconds = 0.0;

    static constexpr auto dragStartThreshold = 5;
    static constexpr auto dragSpeed = 2.f;
    
};

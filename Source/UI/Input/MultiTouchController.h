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

#include "MultiTouchListener.h"

class MultiTouchController final : public MouseListener
{
public:

    explicit MultiTouchController(MultiTouchListener &parent);

    inline bool hasMultiTouch() const noexcept
    {
        return this->hasFinger0 && this->hasFinger1;
    }

    // "is already in multitouch mode or entering it with this event"
    inline bool hasMultiTouch(const MouseEvent &e) const noexcept
    {
        return (this->hasFinger0 && this->hasFinger1) ||
               (this->hasFinger0 && e.source.getIndex() == 1) ||
               (this->hasFinger1 && e.source.getIndex() == 0);
    }

    void mouseDown(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;

private:

    void processContinueZoomingEvent(const MouseEvent &e);

    MultiTouchListener &listener;

    Point<float> relativePositionAnchor1;
    Point<float> relativePositionAnchor2;

    Point<float> relativePosition1;
    Point<float> relativePosition2;

    Point<float> absolutePositionAnchor1;
    Point<float> absolutePositionAnchor2;

    bool hasFinger0 = false;
    bool hasFinger1 = false;

};

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

#include "MultiTouchListener.h"

class MultiTouchController final : public MouseListener
{
public:

    explicit MultiTouchController(MultiTouchListener &parent);

    inline bool hasMultiTouch() const noexcept
    {
        return this->touches.size() >= 2;
    }

    // "is already in multitouch mode or entering it with this event"
    inline bool hasMultiTouch(const MouseEvent &e) const noexcept
    {
        return (this->touches.size() >= 2) ||
            (this->touches.size() == 1 && !this->touches.contains(e.source.getIndex()));
    }

    void mouseDown(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;

private:

    void registerTouchEvent(const MouseEvent &e);
    void processContinueZoomingEvent(const MouseEvent &e);

    MultiTouchListener &listener;

    struct TouchData final
    {
        Point<float> relativePosition;
        Point<float> relativePositionAnchor;
        Point<float> absolutePositionAnchor;
    };

    TouchData getAllTouchData(const MouseEvent &e) const;

    // the app only supports 2-fingers multitouch for zooming and panning
    TouchData finger1;
    TouchData finger2;

    enum class TouchUsage : int
    {
        Unused = 0,
        Finger1,
        Finger2
    };

    // on some platforms (looking at you, Android) we can't seem to rely on specific
    // event ids, e.g. assume that id 0 is the 1st touch and id 1 is the 2nd, hence this map:
    FlatHashMap<int, TouchUsage> touches; // event id : used as

    // also keeping track of unused touches for cases like
    // "3 or more touches are on, so we're using only the first two,
    // and if one of them is released, we reuse one of the remaining touches":
    FlatHashMap<int, TouchData> anchorsCache; // event id : last anchors and position

    //void dump() const;
};

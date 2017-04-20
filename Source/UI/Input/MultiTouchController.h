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

class MultiTouchController : public MouseListener
{
public:

    explicit MultiTouchController(MultiTouchListener &parent);

    inline bool hasMultitouch()
    { return (this->gesture != NoMultitouch); }

    void mouseDown(const MouseEvent &event) override;

    void mouseDrag(const MouseEvent &event) override;

    void mouseUp(const MouseEvent &event) override;

private:

    void initAnchors(const MouseEvent &event);

    MultiTouchListener &listener;

    enum Mode
    {
        NoMultitouch = 1,
        HasMultitouch = 2
    };

    Mode gesture;

    Point<float> finger1Anchor;
    Point<float> finger2Anchor;
    
    Point<float> finger1Position;
    Point<float> finger2Position;

    Point<float> center1;
    Point<float> center2;
    
    Point<float> zoomDiff;
    Point<float> dragDiff;

    bool finger1On;
    bool finger2On;

};

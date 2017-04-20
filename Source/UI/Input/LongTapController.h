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

#define LONGTAP_MILLISECONDS 400
#define LONGTAP_TOLERANCE 7

#include "LongTapListener.h"

class LongTapController :
    public Timer,
    public MouseListener
{
public:

    explicit LongTapController(LongTapListener &parent) :
        listener(parent)
    {}

    ~LongTapController() override
    {}

    void timerCallback() override
    {
        this->reset();
        this->listener.longTapEvent(*this->tapEvent);
    }

    void mouseDown(const MouseEvent &event) override
    {
        if (event.mods.isLeftButtonDown())
        {
            this->tapEvent = new MouseEvent(event);
            this->startTimer(LONGTAP_MILLISECONDS);
        }
    }

    void mouseDrag(const MouseEvent &event) override
    {
        if (this->isTimerRunning())
        {
            if (event.getDistanceFromDragStart() > LONGTAP_TOLERANCE)
            {
                this->reset();
            }
        }
    }

    void mouseUp(const MouseEvent &event) override
    { this->reset(); }

    void mouseExit(const MouseEvent &event) override
    { this->reset(); }

    void mouseDoubleClick(const MouseEvent &event) override
    { this->reset(); }

private:

    void reset()
    {
        if (this->isTimerRunning())
        {
            this->stopTimer();
        }
    }

    LongTapListener &listener;

    ScopedPointer<MouseEvent> tapEvent;
    
};

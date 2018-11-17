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

#include "SmoothPanListener.h"

#define SMOOTH_PAN_STOP_FACTOR 5
#define SMOOTH_PAN_SLOWDOWN_FACTOR 0.95f
#define SMOOTH_PAN_DISABLED 1

class SmoothPanController final : private Timer
{
public:

    explicit SmoothPanController(SmoothPanListener &parent) :
        listener(parent),
        origin(0, 0),
        target(0, 0)
    {}

    void cancelPan()
    {
        this->stopTimer();
    }

    void panByOffset(Point<int> offset)
    {
        #if SMOOTH_PAN_DISABLED

        this->listener.panByOffset(offset.getX(), offset.getY());

        #else

        this->origin = this->listener.getPanOffset().toFloat();
        this->target = offset.toFloat();

        if (!this->isTimerRunning())
        {
            this->startTimerHz(60);
            this->process();
        }
        else
        {
            this->process();
        }

        #endif
    }

private:

    void timerCallback() override
    {
        this->process();
    }

    inline void process()
    {
        const Point<float> &diff = this->target - this->origin;

        this->origin += (diff * SMOOTH_PAN_SLOWDOWN_FACTOR);

        this->listener.panByOffset(int(this->origin.getX()),
            int(this->origin.getY()));

        if (diff.getDistanceFromOrigin() < SMOOTH_PAN_STOP_FACTOR)
        {
            this->stopTimer();
        }
    }

    SmoothPanListener &listener;

    Point<float> origin;

    Point<float> target;

};

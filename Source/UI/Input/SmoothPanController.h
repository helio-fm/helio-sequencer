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

#if HELIO_DESKTOP
#    define SMOOTH_PAN_TIMER_DELAY_MS 7
#    define SMOOTH_PAN_STOP_FACTOR 15
#    define SMOOTH_PAN_REDUX_FACTOR 0.55f
#    define SMOOTH_PAN_SPEED_MULTIPLIER 1
#elif defined JUCE_IOS
#    define SMOOTH_PAN_TIMER_DELAY_MS 7
#    define SMOOTH_PAN_STOP_FACTOR 15
#    define SMOOTH_PAN_REDUX_FACTOR 0.55f
#    define SMOOTH_PAN_SPEED_MULTIPLIER 2
#elif defined JUCE_ANDROID
#    define SMOOTH_PAN_TIMER_DELAY_MS 7
#    define SMOOTH_PAN_STOP_FACTOR 15
#    define SMOOTH_PAN_REDUX_FACTOR 0.55f
#    define SMOOTH_PAN_SPEED_MULTIPLIER 1
#endif

#define SMOOTH_PAN_DISABLED 1

class SmoothPanController : private Timer
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
        //Logger::writeToLog("SmoothPan: " + String(offset.getX()) + ", " + String(offset.getY()));

        #if SMOOTH_PAN_DISABLED

        this->listener.panByOffset(offset.getX(), offset.getY());

        #else

        this->origin = this->listener.getPanOffset().toFloat();
        this->target = offset.toFloat();

        if (!this->isTimerRunning())
        {
            this->startTimer(SMOOTH_PAN_TIMER_DELAY_MS);
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

        this->origin += (diff * SMOOTH_PAN_REDUX_FACTOR);

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

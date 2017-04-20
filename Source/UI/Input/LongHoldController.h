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

#define LONG_HOLD_TIME_MS 500
#define LONG_HOLD_TOLERANCE 10

#include "LongHoldListener.h"

class LongHoldController :
    public Timer,
    public MouseListener
{
public:

    LongHoldController(LongHoldListener &parent) :
        listener(parent),
        tapEvent(nullptr)
    {}

    virtual ~LongHoldController() override
    {}

    virtual void timerCallback() override
    {
        this->stopTimer();
        this->listener.longHoldEvent(*this->tapEvent);
    }

    virtual void mouseMove(const MouseEvent &event) override
    {
        if (! this->tapEvent) { this->tapEvent = new MouseEvent(event); }

        Point<int> p(event.getPosition() - this->tapEvent->getPosition());

        if (p.getDistanceFromOrigin() > LONG_HOLD_TOLERANCE)
        {
            this->tapEvent = new MouseEvent(event);
            this->restartWait();
        }
    }

    virtual void mouseDown(const MouseEvent &event) override
    { this->cancelWait(); }

    virtual void mouseUp(const MouseEvent &event) override
    { this->cancelWait(); }

    virtual void mouseEnter(const MouseEvent &event) override
    {
        this->tapEvent = new MouseEvent(event);
    }

    virtual void mouseExit(const MouseEvent &event) override
    { this->cancelWait(); }

    virtual void mouseDoubleClick(const MouseEvent &event) override
    { this->cancelWait(); }

private:

    void cancelWait()
    {
        if (this->isTimerRunning())
        {
            this->stopTimer();
        }
    }

    void restartWait()
    {
        this->cancelWait();
        this->startTimer(LONG_HOLD_TIME_MS);
    }

    LongHoldListener &listener;

    ScopedPointer<MouseEvent> tapEvent;

};

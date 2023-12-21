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

#include "LongTapListener.h"

class LongTapController final : public Timer, public MouseListener
{
public:

    explicit LongTapController(LongTapListener &parent) :
        listener(parent) {}

    void timerCallback() override
    {
        this->reset();
        this->listener.onLongTap(this->position, this->component);
    }

    void mouseDown(const MouseEvent &e) override
    {
        if (e.mods.isLeftButtonDown())
        {
            this->position = e.mouseDownPosition;
            this->component = e.eventComponent;
            this->startTimer(LongTapController::delayMs);
        }
    }

    void mouseDrag(const MouseEvent &e) override
    {
        if (this->isTimerRunning())
        {
            const auto sqrDragDistance = e.mouseDownPosition.getDistanceSquaredFrom(e.position);
            if (sqrDragDistance > LongTapController::sqrThreshold)
            {
                this->reset();
            }
        }
    }

    void mouseUp(const MouseEvent &) override
    {
        this->reset();
    }

    void mouseExit(const MouseEvent &) override
    {
        this->reset();
    }

    void mouseDoubleClick(const MouseEvent &) override
    {
        this->reset();
    }

private:

    // both iOS and Android seem to have a default long tap delay of 500ms,
    // but we'll cut it down to 400ms because long-tapping is used a lot for selection:
    static constexpr auto delayMs = 400;
    static constexpr auto sqrThreshold = 16;

    inline void reset()
    {
        this->stopTimer();
    }

    LongTapListener &listener;

    Point<float> position;
    WeakReference<Component> component;

};

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

#include "SmoothPanListener.h"

class SmoothPanController final : private Timer
{
public:

    explicit SmoothPanController(SmoothPanListener &parent) :
        listener(parent) {}

    static inline float getInitialSpeed() noexcept
    {
        return SmoothPanController::initialPanSpeed;
    }

    void setAnimationsEnabled(bool enabled)
    {
        this->animationsEnabled = enabled;
    }

    void cancelPan()
    {
        this->stopTimer();
    }

    void panByOffset(Point<float> offset)
    {
        this->origin = this->listener.getPanOffset().toFloat();
        this->target = this->origin + offset.toFloat();

        if (!this->animationsEnabled)
        {
            this->listener.panByOffset(int(this->target.getX()),
                int(this->target.getY()));

            return;
        }

        if (!this->isTimerRunning())
        {
            this->startTimerHz(60);
            this->process();
        }
        else
        {
            this->process();
        }
    }

private:

    bool animationsEnabled = true;

    static constexpr auto initialPanSpeed = 150.f;

    void timerCallback() override
    {
        this->process();
    }

    inline void process()
    {
        const auto diff = this->target - this->origin;
        const auto delta = (diff * 0.5f).roundToInt();
        this->origin += delta.toFloat();

        const bool hitTheBorder = 
            this->listener.panByOffset(int(this->origin.getX()),
                int(this->origin.getY()));

        if (hitTheBorder || delta.isOrigin())
        {
            this->stopTimer();
        }
    }

    SmoothPanListener &listener;

    Point<float> origin = { 0.f, 0.f };
    Point<float> target = { 0.f, 0.f };

};

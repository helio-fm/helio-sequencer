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

class SmoothPanController final : private Timer
{
public:

    explicit SmoothPanController(SmoothPanListener &parent) :
        listener(parent) {}

    static inline float getInitialSpeed() noexcept
    {
        return SmoothPanController::initialPanSpeed;
    }

    void cancelPan()
    {
        this->stopTimer();
    }

    void panByOffset(Point<float> offset)
    {
        this->origin = this->listener.getPanOffset().toFloat();
        this->target = this->origin + offset.toFloat();

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

    static constexpr auto stopDistance = 5;
    static constexpr auto slowdownFactor = 0.5f;
    static constexpr auto initialPanSpeed = 155.f;

    void timerCallback() override
    {
        this->process();
    }

    inline void process()
    {
        const auto diff = this->target - this->origin;
        this->origin += (diff * SmoothPanController::slowdownFactor);

        const bool hitTheBorder = 
            this->listener.panByOffset(int(this->origin.getX()),
                int(this->origin.getY()));

        if (hitTheBorder || diff.getDistanceFromOrigin() < SmoothPanController::stopDistance)
        {
            this->stopTimer();
        }
    }

    SmoothPanListener &listener;

    Point<float> origin = { 0.f, 0.f };
    Point<float> target = { 0.f, 0.f };

};

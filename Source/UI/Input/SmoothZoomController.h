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

#include "SmoothZoomListener.h"

class SmoothZoomController final : private Timer
{
public:

    explicit SmoothZoomController(SmoothZoomListener &parent) noexcept :
        listener(parent) {}

    ~SmoothZoomController() override
    {
        this->stopTimer();
    }

    static inline float getInitialSpeed() noexcept
    {
        return Defaults::initialZoomSpeed;
    }

    void cancelZoom() noexcept
    {
        this->factorX = 0.f;
        this->factorY = 0.f;
        this->stopTimer();
    }

    void setAnimationsEnabled(bool enabled) noexcept
    {
        this->animated = enabled;
    }

    void zoomRelative(const Point<float> &from, const Point<float> &zoom) noexcept
    {
        if (this->animated)
        {
            this->inertialZoom =
                juce_hypot(this->factorX, this->factorY) >= (Defaults::zoomStopFactor * 10.f);

            this->factorX += zoom.getX();
            this->factorY += zoom.getY();
            this->originX = from.getX();
            this->originY = from.getY();

            this->startTimer(Defaults::timerDelay);
        }
        else
        {
            this->listener.zoomRelative(
                { from.getX(), from.getY() },
                { zoom.getX(), zoom.getY() },
                false);
        }
    }

private:

    inline bool stillNeedsZoom() const noexcept
    {
        return juce_hypot(this->factorX, this->factorY) >= Defaults::zoomStopFactor;
    }

    void timerCallback() override
    {
        if (this->stillNeedsZoom())
        {
            this->factorX = this->factorX * Defaults::zoomDecayFactor;
            this->factorY = this->factorY * Defaults::zoomDecayFactor;

            this->listener.zoomRelative(
                { this->originX, this->originY },
                { this->factorX, this->factorY },
                this->inertialZoom);

            this->inertialZoom = true;
        }
        else
        {
            this->cancelZoom();
        }
    }

    SmoothZoomListener &listener;

private:

    struct Defaults
    {
        static constexpr auto timerDelay = 6;
        static constexpr auto zoomDecayFactor = 0.71f;
        static constexpr auto zoomStopFactor = 0.001f;
        static constexpr auto initialZoomSpeed = 0.35f;
    };

    float factorX = 0.f;
    float factorY = 0.f;
    float originX = 0.f;
    float originY = 0.f;
    bool inertialZoom = false;
    bool animated = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SmoothZoomController)
};

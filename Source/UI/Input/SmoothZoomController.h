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

    explicit SmoothZoomController(SmoothZoomListener &parent) :
        listener(parent) {}

    ~SmoothZoomController() override
    {
        this->stopTimer();
    }

    static inline float getInitialSpeed() noexcept
    {
        return Defaults::initialZoomSpeed;
    }

    inline bool isZooming() const noexcept
    {
        return this->factorX != 0.f;
    }

    void cancelZoom() noexcept
    {
        this->factorX = 0.f;
        this->factorY = 0.f;
        this->stopTimer();
    }

    void setAnimationsEnabled(bool enabled)
    {
        this->animationDelay = enabled ?
            Defaults::timerDelay : Defaults::timerDelayNoAnim;

        this->zoomDecay = enabled ?
            Defaults::zoomDecayFactor : Defaults::zoomDecayFactorNoAnim;
    }

    void zoomRelative(const Point<float> &from, const Point<float> &zoom) noexcept
    {
        this->inertialZoom = this->isZooming();

        this->factorX += zoom.getX();
        this->factorY += zoom.getY();
        this->originX = from.getX();
        this->originY = from.getY();

        this->startTimer(this->animationDelay);
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
            this->factorX = this->factorX * this->zoomDecay;
            this->factorY = this->factorY * this->zoomDecay;

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
        static constexpr auto timerDelayNoAnim = 1;
        static constexpr auto zoomDecayFactor = 0.72f;
        static constexpr auto zoomDecayFactorNoAnim = 0.8f;
        static constexpr auto zoomStopFactor = 0.0005f;
        static constexpr auto initialZoomSpeed = 0.35f;
    };
    
    float factorX = 0.f;
    float factorY = 0.f;
    float originX = 0.f;
    float originY = 0.f;
    bool inertialZoom = false;

    int animationDelay = Defaults::timerDelay;
    float zoomDecay = Defaults::zoomDecayFactor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SmoothZoomController)
};

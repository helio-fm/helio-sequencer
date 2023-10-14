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

class SmoothZoomController final :
    private Thread,
    private WaitableEvent,
    private AsyncUpdater
{
public:

    explicit SmoothZoomController(SmoothZoomListener &parent) :
        Thread("SmoothZoom"),
        listener(parent)
    {
        this->startThread(9);
    }

    ~SmoothZoomController() override
    {
        this->signalThreadShouldExit();
        this->signal();
        this->stopThread(500);
    }

    static inline float getInitialSpeed() noexcept
    {
        return Defaults::initialZoomSpeed;
    }

    inline bool isZooming() const noexcept
    {
        return this->factorX.get() != 0.f;
    }

    void cancelZoom() noexcept
    {
        this->factorX = 0.f;
        this->factorY = 0.f;
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

        this->factorX = (this->factorX.get() + zoom.getX()) * Defaults::zoomSmoothFactor;
        this->factorY = (this->factorY.get() + zoom.getY()) * Defaults::zoomSmoothFactor;
        this->originX = from.getX();
        this->originY = from.getY();

        WaitableEvent::signal();
    }

private:

    inline bool stillNeedsZoom() const noexcept
    {
        return juce_hypot(this->factorX.get(), this->factorY.get()) >= Defaults::zoomStopFactor;
    }

    void run() override
    {
        while (! this->threadShouldExit())
        {
            while (this->stillNeedsZoom())
            {
                if (this->threadShouldExit())
                {
                    return;
                }

                this->factorX = this->factorX.get() * this->zoomDecay.get();
                this->factorY = this->factorY.get() * this->zoomDecay.get();

                this->triggerAsyncUpdate();

                Thread::sleep(this->animationDelay.get());
            }

            this->cancelZoom();

            if (this->threadShouldExit())
            {
                return;
            }

            WaitableEvent::wait();
        }
    }

    void handleAsyncUpdate() noexcept override
    {
        this->listener.zoomRelative(
            { this->originX.get(), this->originY.get() },
            { this->factorX.get(), this->factorY.get() },
            this->inertialZoom.get());

        this->inertialZoom = true;
    }

    SmoothZoomListener &listener;

private:

    struct Defaults
    {
        static constexpr auto timerDelay = 7;
        static constexpr auto timerDelayNoAnim = 1;
        static constexpr auto zoomDecayFactor = 0.725f;
        static constexpr auto zoomDecayFactorNoAnim = 0.8f;

        static constexpr auto zoomStopFactor = 0.0005f;
        static constexpr auto zoomSmoothFactor = 0.9f;
        static constexpr auto initialZoomSpeed = 0.35f;
    };
    
    Atomic<float> factorX = 0.f;
    Atomic<float> factorY = 0.f;
    Atomic<float> originX = 0.f;
    Atomic<float> originY = 0.f;
    Atomic<bool> inertialZoom = false;

    Atomic<int> animationDelay = Defaults::timerDelay;
    Atomic<float> zoomDecay = Defaults::zoomDecayFactor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SmoothZoomController)
};

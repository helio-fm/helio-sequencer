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

#include "SmoothZoomListener.h"

#define SMOOTH_ZOOM_TIMER_DELAY_MS 8
#define ZOOM_STOP_FACTOR 0.001f
#define ZOOM_DECAY_FACTOR 0.77f
#define ZOOM_INITIAL_SPEED 0.55f

class SmoothZoomController : private Thread, private WaitableEvent, private AsyncUpdater
{
public:

    explicit SmoothZoomController(SmoothZoomListener &parent) :
        Thread("SmoothZoomController"),
        listener(parent),
        factorX(0.f),
        factorY(0.f),
        originX(0.f),
        originY(0.f),
        timerDelay(SMOOTH_ZOOM_TIMER_DELAY_MS),
        zoomStopFactor(ZOOM_STOP_FACTOR),
        zoomDecayFactor(ZOOM_DECAY_FACTOR),
        initialZoomSpeed(ZOOM_INITIAL_SPEED)
    {
        this->startThread(5);
    }

    ~SmoothZoomController() override
    {
        this->signalThreadShouldExit();
        this->signal();
        this->stopThread(500);
    }

    inline int getTimerDelay() const noexcept { return timerDelay; }
    inline float getZoomStopFactor() const noexcept { return zoomStopFactor; }
    inline float getZoomDecayFactor() const noexcept { return zoomDecayFactor; }
    inline float getInitialZoomSpeed() const noexcept { return initialZoomSpeed; }

    bool isZooming() const
    {
        return fabs(this->factorX.get()) > 0.f;
    }

    void cancelZoom()
    {
        this->factorX = 0.f;
        this->factorY = 0.f;
    }

    void zoomRelative(const Point<float> &from, const Point<float> &zoom)
    {
        if (! this->isZooming())
        {
            this->factorX = zoom.getX();
            this->factorY = zoom.getY();
            this->originX = from.getX();
            this->originY = from.getY();
        }
        else
        {
            this->factorX = (this->factorX.get() + zoom.getX()) / 2.f;
            this->factorY = (this->factorY.get() + zoom.getY()) / 2.f;
            this->originX = (this->originX.get() + from.getX()) / 2.f;
            this->originY = (this->originY.get() + from.getY()) / 2.f;
        }

        WaitableEvent::signal();
    }

private:

    inline bool stillNeedsZoom() const
    {
        return juce_hypot(this->factorX.get(), this->factorY.get()) >= this->zoomStopFactor;
    }

    void run() override
    {
        while (! this->threadShouldExit())
        {
            while (this->stillNeedsZoom())
            {
                const double b = Time::getMillisecondCounterHiRes();
                this->factorX = this->factorX.get() * this->zoomDecayFactor;
                this->factorY = this->factorY.get() * this->zoomDecayFactor;

                this->triggerAsyncUpdate();
                
                if (this->threadShouldExit())
                {
                    return;
                }

                const double a = Time::getMillisecondCounterHiRes();
                const int skewTime = int(a - b);
                Thread::sleep(jlimit(10, 100, this->timerDelay - skewTime));
            }

            this->cancelZoom();
            WaitableEvent::wait();
        }
    }

    void handleAsyncUpdate() override
    {
        this->listener.zoomRelative({ this->originX.get(), this->originY.get() },
            { this->factorX.get(), this->factorY.get() });
    }

    SmoothZoomListener &listener;

    Atomic<float> factorX;
    Atomic<float> factorY;
    Atomic<float> originX;
    Atomic<float> originY;

private:

    int timerDelay;
    float zoomStopFactor;
    float zoomDecayFactor;
    float initialZoomSpeed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SmoothZoomController)
};

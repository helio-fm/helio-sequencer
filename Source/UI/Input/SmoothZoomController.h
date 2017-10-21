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

#if JUCE_LINUX
#    define SMOOTH_ZOOM_TIMER_DELAY_MS 7
#    define ZOOM_STOP_FACTOR 0.01f
#    define ZOOM_DECAY_FACTOR 0.75f
#    define ZOOM_INITIAL_SPEED 1.0f
#else
#    define SMOOTH_ZOOM_TIMER_DELAY_MS 7
#    define ZOOM_STOP_FACTOR 0.004f
#    define ZOOM_DECAY_FACTOR 0.7f
#    define ZOOM_INITIAL_SPEED 0.6f
#endif

class SmoothZoomController : private Thread, private WaitableEvent, private AsyncUpdater
{
public:

    explicit SmoothZoomController(SmoothZoomListener &parent) :
        Thread("SmoothZoomController"),
        listener(parent),
        factor(0.f, 0.f),
        origin(0, 0),
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
        this->stopThread(1000);
    }

    int getTimerDelay() const { return timerDelay; }
    void setTimerDelay(int val) { this->timerDelay = val; }

    float getZoomStopFactor() const { return zoomStopFactor; }
    void setZoomStopFactor(float val) { this->zoomStopFactor = val; }

    float getZoomDecayFactor() const { return zoomDecayFactor; }
    void setZoomReduxFactor(float val) { this->zoomDecayFactor = val; }

    float getInitialZoomSpeed() const { return initialZoomSpeed; }
    void setInitialZoomSpeed(float val) { this->initialZoomSpeed = val; }

    bool isZooming() const
    {
        ScopedReadLock lock(this->dataLock);
        return !this->factor.isOrigin();
    }

    void cancelZoom()
    {
        ScopedWriteLock lock(this->dataLock);
        this->factor.setXY(float(), float());
    }

    void zoomRelative(const Point<float> &from, const Point<float> &zoom)
    {
        if (! this->isZooming())
        {
            ScopedWriteLock lock(this->dataLock);
            this->factor = zoom;
            this->origin = from;
        }
        else
        {
            ScopedWriteLock lock(this->dataLock);
            this->factor = (this->factor + zoom) / 2.f;
            this->origin = (this->origin + from) / 2;
        }

        WaitableEvent::signal();
    }

private:

    inline bool stillNeedsZoom() const
    {
        ScopedReadLock lock(this->dataLock);
        return (this->factor.getDistanceFromOrigin() >= this->zoomStopFactor);
    }

    void run() override
    {
        while (! this->threadShouldExit())
        {
            while (this->stillNeedsZoom())
            {
                const double b = Time::getMillisecondCounterHiRes();

                {
                    ScopedWriteLock lock(this->dataLock);
                    this->factor *= this->zoomDecayFactor;
                }

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
        ScopedReadLock lock(this->dataLock);
        this->listener.zoomRelative(this->origin, this->factor);
    }


    SmoothZoomListener &listener;

    ReadWriteLock dataLock;
    Point<float> factor;
    Point<float> origin;

private:

    int timerDelay;
    float zoomStopFactor;
    float zoomDecayFactor;
    float initialZoomSpeed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SmoothZoomController)
};

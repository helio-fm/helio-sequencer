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

#include "Common.h"
#include "TransportIndicator.h"
#include "Transport.h"
#include "PianoRoll.h"
#include "PlayerThread.h"

#define FREE_SPACE 4


//#if PLAYER_THREAD_SENDS_SEEK_EVENTS
//#   define TRANSPORT_INDICATOR_ESTIMATES_MOVEMENT 0
//#else
#   define TRANSPORT_INDICATOR_ESTIMATES_MOVEMENT 1
//#endif

//#define TRANSPORT_INDICATOR_UPDATE_TIME_MS (1000 / 50)
#define TRANSPORT_INDICATOR_UPDATE_TIME_MS 7

// TODO: check deadlocks?

TransportIndicator::TransportIndicator(MidiRoll &parentRoll,
                                       Transport &owner,
                                       TransportIndicator::MovementListener *movementListener /*= nullptr*/,
                                       int width /*= 2*/) :
    roll(parentRoll),
    transport(owner),
    indicatorWidth(width + FREE_SPACE),
    lastCorrectPosition(0.0),
    timerStartTime(0.0),
    tempo(1.0),
    timerStartPosition(0.0),
    listener(movementListener)
{
    // debug
    //this->setName(String(Random::getSystemRandom().nextInt()));

    this->setInterceptsMouseClicks(false, false);
    this->setOpaque(false);
    this->setAlwaysOnTop(true);
    this->setSize(this->indicatorWidth, 1);

    this->lastCorrectPosition = this->transport.getSeekPosition();
    this->timerStartTime = Time::getMillisecondCounterHiRes();
    this->timerStartPosition = this->lastCorrectPosition;

    this->transport.addTransportListener(this);
}

TransportIndicator::~TransportIndicator()
{
    this->transport.removeTransportListener(this);
}


//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void TransportIndicator::onSeek(const double newPosition,
                                const double currentTimeMs,
                                const double totalTimeMs)
{
    //Logger::writeToLog("TransportIndicator::onSeek " + String(newPosition));
    //Logger::writeToLog(this->getName() + " onSeek newPosition = " + String(newPosition));

    {
        ScopedWriteLock lock(this->lastCorrectPositionLock);
        this->lastCorrectPosition = newPosition;
    }

    this->triggerAsyncUpdate();

#if TRANSPORT_INDICATOR_ESTIMATES_MOVEMENT
    if (this->isTimerRunning())
    {
        ScopedWriteLock lock(this->anchorsLock);
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
        //this->startTimer(TRANSPORT_INDICATOR_UPDATE_TIME_MS);
    }
#endif
}

void TransportIndicator::onTempoChanged(const double newTempo)
{
    //Logger::writeToLog("TransportIndicator::onTempoChanged " + String(newTempo));
#if TRANSPORT_INDICATOR_ESTIMATES_MOVEMENT
    ScopedWriteLock lock(this->anchorsLock);
    this->tempo = jmax(newTempo, 0.01);
        
    if (this->isTimerRunning())
    {
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
    }
#endif
}

void TransportIndicator::onTotalTimeChanged(const double timeMs)
{
}

void TransportIndicator::onPlay()
{
    //Logger::writeToLog("TransportIndicator::onPlay");
#if TRANSPORT_INDICATOR_ESTIMATES_MOVEMENT
    {
        ScopedWriteLock lock(this->anchorsLock);
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
    }

    //Logger::writeToLog("   !!!!! TransportIndicator startTimer");
    this->startTimer(TRANSPORT_INDICATOR_UPDATE_TIME_MS);
#endif
}

void TransportIndicator::onStop()
{
    //Logger::writeToLog("TransportIndicator::onStop");
#if TRANSPORT_INDICATOR_ESTIMATES_MOVEMENT
    this->stopTimer();

    {
        ScopedWriteLock lock(this->anchorsLock);
        this->timerStartTime = 0.0;
        this->timerStartPosition = 0.0;
    }
#endif
}


//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void TransportIndicator::timerCallback()
{
    //Logger::writeToLog("TransportIndicator::hiResTimerCallback");
//    MessageManagerLock lock(Thread::getCurrentThread());
//    if (lock.lockWasGained())
//    {
//        this->tick();
//    }

    this->triggerAsyncUpdate();
}


//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void TransportIndicator::handleAsyncUpdate()
{
    //Logger::writeToLog("TransportIndicator::handleAsyncUpdate");

    if (this->isTimerRunning())
    {
        this->tick();
    }
    else
    {
        double position;
        
        {
            ScopedReadLock lock(this->lastCorrectPositionLock);
            position = this->lastCorrectPosition;
        }
        
        this->updatePosition(position);
    }
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void TransportIndicator::paint(Graphics &g)
{
    //Logger::writeToLog("TransportIndicator::paint");
    const Colour indicatorColour(this->findColour(PianoRoll::indicatorColourId));
    const Colour indicatorShade(indicatorColour.withMultipliedBrightness(1.5f));
    
    g.setColour(indicatorColour);
    //g.fillRect(0, 0, this->getWidth() - FREE_SPACE, this->getHeight());
    g.drawVerticalLine(0, 0.f, float(this->getHeight()));

    g.setColour(indicatorShade);
    //g.setColour(Colours::black.withAlpha(0.5f));
    g.drawVerticalLine(1, 0.f, float(this->getHeight()));
}

void TransportIndicator::parentSizeChanged()
{
    //Logger::writeToLog("TransportIndicator::parentSizeChanged");
    this->parentChanged();
}

void TransportIndicator::parentHierarchyChanged()
{
    //Logger::writeToLog("TransportIndicator::parentHierarchyChanged");
    this->parentChanged();
}

void TransportIndicator::parentChanged()
{
    //Logger::writeToLog("TransportIndicator::parentChanged");
    this->setSize(this->indicatorWidth, this->getParentHeight());

    if (this->isTimerRunning())
    {
        //Logger::writeToLog("this->isTimerRunning()");
        this->tick();
    }
    else
    {
        double position;

        {
            ScopedReadLock lock(this->lastCorrectPositionLock);
            position = this->lastCorrectPosition;
        }
        
        this->updatePosition(position);
        this->toFront(false);
    }
}

void TransportIndicator::updatePosition(double position)
{
    //Logger::writeToLog("TransportIndicator::updatePosition " + String(position));
    //Logger::writeToLog("TransportIndicator::getParentWidth " + String(this->getParentWidth()));
    const int &newX = this->roll.getXPositionByTransportPosition(position, float(this->getParentWidth()));
    this->setTopLeftPosition(newX, 0);
    //this->setBounds(newX, 0, this->indicatorWidth, this->getParentHeight());

    if (this->listener != nullptr)
    {
        this->listener->onTransportIndicatorMoved(newX);
    }

    //Logger::writeToLog("TransportIndicator " + String(this->getX()) + " " + String(this->getY()) + " " + String(this->getWidth()) + " " + String(this->getHeight()));
}

void TransportIndicator::tick()
{
#if TRANSPORT_INDICATOR_ESTIMATES_MOVEMENT
    //Logger::writeToLog("TransportIndicator::tick");
    double estimatedPosition;
    
    {
        ScopedReadLock lock(this->anchorsLock);
        const double timeOffsetMs = Time::getMillisecondCounterHiRes() - this->timerStartTime;
        const double positionOffset = (timeOffsetMs / this->transport.getTotalTime()) / this->tempo;
        estimatedPosition = this->timerStartPosition + positionOffset;
    }
    
    this->updatePosition(estimatedPosition);
#endif
}


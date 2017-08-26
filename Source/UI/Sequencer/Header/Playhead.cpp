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
#include "Playhead.h"
#include "Transport.h"
#include "PianoRoll.h"
#include "PlayerThread.h"

#define FREE_SPACE 4


//#if PLAYER_THREAD_SENDS_SEEK_EVENTS
//#   define PLAYHEAD_ESTIMATES_MOVEMENT 0
//#else
#   define PLAYHEAD_ESTIMATES_MOVEMENT 1
//#endif

//#define PLAYHEAD_UPDATE_TIME_MS (1000 / 50)
#define PLAYHEAD_UPDATE_TIME_MS 7

// TODO: check deadlocks?

Playhead::Playhead(HybridRoll &parentRoll,
    Transport &owner,
    Playhead::Listener *movementListener /*= nullptr*/,
    int width /*= 2*/) :
    roll(parentRoll),
    transport(owner),
    playheadWidth(width + FREE_SPACE),
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
    this->setSize(this->playheadWidth, 1);

    this->lastCorrectPosition = this->transport.getSeekPosition();
    this->timerStartTime = Time::getMillisecondCounterHiRes();
    this->timerStartPosition = this->lastCorrectPosition;

    this->transport.addTransportListener(this);
}

Playhead::~Playhead()
{
    this->transport.removeTransportListener(this);
}


//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void Playhead::onSeek(const double newPosition,
                                const double currentTimeMs,
                                const double totalTimeMs)
{
    //Logger::writeToLog("Playhead::onSeek " + String(newPosition));
    //Logger::writeToLog(this->getName() + " onSeek newPosition = " + String(newPosition));

    {
        ScopedWriteLock lock(this->lastCorrectPositionLock);
        this->lastCorrectPosition = newPosition;
    }

    this->triggerAsyncUpdate();

#if PLAYHEAD_ESTIMATES_MOVEMENT
    if (this->isTimerRunning())
    {
        ScopedWriteLock lock(this->anchorsLock);
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
        //this->startTimer(PLAYHEAD_UPDATE_TIME_MS);
    }
#endif
}

void Playhead::onTempoChanged(const double newTempo)
{
    //Logger::writeToLog("Playhead::onTempoChanged " + String(newTempo));
#if PLAYHEAD_ESTIMATES_MOVEMENT
    ScopedWriteLock lock(this->anchorsLock);
    this->tempo = jmax(newTempo, 0.01);
        
    if (this->isTimerRunning())
    {
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
    }
#endif
}

void Playhead::onTotalTimeChanged(const double timeMs)
{
}

void Playhead::onPlay()
{
    //Logger::writeToLog("Playhead::onPlay");
#if PLAYHEAD_ESTIMATES_MOVEMENT
    {
        ScopedWriteLock lock(this->anchorsLock);
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
    }

    //Logger::writeToLog("   !!!!! Playhead startTimer");
    this->startTimer(PLAYHEAD_UPDATE_TIME_MS);
#endif
}

void Playhead::onStop()
{
    //Logger::writeToLog("Playhead::onStop");
#if PLAYHEAD_ESTIMATES_MOVEMENT
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

void Playhead::timerCallback()
{
    //Logger::writeToLog("Playhead::hiResTimerCallback");
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

void Playhead::handleAsyncUpdate()
{
    //Logger::writeToLog("Playhead::handleAsyncUpdate");

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

void Playhead::paint(Graphics &g)
{
    //Logger::writeToLog("Playhead::paint");
    const Colour playheadColour(this->findColour(PianoRoll::playheadColourId));
    const Colour playheadShade(playheadColour.withMultipliedBrightness(1.5f));
    
    g.setColour(playheadColour);
    //g.fillRect(0, 0, this->getWidth() - FREE_SPACE, this->getHeight());
    g.drawVerticalLine(0, 0.f, float(this->getHeight()));

    g.setColour(playheadShade);
    //g.setColour(Colours::black.withAlpha(0.5f));
    g.drawVerticalLine(1, 0.f, float(this->getHeight()));
}

void Playhead::parentSizeChanged()
{
    //Logger::writeToLog("Playhead::parentSizeChanged");
    this->parentChanged();
}

void Playhead::parentHierarchyChanged()
{
    //Logger::writeToLog("Playhead::parentHierarchyChanged");
    this->parentChanged();
}

void Playhead::parentChanged()
{
    if (this->getParentComponent() != nullptr)
    {
        //Logger::writeToLog("Playhead::parentChanged");
        this->setSize(this->playheadWidth, this->getParentHeight());
        
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
}

void Playhead::updatePosition(double position)
{
    //Logger::writeToLog("Playhead::updatePosition " + String(position));
    //Logger::writeToLog("Playhead::getParentWidth " + String(this->getParentWidth()));
    const int &newX = this->roll.getXPositionByTransportPosition(position, float(this->getParentWidth()));
    this->setTopLeftPosition(newX, 0);
    //this->setBounds(newX, 0, this->indicatorWidth, this->getParentHeight());

    if (this->listener != nullptr)
    {
        this->listener->onPlayheadMoved(newX);
    }

    //Logger::writeToLog("Playhead " + String(this->getX()) + " " + String(this->getY()) + " " + String(this->getWidth()) + " " + String(this->getHeight()));
}

void Playhead::tick()
{
#if PLAYHEAD_ESTIMATES_MOVEMENT
    //Logger::writeToLog("Playhead::tick");
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


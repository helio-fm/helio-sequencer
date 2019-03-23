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
#include "HybridRoll.h"
#include "ColourIDs.h"
#include "PlayerThread.h"

#define FREE_SPACE 2

//#define PLAYHEAD_UPDATE_TIME_MS (1000 / 50)
#define PLAYHEAD_UPDATE_TIME_MS 7

Playhead::Playhead(HybridRoll &parentRoll,
    Transport &owner,
    Playhead::Listener *movementListener /*= nullptr*/,
    int width /*= 2*/) :
    roll(parentRoll),
    transport(owner),
    playheadWidth(width + FREE_SPACE),
    lastCorrectPosition(0.0),
    timerStartTime(0.0),
    msPerQuarterNote(1.0),
    timerStartPosition(0.0),
    listener(movementListener)
{
    this->mainColour = findDefaultColour(ColourIDs::Roll::playhead);
    this->shadeColour = findDefaultColour(ColourIDs::Roll::playheadShade);

    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
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

void Playhead::onSeek(double absolutePosition,
    double currentTimeMs, double totalTimeMs)
{
    {
        SpinLock::ScopedLockType lock(this->lastCorrectPositionLock);
        this->lastCorrectPosition = absolutePosition;
    }

    this->triggerAsyncUpdate();

    if (this->isTimerRunning())
    {
        SpinLock::ScopedLockType lock(this->anchorsLock);
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
        //this->startTimer(PLAYHEAD_UPDATE_TIME_MS);
    }
}

void Playhead::onTempoChanged(double msPerQuarter)
{
    SpinLock::ScopedLockType lock(this->anchorsLock);
    this->msPerQuarterNote = jmax(msPerQuarter, 0.01);
        
    if (this->isTimerRunning())
    {
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
    }
}

void Playhead::onTotalTimeChanged(double timeMs)
{
}

void Playhead::onPlay()
{
    {
        SpinLock::ScopedLockType lock(this->anchorsLock);
        this->timerStartTime = Time::getMillisecondCounterHiRes();
        this->timerStartPosition = this->lastCorrectPosition;
    }

    this->startTimer(PLAYHEAD_UPDATE_TIME_MS);
}

void Playhead::onStop()
{
    this->stopTimer();

    {
        SpinLock::ScopedLockType lock(this->anchorsLock);
        this->timerStartTime = 0.0;
        this->timerStartPosition = 0.0;
    }
}


//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void Playhead::timerCallback()
{
    this->triggerAsyncUpdate();
}


//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void Playhead::handleAsyncUpdate()
{
    if (this->isTimerRunning())
    {
        this->tick();
    }
    else
    {
        double position;
        
        {
            SpinLock::ScopedLockType lock(this->lastCorrectPositionLock);
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
    g.setColour(this->mainColour);
    g.drawVerticalLine(0, 0.f, float(this->getHeight()));

    g.setColour(this->shadeColour);
    g.drawVerticalLine(1, 0.f, float(this->getHeight()));
}

void Playhead::parentSizeChanged()
{
    this->parentChanged();
}

void Playhead::parentHierarchyChanged()
{
    this->parentChanged();
}

void Playhead::parentChanged()
{
    if (this->getParentComponent() != nullptr)
    {
        this->setSize(this->playheadWidth, this->getParentHeight());
        
        if (this->isTimerRunning())
        {
            this->tick();
        }
        else
        {
            double position;
            
            {
                SpinLock::ScopedLockType lock(this->lastCorrectPositionLock);
                position = this->lastCorrectPosition;
            }
            
            this->updatePosition(position);
            this->toFront(false);
        }
    }
}

void Playhead::updatePosition(double position)
{
    const int &newX = this->roll.getXPositionByTransportPosition(position, double(this->getParentWidth()));
    this->setTopLeftPosition(newX, 0);

    if (this->listener != nullptr)
    {
        this->listener->onPlayheadMoved(newX);
    }
}

void Playhead::tick()
{
    double estimatedPosition;
    
    {
        SpinLock::ScopedLockType lock(this->anchorsLock);
        const double timeOffsetMs = Time::getMillisecondCounterHiRes() - this->timerStartTime;
        const double positionOffset = (timeOffsetMs / this->transport.getTotalTime()) / this->msPerQuarterNote;
        estimatedPosition = this->timerStartPosition + positionOffset;
    }
    
    this->updatePosition(estimatedPosition);
}


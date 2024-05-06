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

#include "Common.h"
#include "Playhead.h"
#include "Transport.h"
#include "RollBase.h"
#include "ColourIDs.h"
#include "PlayerThread.h"

Playhead::Playhead(RollBase &parentRoll,
    Transport &owner,
    Playhead::Listener *movementListener /*= nullptr*/,
    float alpha /*= 1.f*/) :
    roll(parentRoll),
    transport(owner),
    listener(movementListener),
    shadeColour(findDefaultColour(ColourIDs::Roll::playheadShade)),
    playbackColour(findDefaultColour(ColourIDs::Roll::playheadPlayback).withMultipliedAlpha(alpha)),
    recordingColour(findDefaultColour(ColourIDs::Roll::playheadRecording).withMultipliedAlpha(alpha))
{
    this->currentColour = this->playbackColour;

    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    this->setAlwaysOnTop(true);
    this->setSize(3, 1); // add some horizontal padding, trying to avoid glitches

    this->lastCorrectBeat = this->transport.getSeekBeat();
    this->beatAnchor = this->lastCorrectBeat;
    this->timeAnchor = Time::getMillisecondCounter();

    this->transport.addTransportListener(this);
}

Playhead::~Playhead()
{
    this->transport.removeTransportListener(this);
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void Playhead::onSeek(float beatPosition)
{
    const SpinLock::ScopedLockType lock(this->moveLock);

    this->lastCorrectBeat = beatPosition;

    if (this->isTimerRunning())
    {
        this->timeAnchor = Time::getMillisecondCounter();
        this->beatAnchor = this->lastCorrectBeat;
    }
    else
    {
        this->triggerAsyncUpdate();
    }
}

void Playhead::onCurrentTempoChanged(double msPerQuarter)
{
    const SpinLock::ScopedLockType lock(this->moveLock);

    jassert(msPerQuarter >= 0.01);
    this->msPerQuarterNote = jmax(msPerQuarter, 0.01);

    if (this->isTimerRunning())
    {
        // expects that lastCorrectBeat has been set
        // just before calling this function:
        this->timeAnchor = Time::getMillisecondCounter();
        this->beatAnchor = this->lastCorrectBeat;
    }
    else
    {
        this->triggerAsyncUpdate();
    }
}

void Playhead::onPlay()
{
    this->timeAnchor = Time::getMillisecondCounter();
    this->beatAnchor = this->lastCorrectBeat;
    this->startTimerHz(60);
}

void Playhead::onRecord()
{
    this->currentColour = this->recordingColour;
    this->repaint();
}

void Playhead::onStop()
{
    this->currentColour = this->playbackColour;
    this->repaint();

    this->stopTimer();

    this->timeAnchor = 0.0;
    this->beatAnchor = 0.0;
    this->msPerQuarterNote = Globals::Defaults::msPerBeat;
}

//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void Playhead::timerCallback()
{
    const SpinLock::ScopedLockType lock(this->moveLock);
    this->updatePosition();
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void Playhead::handleAsyncUpdate()
{
    if (!this->isTimerRunning())
    {
        this->updatePosition();
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void Playhead::paint(Graphics &g)
{
    g.setColour(this->currentColour);
    g.fillRect(0, 1, 1, this->getHeight() - 1);

    g.setColour(this->shadeColour);
    g.fillRect(1, 1, 1, this->getHeight() - 1);
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
        this->setSize(this->getWidth(), this->getParentHeight());
        this->updatePosition();
        this->toFront(false);
    }
}

void Playhead::updatePosition(float position)
{
    const auto oldX = this->getX();
    const int newX = this->roll.getXPositionByBeat(position, float(this->getParentWidth()));

    this->setTopLeftPosition(newX, 0);

    if (this->listener != nullptr)
    {
        this->listener->onMovePlayhead(oldX, newX);
    }
}

void Playhead::updatePosition()
{
    if (this->isTimerRunning())
    {
        const double timeOffsetMs = Time::getMillisecondCounter() - this->timeAnchor;
        const double positionOffset = timeOffsetMs / this->msPerQuarterNote;
        const float estimatedPosition = float(this->beatAnchor + positionOffset);
        this->updatePosition(estimatedPosition);
    }
    else
    {
        this->updatePosition(this->lastCorrectBeat);
    }
}

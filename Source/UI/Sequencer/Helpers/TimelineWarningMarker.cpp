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
#include "TimelineWarningMarker.h"
#include "RollBase.h"

TimelineWarningMarker::TimelineWarningMarker(WarningLevel warningLevel,
    RollBase &parentRoll, float initialBeatPosition) :
    roll(parentRoll),
    colour((warningLevel == WarningLevel::Red) ? Colour(0x33ff0000) : Colour(0x27ffff00)),
    startBeat(initialBeatPosition),
    endBeat(initialBeatPosition + TimelineWarningMarker::minSizeInBeats)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

void TimelineWarningMarker::paint(Graphics &g)
{
    g.setColour(this->colour);
    g.fillRect(this->getLocalBounds());
}

void TimelineWarningMarker::parentHierarchyChanged()
{
    this->updateBounds();
    this->toBack();
}

void TimelineWarningMarker::parentSizeChanged()
{
    this->updateBounds();
}

void TimelineWarningMarker::updateBounds()
{
    const int startX = this->roll.getXPositionByBeat(this->startBeat);
    const int endX = this->roll.getXPositionByBeat(this->endBeat);
    this->setBounds(startX, 0, endX - startX, this->getParentHeight());
}

float TimelineWarningMarker::getStartBeat() const noexcept
{
    return this->startBeat;
}

void TimelineWarningMarker::setStartBeat(float beat)
{
    this->startBeat = beat;
    this->updateBounds();
}

float TimelineWarningMarker::getEndBeat() const noexcept
{
    return this->endBeat;
}

void TimelineWarningMarker::setEndBeat(float beat)
{
    this->endBeat = jmax(beat,  this->startBeat + TimelineWarningMarker::minSizeInBeats);
    this->updateBounds();
}

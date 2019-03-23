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
#include "TimelineWarningMarker.h"
#include "HybridRoll.h"

TimelineWarningMarker::TimelineWarningMarker(WarningLevel warningLevel, HybridRoll &parentRoll, float inititalBeatPosition)
    : roll(parentRoll),
      colour((warningLevel == Red) ? Colour(0x33ff0000) : Colour(0x27ffff00)),
      startBeat(inititalBeatPosition),
      endBeat(inititalBeatPosition + CLIPPING_MARKER_MIN_SIZE_IN_BEATS)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->setAlpha(0.f);
}

void TimelineWarningMarker::paint (Graphics& g)
{
    g.setColour(this->colour);
    g.fillRect(this->getLocalBounds());
}

void TimelineWarningMarker::parentHierarchyChanged()
{
    this->fader.fadeIn(this, 300);
    this->updatePosition();
    this->toBack();
}

void TimelineWarningMarker::parentSizeChanged()
{
    this->updatePosition();
}

void TimelineWarningMarker::updatePosition()
{
    const int startX = this->roll.getXPositionByBeat(this->startBeat);
    const int endX = this->roll.getXPositionByBeat(this->endBeat);
    const int newWidth = endX - startX;
    this->setBounds(startX, 0, newWidth, this->getParentHeight());
}

float TimelineWarningMarker::getStartBeat() const noexcept
{
    return this->startBeat;
}

void TimelineWarningMarker::setStartBeat(float beat)
{
    this->startBeat = beat;
    this->updatePosition();
}

float TimelineWarningMarker::getEndBeat() const noexcept
{
    return this->endBeat;
}

void TimelineWarningMarker::setEndBeat(float beat)
{
    this->endBeat = beat;
    this->updatePosition();
}


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
#include "HybridRollExpandMark.h"
#include "HybridRoll.h"

HybridRollExpandMark::HybridRollExpandMark(HybridRoll &parentRoll, float targetBeat, int numBeatsToTake) :
    roll(parentRoll),
    beat(targetBeat),
    numBeats(numBeatsToTake)
{
    this->plusImage = make<IconComponent>(Icons::expand);
    this->addAndMakeVisible(this->plusImage.get());

    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    this->startTimerHz(60);
}

HybridRollExpandMark::~HybridRollExpandMark() = default;

void HybridRollExpandMark::paint(Graphics &g)
{
    g.setColour(findDefaultColour(Label::textColourId).withAlpha(0.15f * this->alpha));
    g.fillRect(this->getLocalBounds());
}

void HybridRollExpandMark::resized()
{
    constexpr auto iconSize = 16;
    this->plusImage->setBounds((this->getWidth() / 2) - (iconSize / 2),
        (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);
}

void HybridRollExpandMark::parentHierarchyChanged()
{
    this->updatePosition();
}

void HybridRollExpandMark::parentSizeChanged()
{
    this->updatePosition();
}

void HybridRollExpandMark::updatePosition()
{
    const float beatOffset = this->beat - this->roll.getFirstBeat();
    const int xOffset = int(beatOffset * this->roll.getBeatWidth());
    const int newWidth = int(this->roll.getBeatWidth() * this->numBeats);
    this->setBounds(xOffset, 0, newWidth, this->getParentHeight());
}

void HybridRollExpandMark::timerCallback()
{
    this->alpha *= 0.945f;

    if (this->alpha <= 0.02f)
    {
        delete this;
    }
    else
    {
        this->setAlpha(this->alpha);
    }
}
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
#include "RollExpandMark.h"
#include "RollBase.h"

RollExpandMark::RollExpandMark(RollBase &parentRoll, float targetBeat, float numBeatsToTake, bool showPlusIcon) :
    roll(parentRoll),
    beat(targetBeat),
    numBeats(numBeatsToTake)
{
    //this->plusImage = make<IconComponent>(Icons::expand);
    //this->addChildComponent(this->plusImage.get());
    //this->plusImage->setVisible(showPlusIcon);

    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    this->startTimerHz(60);
}

RollExpandMark::~RollExpandMark()
{
    this->stopTimer();
}

void RollExpandMark::paint(Graphics &g)
{
    g.setColour(this->colour.withAlpha(0.2f * this->alpha));
    g.fillRect(this->getLocalBounds());
}

void RollExpandMark::resized()
{
    //constexpr auto iconSize = 16;
    //this->plusImage->setBounds((this->getWidth() / 2) - (iconSize / 2),
    //    (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);
}

void RollExpandMark::parentHierarchyChanged()
{
    this->updatePosition();
}

void RollExpandMark::parentSizeChanged()
{
    this->updatePosition();
}

void RollExpandMark::updatePosition()
{
    const float beatOffset = this->beat - this->roll.getFirstBeat();
    const int xOffset = int(beatOffset * this->roll.getBeatWidth());
    const int newWidth = int(this->roll.getBeatWidth() * this->numBeats);
    this->setBounds(xOffset, 0, newWidth, this->getParentHeight());
}

void RollExpandMark::timerCallback()
{
    this->alpha *= 0.915f;

    if (this->alpha <= 0.02f)
    {
        this->stopTimer();
        UniquePointer<Component> deleter(this);
    }
    else
    {
        //this->plusImage->setIconAlphaMultiplier(this->alpha);
        this->repaint();
    }
}

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
#include "LevelsMapScroller.h"
#include "RollBase.h"
#include "ColourIDs.h"
#include "HelioTheme.h"

LevelsMapScroller::LevelsMapScroller(SafePointer<RollBase> roll) : roll(roll)
{
    this->setPaintingIsUnclipped(false); // cut dragger may and will go out of bounds
    this->setInterceptsMouseClicks(true, true);
    this->setOpaque(true);
}

void LevelsMapScroller::addOwnedMap(Component *newTrackMap)
{
    this->trackMaps.add(newTrackMap);
    this->addAndMakeVisible(newTrackMap);
    newTrackMap->toFront(false);
}

void LevelsMapScroller::removeOwnedMap(Component *existingTrackMap)
{
    if (this->trackMaps.contains(existingTrackMap))
    {
        this->removeChildComponent(existingTrackMap);
        this->trackMaps.removeObject(existingTrackMap);
        this->resized();
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void LevelsMapScroller::resized()
{
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

void LevelsMapScroller::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getBgCacheC(), {} });

    g.fillRect(this->getLocalBounds()); //draw the background

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineDark));
    g.fillRect(0, 0, this->getWidth(), 1);

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineLight));
    g.fillRect(0, 1, this->getWidth(), 1);

    g.setColour(Colours::white.withOpacity(0.04f)); //replace with properly contrasting color someday
    g.fillRect(0, int(this->getHeight()*0.25), this->getWidth(), 1);    //draw 75% line
    g.fillRect(0, int(this->getHeight() * 0.50), this->getWidth(), 1);  //draw 50% line
    g.fillRect(0, int(this->getHeight() * 0.75), this->getWidth(), 1);  //draw 25% line

    g.setColour(Colours::black.withOpacity(0.04f)); //replace with properly contrasting color someday
    g.fillRect(0, int(this->getHeight() * 0.25 + 2), this->getWidth(), 1);    //draw 75% shadow
    g.fillRect(0, int(this->getHeight() * 0.50 + 2), this->getWidth(), 1);  //draw 50% shadow
    g.fillRect(0, int(this->getHeight() * 0.75 + 2), this->getWidth(), 1);  //draw 25% shadow
}

void LevelsMapScroller::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    if (this->roll != nullptr)
    {
        this->roll->mouseWheelMove(event.getEventRelativeTo(this->roll), wheel);
    }
}

//===----------------------------------------------------------------------===//
// MidiRollListener
//===----------------------------------------------------------------------===//

void LevelsMapScroller::onMidiRollMoved(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

void LevelsMapScroller::onMidiRollResized(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void LevelsMapScroller::handleAsyncUpdate()
{
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Rectangle<int> LevelsMapScroller::getMapBounds() const noexcept
{
    if (this->roll != nullptr)
    {
        const auto viewX = this->roll->getViewport().getViewPositionX();
        return { -viewX, 0, this->roll->getWidth(), this->getHeight() };
    }

    return { 0, 0, 0, 0 };
}

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
#include "Playhead.h"
#include "Transport.h"
#include "HybridRoll.h"
#include "PanelBackgroundC.h"
#include "Origami.h"
#include "ColourIDs.h"
#include "HelioTheme.h"
#include "PianoProjectMap.h"

LevelsMapScroller::LevelsMapScroller(SafePointer<HybridRoll> roll) :
    roll(roll)
{
    this->setPaintingIsUnclipped(true);
    this->setOpaque(true);
    this->resized();
}

void LevelsMapScroller::addOwnedMap(Component *newTrackMap)
{
    this->trackMaps.add(newTrackMap);
    this->addAndMakeVisible(newTrackMap);
    newTrackMap->toFront(false);
    this->resized();
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
    const auto mapBounds = this->getMapBounds();
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

void LevelsMapScroller::paint(Graphics &g)
{
    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    g.setFillType({ theme.getBgCacheC(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineDark));
    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineLight));
    g.drawHorizontalLine(1, 0.f, float(this->getWidth()));
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

void LevelsMapScroller::onMidiRollMoved(HybridRoll *targetRoll)
{
    if (this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

void LevelsMapScroller::onMidiRollResized(HybridRoll *targetRoll)
{
    if (this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void LevelsMapScroller::handleAsyncUpdate()
{
    const auto mapBounds = this->getMapBounds();
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
        const float viewX = float(this->roll->getViewport().getViewPositionX());
        const float viewWidth = float(this->roll->getViewport().getViewWidth());
        const float rollWidth = float(this->roll->getWidth());
        const float rollInvisibleArea = rollWidth - viewWidth;
        const float trackWidth = float(this->getWidth());
        const float trackInvisibleArea = float(this->getWidth() - 200);
        const float mapWidth = ((200 * rollWidth) / viewWidth);

        const float rX = ((trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth));
        const float dX = (viewX * mapWidth) / rollWidth;
        return { int(rX - dX), 0, int(mapWidth), this->getHeight() };
    }

    return { 0, 0, 0, 0 };
}

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
#include "EditorPanelsScroller.h"
#include "RollBase.h"
#include "ColourIDs.h"
#include "HelioTheme.h"

EditorPanelsScroller::EditorPanelsScroller(SafePointer<RollBase> roll) : roll(roll)
{
    this->setPaintingIsUnclipped(false); // the cut dragger may and will go out of bounds
    this->setInterceptsMouseClicks(true, true);
    this->setOpaque(true);
}

void EditorPanelsScroller::switchToRoll(SafePointer<RollBase> roll)
{
    this->roll = roll;
    for (auto *map : this->trackMaps)
    {
        map->switchToRoll(roll);
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::resized()
{
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

void EditorPanelsScroller::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getSidebarBackground(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineDark));
    g.fillRect(0, 0, this->getWidth(), 1);

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineLight));
    g.fillRect(0, 1, this->getWidth(), 1);
}

void EditorPanelsScroller::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    if (this->roll != nullptr)
    {
        this->roll->mouseWheelMove(event.getEventRelativeTo(this->roll), wheel);
    }
}

//===----------------------------------------------------------------------===//
// MidiRollListener
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::onMidiRollMoved(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

void EditorPanelsScroller::onMidiRollResized(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::handleAsyncUpdate()
{
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Rectangle<int> EditorPanelsScroller::getMapBounds() const noexcept
{
    if (this->roll != nullptr)
    {
        const auto viewX = this->roll->getViewport().getViewPositionX();
        return { -viewX, 0, this->roll->getWidth(), this->getHeight() };
    }

    return {};
}

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

#pragma once

class MidiEvent;
class MidiLayer;
class Pattern;
class Transport;
class ProjectTreeItem;

// TODO rename methods
// More TODOS:
// Rename LayerTreeItem to MidiLayerTreeItem
// Rename MidiLayer as MidiSequence, PianoSequence, AutomationSequence,
// or as MidiTrack, PianoTrack, AutomationTrack, etc.

class ProjectEventDispatcher
{
public:

    virtual ~ProjectEventDispatcher() {}

    virtual void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) = 0;

    virtual void onEventAdded(const MidiEvent &event) = 0;

    virtual void onEventRemoved(const MidiEvent &event) = 0;

    virtual void onEventRemovedPostAction(const MidiLayer *layer) {}

    virtual void onLayerChanged(const MidiLayer *layer) = 0;

    virtual void onBeatRangeChanged() = 0;

    virtual ProjectTreeItem *getProject() const { return nullptr; }

};

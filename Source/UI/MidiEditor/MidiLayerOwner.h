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
class Transport;
class ProjectTreeItem;

// duplicates methods from ProjectListener :(
// events pass this way:
// note -> layer -> tree item(layer owner) -> project -> listeners
class MidiLayerOwner
{
public:

    static const String xPathSeparator;
    
    virtual ~MidiLayerOwner() {}

    virtual Transport *getTransport() const = 0;

    virtual String getXPath() const = 0; // i.e. "Main Part/Modulation/Arpeggio"

    virtual void setXPath(const String &path) = 0;

    virtual void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) = 0;

    virtual void onEventAdded(const MidiEvent &event) = 0;

    virtual void onEventRemoved(const MidiEvent &event) = 0;

    virtual void onEventRemovedPostAction(const MidiLayer *layer) {}

    virtual void onLayerChanged(const MidiLayer *layer) = 0;

    virtual void onBeatRangeChanged() = 0;

    virtual void activateLayer(MidiLayer* layer, bool selectOthers, bool deselectOthers) {}

    virtual ProjectTreeItem *getProject() const { return nullptr; }

};

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

#include "AutomationEvent.h"
#include "ProjectListener.h"
#include "AutomationTrackMap.h"

class MidiRoll;
class ProjectTreeItem;
class TriggerEventComponent;
class TriggerEventConnector;


//===----------------------------------------------------------------------===//
// Automation editor that treats layer as on/off toggle events
//===----------------------------------------------------------------------===//

class TriggersTrackMap : public AutomationTrackMapCommon
{
public:
    
    TriggersTrackMap(ProjectTreeItem &parentProject, MidiRoll &parentRoll, WeakReference<MidiLayer> targetLayer);
    
    ~TriggersTrackMap() override;
    
    void reloadTrack() override;

    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void mouseDown(const MouseEvent &e) override;
    
    void resized() override;
    
    //virtual void paint(Graphics &g) override;
    
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;

    
    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//
    
    void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    
    void onEventAdded(const MidiEvent &event) override;
    
    void onEventRemoved(const MidiEvent &event) override;
    
    void onLayerChanged(const MidiLayer *layer) override;
    
    void onLayerAdded(const MidiLayer *layer) override;
    
    void onLayerRemoved(const MidiLayer *layer) override;
    
    void onProjectBeatRangeChanged(float firstBeat, float lastBeat) override;
    
protected:
    
    void insertNewEventAt(const MouseEvent &e, bool shouldAddTriggeredEvent);
    void removeEventIfPossible(const AutomationEvent &e);
    
    float getBeatByXPosition(int x) const;
    
    TriggerEventComponent *getPreviousEventComponent(int indexOfSorted) const;
    TriggerEventComponent *getNextEventComponent(int indexOfSorted) const;
    
    Rectangle<float> getEventBounds(TriggerEventComponent *c) const;
    Rectangle<float> getEventBounds(float targetBeat, bool isPedalDown, float anchor) const;
    
    friend class TriggerEventComponent;
    friend class TriggerEventConnector;
    
private:
    
    void updateEventComponent(TriggerEventComponent *component);
    
    float projectFirstBeat;
    float projectLastBeat;
    
    float rollFirstBeat;
    float rollLastBeat;
    
    MidiRoll &roll;
    ProjectTreeItem &project;

    WeakReference<MidiLayer> layer;

    ScopedPointer<TriggerEventConnector> leadingConnector;

    OwnedArray<TriggerEventComponent> eventComponents;
    HashMap<AutomationEvent, TriggerEventComponent *, AutomationEventHashFunction> eventsHash;
    
};

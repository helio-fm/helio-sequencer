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

class MidiRoll;
class ProjectTreeItem;
class AutomationCurveHelper;
class AutomationEventComponent;
class ComponentConnectorCurve;


class AutomationTrackMapCommon : public Component, public ProjectListener
{
public:
    
    virtual void reloadTrack() = 0;
};


class AutomationTrackMap : public AutomationTrackMapCommon
{
public:
    
    AutomationTrackMap(ProjectTreeItem &parentProject, MidiRoll &parentRoll, WeakReference<MidiLayer> targetLayer);
    
    ~AutomationTrackMap() override;
    
    void insertNewEventAt(const MouseEvent &e);

    void reloadTrack() override;

    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void mouseDown(const MouseEvent &e) override;

    void mouseDrag(const MouseEvent &e) override;
    
    void mouseUp(const MouseEvent &e) override;
    
    void resized() override;

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
    
    void removeEventIfPossible(const AutomationEvent &e);
    
    Rectangle<int> getEventBounds(AutomationEventComponent *event) const;
    Rectangle<int> getEventBounds(float eventBeat, double controllerValue) const;

    void getRowsColsByMousePosition(int x, int y, float &targetValue, float &targetBeat) const;
    float getEventDiameter() const;
    float getHelperDiameter() const;
    int getAvailableHeight() const;
    
    AutomationEventComponent *getPreviousEventComponent(int indexOfSorted) const;
    AutomationEventComponent *getNextEventComponent(int indexOfSorted) const;
    
    friend class AutomationEventComponent;
    
private:
    
    void updateTempoComponent(AutomationEventComponent *);
    
    float projectFirstBeat;
    float projectLastBeat;
    
    float rollFirstBeat;
    float rollLastBeat;
    
    MidiRoll &roll;
    ProjectTreeItem &project;

    WeakReference<MidiLayer> layer;
    
    ScopedPointer<ComponentConnectorCurve> leadingConnector;

    OwnedArray<AutomationEventComponent> eventComponents;
    HashMap<AutomationEvent, AutomationEventComponent *, AutomationEventHashFunction> eventsHash;
    
    AutomationEventComponent *draggingEvent;
    bool addNewEventMode;
    
};

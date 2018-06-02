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

class HybridRoll;
class ProjectTreeItem;
class AutomationCurveHelper;
class AutomationCurveEventComponent;
class ComponentConnectorCurve;

class AutomationCurveSequenceMap :
    public Component,
    public ProjectListener
{
public:
    
    AutomationCurveSequenceMap(ProjectTreeItem &project, HybridRoll &roll, WeakReference<MidiSequence> sequence);
    ~AutomationCurveSequenceMap() override;
    
    void insertNewEventAt(const MouseEvent &e);
    
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
    
    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    // TODO! As a part of `automation editors` story
    void onAddClip(const Clip &clip) override {}
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override {}
    void onRemoveClip(const Clip &clip) override {}

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks) override;

protected:
    
    void removeEventIfPossible(const AutomationEvent &e);
    
    Rectangle<int> getEventBounds(AutomationCurveEventComponent *event) const;
    Rectangle<int> getEventBounds(float eventBeat, double controllerValue) const;

    void getRowsColsByMousePosition(int x, int y, float &targetValue, float &targetBeat) const;
    float getEventDiameter() const;
    float getHelperDiameter() const;
    int getAvailableHeight() const;
    
    AutomationCurveEventComponent *getPreviousEventComponent(int indexOfSorted) const;
    AutomationCurveEventComponent *getNextEventComponent(int indexOfSorted) const;
    
    friend class AutomationCurveEventComponent;
    
private:
    
    void updateTempoComponent(AutomationCurveEventComponent *);
    void reloadTrack();

    float projectFirstBeat;
    float projectLastBeat;
    
    float rollFirstBeat;
    float rollLastBeat;
    
    HybridRoll &roll;
    ProjectTreeItem &project;

    WeakReference<MidiSequence> sequence;
    
    ScopedPointer<ComponentConnectorCurve> leadingConnector;

    OwnedArray<AutomationCurveEventComponent> eventComponents;
    SparseHashMap<AutomationEvent, AutomationCurveEventComponent *, MidiEventHash> eventsHash;
    
    AutomationCurveEventComponent *draggingEvent;
    bool addNewEventMode;
    
};

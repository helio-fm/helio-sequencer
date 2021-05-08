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

#include "TimeSignatureEvent.h"
#include "ProjectListener.h"

class RollBase;
class ProjectNode;
class TrackStartIndicator;
class TrackEndIndicator;
class TimeSignatureComponent;

class TimeSignaturesProjectMap final :
    public Component,
    public ProjectListener
{
public:

    enum class Type : int8 { Large, Small };

    TimeSignaturesProjectMap(ProjectNode &parentProject, RollBase &parentRoll, Type type);
    ~TimeSignaturesProjectMap() override;

    void alignTimeSignatureComponent(TimeSignatureComponent *nc);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    // Assuming the timeline has no patterns/clips:
    void onAddClip(const Clip &clip) override {}
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override {}
    void onRemoveClip(const Clip &clip) override {}

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

    //===------------------------------------------------------------------===//
    // Stuff for children
    //===------------------------------------------------------------------===//

    void onTimeSignatureMoved(TimeSignatureComponent *nc);
    void onTimeSignatureTapped(TimeSignatureComponent *nc);
    void showContextMenuFor(TimeSignatureComponent *nc);
    void alternateActionFor(TimeSignatureComponent *nc);
    float getBeatByXPosition(int x) const;
    
private:
    
    void reloadTrackMap();
    void applyTimeSignatureBounds(TimeSignatureComponent *nc, TimeSignatureComponent *nextOne = nullptr);
    
    TimeSignatureComponent *getPreviousEventComponent(int indexOfSorted) const;
    TimeSignatureComponent *getNextEventComponent(int indexOfSorted) const;
    
    void updateTrackRangeIndicatorsAnchors();
    
private:
    
    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength; // not zero!

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;
    
    RollBase &roll;
    ProjectNode &project;
    
    UniquePointer<TrackStartIndicator> trackStartIndicator;
    UniquePointer<TrackEndIndicator> trackEndIndicator;
    
    ComponentAnimator animator;

    const Type type;
    TimeSignatureComponent *createComponent(const TimeSignatureEvent &event);

    OwnedArray<TimeSignatureComponent> timeSignatureComponents;
    FlatHashMap<TimeSignatureEvent, TimeSignatureComponent *, MidiEventHash> timeSignaturesHash;
    
};


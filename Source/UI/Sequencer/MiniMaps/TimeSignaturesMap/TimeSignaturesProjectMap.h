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

class RollBase;
class ProjectNode;
class TrackStartIndicator;
class TrackEndIndicator;
class TimeSignatureComponent;

#include "ProjectListener.h"
#include "TimeSignatureEvent.h"
#include "TimeSignaturesAggregator.h"

class TimeSignaturesProjectMap final : public Component,
    public TimeSignaturesAggregator::Listener,
    public ProjectListener
{
public:

    enum class Type : int8 { Large, Small };

    TimeSignaturesProjectMap(ProjectNode &parentProject, RollBase &parentRoll, Type type);
    ~TimeSignaturesProjectMap() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &, const MidiEvent &) override {}
    void onAddMidiEvent(const MidiEvent &) override {}
    void onRemoveMidiEvent(const MidiEvent &) override {}
    void onAddClip(const Clip &) override {}
    void onChangeClip(const Clip &, const Clip &) override {}
    void onRemoveClip(const Clip &) override {}
    void onAddTrack(MidiTrack *const) override {}
    void onRemoveTrack(MidiTrack *const) override {}
    void onChangeTrackProperties(MidiTrack *const) override {}
    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &, const ProjectMetadata *) override {}

    //===------------------------------------------------------------------===//
    // TimeSignaturesAggregator::Listener
    //===------------------------------------------------------------------===//

    void onTimeSignaturesUpdated() override;

    //===------------------------------------------------------------------===//
    // Stuff for children
    //===------------------------------------------------------------------===//

    void onTimeSignatureTapped(TimeSignatureComponent *nc);
    void showDialogFor(TimeSignatureComponent *nc);
    void alternateActionFor(TimeSignatureComponent *nc);

    float getBeatByXPosition(int x) const;
    void applyTimeSignatureBounds(TimeSignatureComponent *c,
        TimeSignatureComponent *nextOne = nullptr);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;

private:
    
    void reloadTrackMap();
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
    TimeSignatureComponent *createComponent();

    OwnedArray<TimeSignatureComponent> timeSignatureComponents;
    FlatHashMap<TimeSignatureEvent, TimeSignatureComponent *, MidiEventHash> timeSignaturesMap;
};

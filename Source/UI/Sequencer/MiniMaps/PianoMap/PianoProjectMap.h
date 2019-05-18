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

#include "Clip.h"
#include "Note.h"
#include "ProjectListener.h"
#include "ProjectMapNoteComponent.h"

class HybridRoll;
class ProjectNode;

class PianoProjectMap final :
    public Component,
    public ProjectListener,
    public AsyncUpdater
{
public:

    PianoProjectMap(ProjectNode &parentProject, HybridRoll &parentRoll);
    ~PianoProjectMap() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onAddMidiEvent(const MidiEvent &event) override;
    void onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks) override;

private:

    void applyNoteBounds(ProjectMapNoteComponent *nc);
    void reloadTrackMap();
    void loadTrack(const MidiTrack *const track);

    float projectFirstBeat = 0.f;
    float projectLastBeat = 0.f;

    float rollFirstBeat = 0.f;
    float rollLastBeat = 0.f;

    float componentHeight = 1.f;

    HybridRoll &roll;
    ProjectNode &project;

    using SequenceMap = FlatHashMap<Note, UniquePointer<ProjectMapNoteComponent>, MidiEventHash>;
    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceMap>, ClipHash>;
    PatternMap patternMap;

    void triggerBatchRepaintFor(ProjectMapNoteComponent *target);
    void handleAsyncUpdate() override;
    Array<WeakReference<Component>> batchRepaintList;

    JUCE_LEAK_DETECTOR(PianoProjectMap)
};

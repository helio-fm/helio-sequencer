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
#include "ComponentFader.h"

class RollBase;
class ProjectNode;
class VelocityMapNoteComponent;
class VelocityLevelDraggingHelper;
class FineTuningValueIndicator;

class VelocityProjectMap final :
    public Component,
    public ProjectListener,
    public AsyncUpdater, // triggers batch repaints for children
    public ChangeListener // subscribes on parent roll's lasso changes
{
public:

    VelocityProjectMap(ProjectNode &parentProject, RollBase &parentRoll);
    ~VelocityProjectMap() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &w) override;

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
    void onChangeViewEditableScope(MidiTrack *const, const Clip &clip, bool) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

private:

    void changeListenerCallback(ChangeBroadcaster *source) override;

    void applyNoteBounds(VelocityMapNoteComponent *nc);
    void reloadTrackMap();
    void loadTrack(const MidiTrack *const track);

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;

    RollBase &roll;
    ProjectNode &project;

    Clip activeClip;

    using SequenceMap = FlatHashMap<Note, UniquePointer<VelocityMapNoteComponent>, MidiEventHash>;
    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceMap>, ClipHash>;
    PatternMap patternMap;

    UniquePointer<VelocityLevelDraggingHelper> dragHelper;
    FlatHashMap<Note, float, MidiEventHash> dragIntersections;
    Array<Note> dragChangedNotes, dragChanges;
    bool dragHasChanges = false;

    Array<Note> sweepChangedNotes, sweepChanges;
    bool sweepHasChanges = false;

    float volumeBlendingAmount = 1.f;
    UniquePointer<FineTuningValueIndicator> volumeBlendingIndicator;
    void updateVolumeBlendingIndicator(const Point<int> &pos);
    ComponentFader fader;

    void applyVolumeChanges();
    void sweepEdit(const MouseEvent& e);

    void triggerBatchRepaintFor(VelocityMapNoteComponent *target);
    void handleAsyncUpdate() override;
    Array<WeakReference<Component>> batchRepaintList;

    JUCE_LEAK_DETECTOR(VelocityProjectMap)
};

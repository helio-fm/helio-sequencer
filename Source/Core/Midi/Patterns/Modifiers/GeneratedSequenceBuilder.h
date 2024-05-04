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

#pragma once

class ProjectNode;
class PianoSequence;

#include "ProjectListener.h"
#include "ProjectEventDispatcher.h"
#include "Clip.h"

// the purpose of this class is to listen to project changes and use
// the clip's modifiers stacks to rebuild the generated sequences asynchronously
// (it's convenient to keep the modifiers stack in clips,
// but it seems too cumbersome to do all this work in the Clip class)

class GeneratedSequenceBuilder final :
    public ProjectEventDispatcher, // swallows events from generated sequences
    public ProjectListener, // listens to changing events to trigger rebuilds
    public AsyncUpdater // coalesces multiple edits to re-generate sequences later
{
public:

    explicit GeneratedSequenceBuilder(ProjectNode &project);
    ~GeneratedSequenceBuilder();

    MidiSequence *getSequenceFor(const Clip &clip);

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onAddMidiEvent(const MidiEvent &event) override;
    void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;
    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;
    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeProjectInfo(const ProjectMetadata *info) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

    //===------------------------------------------------------------------===//
    // ProjectEventDispatcher
    //===------------------------------------------------------------------===//

    // this silences all change events from generated sequences:
    // those events are useless for the rolls, because they will fire while
    // rebuilding sequences from scratch using the clips' modifier stacks;
    // generated sequences have to be reloaded entirely by the rolls
    // after they are rebuilt on onReloadGeneratedSequence event,
    // and this class will make sure to dispatch such event when needed

    void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) noexcept override {}
    void dispatchAddEvent(const MidiEvent &event) noexcept override {}
    void dispatchRemoveEvent(const MidiEvent &event) noexcept override {}
    void dispatchPostRemoveEvent(MidiSequence *const layer) noexcept override {}

    void dispatchAddClip(const Clip &clip) noexcept override {}
    void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) noexcept override {}
    void dispatchRemoveClip(const Clip &clip) noexcept override {}
    void dispatchPostRemoveClip(Pattern *const pattern) noexcept override {}

    void dispatchChangeTrackProperties() noexcept override {}
    void dispatchChangeProjectBeatRange() noexcept override {}
    void dispatchChangeTrackBeatRange() noexcept override {}

    ProjectNode *getProject() const noexcept override
    {
        jassertfalse; // not expected to be called, doing something wrong?
        return &this->project;
    }

private:

    //===----------------------------------------------------------------------===//
    // Async updates
    //===----------------------------------------------------------------------===//

    void triggerAsyncUpdatesForTrack(WeakReference<MidiTrack> track);
    void triggerAsyncUpdateForClip(const Clip &clip);

    void handleAsyncUpdate() override;

    FlatHashSet<Clip, ClipHash> clipsToUpdate;

private:

    ProjectNode &project;

    FlatHashMap<Clip, UniquePointer<PianoSequence>, ClipHash> generatedSequences;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GeneratedSequenceBuilder)
};

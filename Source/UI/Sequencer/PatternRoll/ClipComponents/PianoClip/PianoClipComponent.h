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

#include "Note.h"
#include "ClipComponent.h"
#include "ProjectListener.h"

class HybridRoll;
class MidiSequence;
class ProjectNode;

class PianoClipComponent final : public ClipComponent, public ProjectListener
{
public:

    PianoClipComponent(ProjectNode &project, MidiSequence *sequence,
        HybridRoll &roll, const Clip &clip);

    ~PianoClipComponent() override;

    void setShowRecordingMode(bool isRecording);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    
    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    void onAddClip(const Clip &clip) override {}
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override {}

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override {}
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override {}
    void onReloadProjectContent(const Array<MidiTrack *> &tracks) override;
    void onChangeProjectInfo(const ProjectMetadata *info) override;

private:

    void reloadTrackMap();

    ProjectNode &project;
    WeakReference<MidiSequence> sequence;
    FlatHashSet<Note, MidiEventHash> displayedNotes;

    int keyboardSize = Globals::twelveToneKeyboardSize;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoClipComponent)
};

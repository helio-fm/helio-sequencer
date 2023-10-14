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

#include "Clip.h"
#include "Note.h"
#include "ProjectListener.h"
#include "ProjectMapsScroller.h"

class RollBase;
class ProjectNode;

class PianoProjectMap final :
    public ProjectMapsScroller::ScrolledComponent,
    public ProjectListener,
    public AsyncUpdater
{
public:

    explicit PianoProjectMap(ProjectNode &parentProject);
    ~PianoProjectMap() override;

    void setBrightness(float brighness);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void paint(Graphics &g) override;

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
    void onChangeViewEditableScope(MidiTrack *const track, const Clip &clip, bool) override;

    void onChangeProjectInfo(const ProjectMetadata *info) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

private:

    void reloadTrackMap();
    void loadTrack(const MidiTrack *const track);

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;

    float componentHeight = 1.f;

    float brightnessFactor = 1.f;

    int keyboardSize = Globals::twelveToneKeyboardSize;

    ProjectNode &project;

    Clip activeClip;
    Colour baseColour;

    using SequenceSet = FlatHashSet<Note, MidiEventHash>;
    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceSet>, ClipHash>;
    PatternMap patternMap;

    void handleAsyncUpdate() override;

    JUCE_LEAK_DETECTOR(PianoProjectMap)
};

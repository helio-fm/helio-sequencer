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

class MidiTrack;
class MidiEvent;
class MidiSequence;
class Pattern;
class Clip;
class ProjectMetadata;

class ProjectListener
{
public:

    ProjectListener() {}
    virtual ~ProjectListener() = default;

    virtual void onAddMidiEvent(const MidiEvent &event) {}
    virtual void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) {}
    virtual void onRemoveMidiEvent(const MidiEvent &event) {}
    virtual void onPostRemoveMidiEvent(MidiSequence *const layer) {}

    virtual void onAddClip(const Clip &clip) {}
    virtual void onChangeClip(const Clip &oldClip, const Clip &newClip) {}
    virtual void onRemoveClip(const Clip &clip) {}
    virtual void onPostRemoveClip(Pattern *const pattern) {}

    virtual void onAddTrack(MidiTrack *const track) {}
    virtual void onRemoveTrack(MidiTrack *const track) {}
    virtual void onChangeTrackProperties(MidiTrack *const track) {}
    virtual void onChangeTrackBeatRange(MidiTrack *const track) {}

    virtual void onChangeProjectInfo(const ProjectMetadata *info) {}

    // This will also be called when any track beat range changes (sounds weird, I know)
    // need to introduce onChangePatternBeatRange() and onChangeTrackBeatRange() instead;
    virtual void onChangeProjectBeatRange(float firstBeat, float lastBeat) {}

    // Any editor should restrict editing to a single clip of one track at a time.
    // I've removed the ability to edit multiple tracks at once, because, first,
    // it's unclear for the breadcrumbs what to display as a current track when multiple tracks are active,
    // and, second, some editing operations make no sense when having selected notes of different clips.
    // All the code that works with the selection, assumes that selected notes are unique,
    // and there are no multiple instances of the same note within different clips selected.
    virtual void onChangeViewEditableScope(MidiTrack *const track,
        const Clip &clip, bool shouldFocus) {}

    virtual void onChangeViewBeatRange(float firstBeat, float lastBeat) {}

    // Sent on midi import, reload or reset by VCS
    virtual void onBeforeReloadProjectContent() {};
    virtual void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) {}

    // Called when the project is switched to or opened, and vice versa
    virtual void onActivateProjectSubtree(const ProjectMetadata *info) {}
    virtual void onDeactivateProjectSubtree(const ProjectMetadata *info) {}

};

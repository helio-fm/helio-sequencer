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

class MidiTrack;
class MidiEvent;
class MidiSequence;
class Pattern;
class Clip;
class ProjectInfo;

class ProjectListener
{
public:

    ProjectListener() {}
    virtual ~ProjectListener() {}

    virtual void onAddMidiEvent(const MidiEvent &event) = 0;
    virtual void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) = 0;
    virtual void onRemoveMidiEvent(const MidiEvent &event) = 0;
    virtual void onPostRemoveMidiEvent(MidiSequence *const layer) {}

    virtual void onAddClip(const Clip &clip) = 0;
    virtual void onChangeClip(const Clip &oldClip, const Clip &newClip) = 0;
    virtual void onRemoveClip(const Clip &clip) = 0;
    virtual void onPostRemoveClip(Pattern *const pattern) {}

    virtual void onAddTrack(MidiTrack *const track) = 0;
    virtual void onRemoveTrack(MidiTrack *const track) = 0;
    virtual void onChangeTrackProperties(MidiTrack *const track) = 0;

    virtual void onChangeProjectInfo(const ProjectInfo *info) {}
    virtual void onChangeProjectBeatRange(float firstBeat, float lastBeat) = 0;
    virtual void onChangeViewBeatRange(float firstBeat, float lastBeat) = 0;

    // Sent on midi import, reload or reset by VCS
    virtual void onReloadProjectContent(const Array<MidiTrack *> &tracks) = 0;

};

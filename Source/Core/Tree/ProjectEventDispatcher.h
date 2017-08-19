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

class Clip;
class Pattern;
class MidiTrack;
class MidiEvent;
class MidiSequence;
class ProjectTreeItem;

#include "ProjectListener.h"

class ProjectEventDispatcher
{
public:

    virtual ~ProjectEventDispatcher() {}

    // Notes/events and sequences 
    virtual void dispatchAddEvent(const MidiEvent &event) = 0;
    virtual void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) = 0;
    virtual void dispatchRemoveEvent(const MidiEvent &event) = 0;
    virtual void dispatchPostRemoveEvent(MidiSequence *const sequence) = 0;

    // Patterns and clips
    virtual void dispatchAddClip(const Clip &clip) = 0;
    virtual void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) = 0;
    virtual void dispatchRemoveClip(const Clip &clip) = 0;
    virtual void dispatchPostRemoveClip(Pattern *const pattern) = 0;

    // Sent on lightweight changes like mute/unmute, instrument change
    virtual void dispatchChangeTrackProperties(MidiTrack *const track) = 0;
    // Needed for project to calculate and send the total beat range
    virtual void dispatchChangeTrackBeatRange(MidiTrack *const track) = 0;
    // Send on midi import, reload or reset by VCS
    virtual void dispatchChangeTrackContent(MidiTrack *const track) = 0;

    virtual ProjectTreeItem *getProject() const { return nullptr; }
};

class EmptyEventDispatcher : public ProjectEventDispatcher
{
public:

    void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override {}
    void dispatchAddEvent(const MidiEvent &event) override {}
    void dispatchRemoveEvent(const MidiEvent &event) override {}
    void dispatchPostRemoveEvent(MidiSequence *const layer) override {}

    void dispatchAddClip(const Clip &clip) override {}
    void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) override {}
    void dispatchRemoveClip(const Clip &clip) override {}
    void dispatchPostRemoveClip(Pattern *const pattern) override {}

    void dispatchChangeTrackProperties(MidiTrack *const track) override {}
    void dispatchChangeTrackBeatRange(MidiTrack *const track) override {}
    void dispatchChangeTrackContent(MidiTrack *const track) override {}
};

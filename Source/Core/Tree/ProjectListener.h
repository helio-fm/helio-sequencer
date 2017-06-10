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

class MidiEvent;
class MidiLayer;
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
	virtual void onPostRemoveMidiEvent(const MidiLayer *layer) {}

    virtual void onAddMidiLayer(const MidiLayer *layer) = 0;
	virtual void onChangeMidiLayer(const MidiLayer *layer) = 0;
	virtual void onRemoveMidiLayer(const MidiLayer *layer) = 0;
    
    virtual void onAddClip(const Clip &clip) {}
    virtual void onChangeClip(const Clip &oldClip, const Clip &newClip) {}
    virtual void onRemoveClip(const Clip &clip) {}
    virtual void onPostRemoveClip(const Pattern *pattern) {}

    virtual void onAddPattern(const Pattern *pattern) {}
    virtual void onChangePattern(const Pattern *pattern) {}
    virtual void onRemovePattern(const Pattern *pattern) {}

    virtual void onChangeProjectInfo(const ProjectInfo *info) {}
    virtual void onChangeProjectBeatRange(float firstBeat, float lastBeat) = 0;

};

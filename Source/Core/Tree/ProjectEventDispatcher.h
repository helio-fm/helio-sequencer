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
class Transport;
class ProjectTreeItem;
class Clip;
class Pattern;

#include "ProjectListener.h"

class ProjectEventDispatcher
{
public:

    virtual ~ProjectEventDispatcher() {}

	virtual void dispatchAddEvent(const MidiEvent &event) = 0;
	virtual void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) = 0;
	virtual void dispatchRemoveEvent(const MidiEvent &event) = 0;
	virtual void dispatchPostRemoveEvent(MidiLayer *const layer) = 0;

	// Sent on mute/unmute, instrument change, midi import, reload or reset
	virtual void dispatchReloadLayer(MidiLayer *const layer) = 0;
	virtual void dispatchChangeLayerBeatRange() = 0;

	virtual void dispatchAddClip(const Clip &clip) = 0;
	virtual void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) = 0;
	virtual void dispatchRemoveClip(const Clip &clip) = 0;
	virtual void dispatchPostRemoveClip(const Pattern *pattern) = 0;

	virtual void dispatchReloadPattern(Pattern *const pattern) = 0;
	virtual void dispatchChangePatternBeatRange() = 0;

    virtual ProjectTreeItem *getProject() const { return nullptr; }
};

class EmptyEventDispatcher : public ProjectEventDispatcher
{
public:

	void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override {}
	void dispatchAddEvent(const MidiEvent &event) override {}
	void dispatchRemoveEvent(const MidiEvent &event) override {}
	void dispatchPostRemoveEvent(MidiLayer *const layer) override {}

	void dispatchReloadLayer(MidiLayer *const layer) override {}
	void dispatchChangeLayerBeatRange() override {}

	void dispatchAddClip(const Clip &clip) override {}
	void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) override {}
	void dispatchRemoveClip(const Clip &clip) override {}
	void dispatchPostRemoveClip(const Pattern *pattern) override {}

	void dispatchReloadPattern(Pattern *const pattern) override {}
	void dispatchChangePatternBeatRange() override {}
};

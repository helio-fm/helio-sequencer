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

class MidiLayer;
class Pattern;

// A track is a meta-object that has
// all the properties and
// sequence with events and
// pattern with clips;
// MidiLayerTreeItem implements this

class MidiTrack
{
public:

	virtual String getTrackName() const noexcept = 0;
	virtual Colour getTrackColour() const noexcept = 0;
	virtual int getTrackChannel() const noexcept = 0;

	virtual String getTrackInstrumentId() const noexcept = 0;
	virtual int getTrackControllerNumber() const noexcept = 0;

	virtual bool isTrackMuted() const noexcept = 0;
	virtual bool isTrackSolo() const noexcept = 0;

	virtual MidiLayer *getLayer() const noexcept = 0;
	virtual Pattern *getPattern() const noexcept = 0;

	// used by player thread to link tracks with instruments
	// Uuid getLayerId() const noexcept;
	// String getLayerIdAsString() const;

	// Some shorthands:

	bool isTempoTrack() const noexcept
	{
		return (this->getTrackControllerNumber() == MidiLayer::tempoController);
	}

	bool isSustainPedalTrack() const noexcept
	{
		return (this->getTrackControllerNumber() == MidiLayer::sustainPedalController);
	}

	bool isOnOffTrack() const noexcept
	{
		// hardcoded for now
		return (this->getTrackControllerNumber() >= 64 &&
			this->getTrackControllerNumber() <= 69);
	}
};

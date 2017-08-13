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

class ProjectTreeItem;

#include "UndoAction.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

class MidiTrackRenameAction : public UndoAction
{
public:

    explicit MidiTrackRenameAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    MidiTrackRenameAction(ProjectTreeItem &project,
                              String layerId,
                              String newXPath);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    String layerId;
    
    String xPathBefore;
    String xPathAfter;

    JUCE_DECLARE_NON_COPYABLE(MidiTrackRenameAction)
};

//===----------------------------------------------------------------------===//
// Change Colour
//===----------------------------------------------------------------------===//

class MidiTrackChangeColourAction : public UndoAction
{
public:

	explicit MidiTrackChangeColourAction(ProjectTreeItem &project) :
		UndoAction(project) {}

	MidiTrackChangeColourAction(ProjectTreeItem &project,
		String layerId,
		const Colour &newColour);

	bool perform() override;
	bool undo() override;
	int getSizeInUnits() override;

	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

private:

	String layerId;

	Colour colourBefore;
	Colour colourAfter;

	JUCE_DECLARE_NON_COPYABLE(MidiTrackChangeColourAction)
};


//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

class MidiTrackChangeInstrumentAction : public UndoAction
{
public:

	explicit MidiTrackChangeInstrumentAction(ProjectTreeItem &project) :
		UndoAction(project) {}

	MidiTrackChangeInstrumentAction(ProjectTreeItem &project,
		String layerId,
		String newInstrumentId);

	bool perform() override;
	bool undo() override;
	int getSizeInUnits() override;

	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

private:

	String layerId;

	String instrumentIdBefore;
	String instrumentIdAfter;

	JUCE_DECLARE_NON_COPYABLE(MidiTrackChangeInstrumentAction)
};


//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

class MidiTrackMuteAction : public UndoAction
{
public:

	explicit MidiTrackMuteAction(ProjectTreeItem &project) :
		UndoAction(project) {}

	MidiTrackMuteAction(ProjectTreeItem &project,
		String layerId,
		bool shouldBeMuted);

	bool perform() override;
	bool undo() override;
	int getSizeInUnits() override;

	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

private:

	String layerId;

	bool muteStateBefore;
	bool muteStateAfter;

	JUCE_DECLARE_NON_COPYABLE(MidiTrackMuteAction)
};

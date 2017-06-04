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
#include "MidiLayer.h"

//===----------------------------------------------------------------------===//
// Change Colour
//===----------------------------------------------------------------------===//

class MidiLayerChangeColourAction : public UndoAction
{
public:
    
    explicit MidiLayerChangeColourAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    MidiLayerChangeColourAction(ProjectTreeItem &project,
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
    
    JUCE_DECLARE_NON_COPYABLE(MidiLayerChangeColourAction)
};


//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

class MidiLayerChangeInstrumentAction : public UndoAction
{
public:
    
    explicit MidiLayerChangeInstrumentAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    MidiLayerChangeInstrumentAction(ProjectTreeItem &project,
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
    
    JUCE_DECLARE_NON_COPYABLE(MidiLayerChangeInstrumentAction)
};


//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

class MidiLayerMuteAction : public UndoAction
{
public:
    
    explicit MidiLayerMuteAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    MidiLayerMuteAction(ProjectTreeItem &project,
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
    
    JUCE_DECLARE_NON_COPYABLE(MidiLayerMuteAction)
};


//===----------------------------------------------------------------------===//
// Insert Instance
//===----------------------------------------------------------------------===//

class MidiLayerInsertInstanceAction : public UndoAction
{
public:

	explicit MidiLayerInsertInstanceAction(ProjectTreeItem &project) :
		UndoAction(project) {}

	MidiLayerInsertInstanceAction(ProjectTreeItem &project,
		String layerId, const MidiLayer::Instance &target);

	bool perform() override;
	bool undo() override;
	int getSizeInUnits() override;

	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

private:

	String layerId;
	MidiLayer::Instance instance;

	JUCE_DECLARE_NON_COPYABLE(MidiLayerInsertInstanceAction)
};


//===----------------------------------------------------------------------===//
// Remove Instance
//===----------------------------------------------------------------------===//

class MidiLayerRemoveInstanceAction : public UndoAction
{
public:

	explicit MidiLayerRemoveInstanceAction(ProjectTreeItem &project) :
		UndoAction(project) {}

	MidiLayerRemoveInstanceAction(ProjectTreeItem &project,
		String layerId, const MidiLayer::Instance &target);

	bool perform() override;
	bool undo() override;
	int getSizeInUnits() override;

	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

private:

	String layerId;
	MidiLayer::Instance instance;

	JUCE_DECLARE_NON_COPYABLE(MidiLayerRemoveInstanceAction)
};


//===----------------------------------------------------------------------===//
// Change Instance
//===----------------------------------------------------------------------===//

class MidiLayerChangeInstanceAction : public UndoAction
{
public:

	explicit MidiLayerChangeInstanceAction(ProjectTreeItem &project) :
		UndoAction(project) {}

	MidiLayerChangeInstanceAction(ProjectTreeItem &project,
		String layerId,
		const MidiLayer::Instance &target,
		const MidiLayer::Instance &newParameters);

	bool perform() override;
	bool undo() override;
	int getSizeInUnits() override;
	UndoAction *createCoalescedAction(UndoAction *nextAction) override;

	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

private:

	String layerId;

	MidiLayer::Instance instanceBefore;
	MidiLayer::Instance instanceAfter;

	JUCE_DECLARE_NON_COPYABLE(MidiLayerChangeInstanceAction)

};

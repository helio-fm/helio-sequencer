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

#include "Common.h"
#include "MidiTrackActions.h"
#include "ProjectTreeItem.h"
#include "MidiTrackTreeItem.h"
#include "TreeItem.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

MidiTrackRenameAction::MidiTrackRenameAction(ProjectTreeItem &parentProject,
                                                     String targetLayerId,
                                                     String newXPath) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    xPathAfter(std::move(newXPath))
{
}

bool MidiTrackRenameAction::perform()
{
    if (MidiTrackTreeItem *treeItem =
        this->project.findChildByLayerId<MidiTrackTreeItem>(this->layerId))
    {
        this->xPathBefore = treeItem->getXPath();
        treeItem->onRename(this->xPathAfter);
        return true;
    }
    
    return false;
}

bool MidiTrackRenameAction::undo()
{
    if (MidiTrackTreeItem *treeItem =
        this->project.findChildByLayerId<MidiTrackTreeItem>(this->layerId))
    {
        treeItem->onRename(this->xPathBefore);
        return true;
    }
    
    return false;
}

int MidiTrackRenameAction::getSizeInUnits()
{
    return this->xPathBefore.length() + this->xPathAfter.length();
}

XmlElement *MidiTrackRenameAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::layerTreeItemRenameAction);
    xml->setAttribute(Serialization::Undo::xPathBefore, this->xPathBefore);
    xml->setAttribute(Serialization::Undo::xPathAfter, this->xPathAfter);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    return xml;
}

void MidiTrackRenameAction::deserialize(const XmlElement &xml)
{
    this->xPathBefore = xml.getStringAttribute(Serialization::Undo::xPathBefore);
    this->xPathAfter = xml.getStringAttribute(Serialization::Undo::xPathAfter);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiTrackRenameAction::reset()
{
    this->xPathBefore.clear();
    this->xPathAfter.clear();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Colour
//===----------------------------------------------------------------------===//

MidiTrackChangeColourAction::MidiTrackChangeColourAction(ProjectTreeItem &parentProject,
	String targetLayerId,
	const Colour &newColour) :
	UndoAction(parentProject),
	layerId(std::move(targetLayerId)),
	colourAfter(newColour)
{
}

bool MidiTrackChangeColourAction::perform()
{
	if (WeakReference<MidiSequence> layer =
		this->project.getLayerWithId<MidiSequence>(this->layerId))
	{
		this->colourBefore = layer->getColour();
		layer->setColour(this->colourAfter);
		layer->notifyLayerChanged();
		return true;
	}

	return false;
}

bool MidiTrackChangeColourAction::undo()
{
	if (WeakReference<MidiSequence> layer =
		this->project.getLayerWithId<MidiSequence>(this->layerId))
	{
		layer->setColour(this->colourBefore);
		layer->notifyLayerChanged();
		return true;
	}

	return false;
}

int MidiTrackChangeColourAction::getSizeInUnits()
{
	return sizeof(this->colourBefore) + sizeof(this->colourAfter);
}

XmlElement *MidiTrackChangeColourAction::serialize() const
{
	auto xml = new XmlElement(Serialization::Undo::midiLayerChangeColourAction);
	xml->setAttribute(Serialization::Undo::colourBefore, this->colourBefore.toString());
	xml->setAttribute(Serialization::Undo::colourAfter, this->colourAfter.toString());
	xml->setAttribute(Serialization::Undo::layerId, this->layerId);
	return xml;
}

void MidiTrackChangeColourAction::deserialize(const XmlElement &xml)
{
	this->colourBefore = Colour::fromString(xml.getStringAttribute(Serialization::Undo::colourBefore));
	this->colourAfter = Colour::fromString(xml.getStringAttribute(Serialization::Undo::colourAfter));
	this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiTrackChangeColourAction::reset()
{
	this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

MidiTrackChangeInstrumentAction::MidiTrackChangeInstrumentAction(ProjectTreeItem &parentProject,
	String targetLayerId,
	String newInstrumentId) :
	UndoAction(parentProject),
	layerId(std::move(targetLayerId)),
	instrumentIdAfter(std::move(newInstrumentId))
{
}

bool MidiTrackChangeInstrumentAction::perform()
{
	if (WeakReference<MidiSequence> layer =
		this->project.getLayerWithId<MidiSequence>(this->layerId))
	{
		this->instrumentIdBefore = layer->getInstrumentId();
		layer->setInstrumentId(this->instrumentIdAfter);
		return true;
	}

	return false;
}

bool MidiTrackChangeInstrumentAction::undo()
{
	if (WeakReference<MidiSequence> layer =
		this->project.getLayerWithId<MidiSequence>(this->layerId))
	{
		layer->setInstrumentId(this->instrumentIdBefore);
		return true;
	}

	return false;
}

int MidiTrackChangeInstrumentAction::getSizeInUnits()
{
	return this->instrumentIdAfter.length() + this->instrumentIdBefore.length();
}

XmlElement *MidiTrackChangeInstrumentAction::serialize() const
{
	auto xml = new XmlElement(Serialization::Undo::midiLayerChangeInstrumentAction);
	xml->setAttribute(Serialization::Undo::instrumentIdBefore, this->instrumentIdBefore);
	xml->setAttribute(Serialization::Undo::instrumentIdAfter, this->instrumentIdAfter);
	xml->setAttribute(Serialization::Undo::layerId, this->layerId);
	return xml;
}

void MidiTrackChangeInstrumentAction::deserialize(const XmlElement &xml)
{
	this->instrumentIdBefore = xml.getStringAttribute(Serialization::Undo::instrumentIdBefore);
	this->instrumentIdAfter = xml.getStringAttribute(Serialization::Undo::instrumentIdAfter);
	this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiTrackChangeInstrumentAction::reset()
{
	this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

MidiTrackMuteAction::MidiTrackMuteAction(ProjectTreeItem &parentProject,
	String targetLayerId,
	bool shouldBeMuted) :
	UndoAction(parentProject),
	layerId(std::move(targetLayerId)),
	muteStateAfter(shouldBeMuted)
{
}

bool MidiTrackMuteAction::perform()
{
	if (WeakReference<MidiSequence> layer =
		this->project.getLayerWithId<MidiSequence>(this->layerId))
	{
		this->muteStateBefore = layer->isMuted();
		layer->setMuted(this->muteStateAfter);
		return true;
	}

	return false;
}

bool MidiTrackMuteAction::undo()
{
	if (WeakReference<MidiSequence> layer =
		this->project.getLayerWithId<MidiSequence>(this->layerId))
	{
		layer->setMuted(this->muteStateBefore);
		return true;
	}

	return false;
}

int MidiTrackMuteAction::getSizeInUnits()
{
	return 1;
}

String boolToString(bool val)
{
	return val ? "yes" : "no";
}

bool stringToBool(const String &val)
{
	return val == "yes";
}

XmlElement *MidiTrackMuteAction::serialize() const
{
	auto xml = new XmlElement(Serialization::Undo::midiLayerMuteAction);
	xml->setAttribute(Serialization::Undo::muteStateBefore, boolToString(this->muteStateBefore));
	xml->setAttribute(Serialization::Undo::muteStateAfter, boolToString(this->muteStateAfter));
	xml->setAttribute(Serialization::Undo::layerId, this->layerId);
	return xml;
}

void MidiTrackMuteAction::deserialize(const XmlElement &xml)
{
	this->muteStateBefore = stringToBool(xml.getStringAttribute(Serialization::Undo::muteStateBefore));
	this->muteStateAfter = stringToBool(xml.getStringAttribute(Serialization::Undo::muteStateAfter));
	this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiTrackMuteAction::reset()
{
	this->layerId.clear();
}

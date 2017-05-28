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
#include "MidiLayerActions.h"
#include "ProjectTreeItem.h"
#include "MidiLayer.h"
#include "TreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Change Colour
//===----------------------------------------------------------------------===//

MidiLayerChangeColourAction::MidiLayerChangeColourAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const Colour &newColour) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    colourAfter(newColour)
{
}

bool MidiLayerChangeColourAction::perform()
{
    if (WeakReference<MidiLayer> layer =
        this->project.getLayerWithId<MidiLayer>(this->layerId))
    {
        this->colourBefore = layer->getColour();
        layer->setColour(this->colourAfter);
        layer->notifyLayerChanged();
        return true;
    }
    
    return false;
}

bool MidiLayerChangeColourAction::undo()
{
    if (WeakReference<MidiLayer> layer =
        this->project.getLayerWithId<MidiLayer>(this->layerId))
    {
        layer->setColour(this->colourBefore);
        layer->notifyLayerChanged();
        return true;
    }
    
    return false;
}

int MidiLayerChangeColourAction::getSizeInUnits()
{
    return sizeof(this->colourBefore) + sizeof(this->colourAfter);
}

XmlElement *MidiLayerChangeColourAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::midiLayerChangeColourAction);
    xml->setAttribute(Serialization::Undo::colourBefore, this->colourBefore.toString());
    xml->setAttribute(Serialization::Undo::colourAfter, this->colourAfter.toString());
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    return xml;
}

void MidiLayerChangeColourAction::deserialize(const XmlElement &xml)
{
    this->colourBefore = Colour::fromString(xml.getStringAttribute(Serialization::Undo::colourBefore));
    this->colourAfter = Colour::fromString(xml.getStringAttribute(Serialization::Undo::colourAfter));
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiLayerChangeColourAction::reset()
{
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

MidiLayerChangeInstrumentAction::MidiLayerChangeInstrumentAction(ProjectTreeItem &parentProject,
                                                                 String targetLayerId,
                                                                 String newInstrumentId) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    instrumentIdAfter(std::move(newInstrumentId))
{
}

bool MidiLayerChangeInstrumentAction::perform()
{
    if (WeakReference<MidiLayer> layer =
        this->project.getLayerWithId<MidiLayer>(this->layerId))
    {
        this->instrumentIdBefore = layer->getInstrumentId();
        layer->setInstrumentId(this->instrumentIdAfter);
        return true;
    }
    
    return false;
}

bool MidiLayerChangeInstrumentAction::undo()
{
    if (WeakReference<MidiLayer> layer =
        this->project.getLayerWithId<MidiLayer>(this->layerId))
    {
        layer->setInstrumentId(this->instrumentIdBefore);
        return true;
    }
    
    return false;
}

int MidiLayerChangeInstrumentAction::getSizeInUnits()
{
    return this->instrumentIdAfter.length() + this->instrumentIdBefore.length();
}

XmlElement *MidiLayerChangeInstrumentAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::midiLayerChangeInstrumentAction);
    xml->setAttribute(Serialization::Undo::instrumentIdBefore, this->instrumentIdBefore);
    xml->setAttribute(Serialization::Undo::instrumentIdAfter, this->instrumentIdAfter);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    return xml;
}

void MidiLayerChangeInstrumentAction::deserialize(const XmlElement &xml)
{
    this->instrumentIdBefore = xml.getStringAttribute(Serialization::Undo::instrumentIdBefore);
    this->instrumentIdAfter = xml.getStringAttribute(Serialization::Undo::instrumentIdAfter);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiLayerChangeInstrumentAction::reset()
{
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

MidiLayerMuteAction::MidiLayerMuteAction(ProjectTreeItem &parentProject,
                                         String targetLayerId,
                                         bool shouldBeMuted) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    muteStateAfter(shouldBeMuted)
{
}

bool MidiLayerMuteAction::perform()
{
    if (WeakReference<MidiLayer> layer =
        this->project.getLayerWithId<MidiLayer>(this->layerId))
    {
        this->muteStateBefore = layer->isMuted();
        layer->setMuted(this->muteStateAfter);
        return true;
    }
    
    return false;
}

bool MidiLayerMuteAction::undo()
{
    if (WeakReference<MidiLayer> layer =
        this->project.getLayerWithId<MidiLayer>(this->layerId))
    {
        layer->setMuted(this->muteStateBefore);
        return true;
    }
    
    return false;
}

int MidiLayerMuteAction::getSizeInUnits()
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

XmlElement *MidiLayerMuteAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::midiLayerMuteAction);
    xml->setAttribute(Serialization::Undo::muteStateBefore, boolToString(this->muteStateBefore));
    xml->setAttribute(Serialization::Undo::muteStateAfter, boolToString(this->muteStateAfter));
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    return xml;
}

void MidiLayerMuteAction::deserialize(const XmlElement &xml)
{
    this->muteStateBefore = stringToBool(xml.getStringAttribute(Serialization::Undo::muteStateBefore));
    this->muteStateAfter = stringToBool(xml.getStringAttribute(Serialization::Undo::muteStateAfter));
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiLayerMuteAction::reset()
{
    this->layerId.clear();
}



//===----------------------------------------------------------------------===//
// Insert Instance
//===----------------------------------------------------------------------===//


MidiLayerInsertInstanceAction::MidiLayerInsertInstanceAction(ProjectTreeItem &project,
	String layerId, const MidiLayer::Instance &target) :
	UndoAction(project),
	layerId(std::move(layerId)),
	instance(target)
{
}

bool MidiLayerInsertInstanceAction::perform()
{
	if (MidiLayer *layer = 
		this->project.getLayerWithId<MidiLayer>(this->layerId))
	{
		return layer->insertInstance(this->instance, false);
	}

	return false;
}

bool MidiLayerInsertInstanceAction::undo()
{
	if (MidiLayer *layer = 
		this->project.getLayerWithId<MidiLayer>(this->layerId))
	{
		return layer->removeInstance(this->instance, false);
	}

	return false;
}

int MidiLayerInsertInstanceAction::getSizeInUnits()
{
	return sizeof(MidiLayer::Instance);
}

XmlElement *MidiLayerInsertInstanceAction::serialize() const
{
	auto xml = new XmlElement(Serialization::Undo::midiLayerInstanceInsertAction);
	xml->setAttribute(Serialization::Undo::layerId, this->layerId);
	xml->prependChildElement(this->instance.serialize());
	return xml;
}

void MidiLayerInsertInstanceAction::deserialize(const XmlElement &xml)
{
	this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
	this->instance.deserialize(*xml.getFirstChildElement());
}

void MidiLayerInsertInstanceAction::reset()
{
	this->instance.reset();
	this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove Instance
//===----------------------------------------------------------------------===//

MidiLayerRemoveInstanceAction::MidiLayerRemoveInstanceAction(ProjectTreeItem &project,
	String layerId, const MidiLayer::Instance &target) :
	UndoAction(project),
	layerId(std::move(layerId)),
	instance(target)
{
}

bool MidiLayerRemoveInstanceAction::perform()
{
	if (MidiLayer *layer =
		this->project.getLayerWithId<MidiLayer>(this->layerId))
	{
		return layer->removeInstance(this->instance, false);
	}

	return false;
}

bool MidiLayerRemoveInstanceAction::undo()
{
	if (MidiLayer *layer =
		this->project.getLayerWithId<MidiLayer>(this->layerId))
	{
		return layer->insertInstance(this->instance, false);
	}

	return false;
}

int MidiLayerRemoveInstanceAction::getSizeInUnits()
{
	return sizeof(MidiLayer::Instance);
}

XmlElement *MidiLayerRemoveInstanceAction::serialize() const
{
	auto xml = new XmlElement(Serialization::Undo::midiLayerInstanceRemoveAction);
	xml->setAttribute(Serialization::Undo::layerId, this->layerId);
	xml->prependChildElement(this->instance.serialize());
	return xml;
}

void MidiLayerRemoveInstanceAction::deserialize(const XmlElement &xml)
{
	this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
	this->instance.deserialize(*xml.getFirstChildElement());
}

void MidiLayerRemoveInstanceAction::reset()
{
	this->instance.reset();
	this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Instance
//===----------------------------------------------------------------------===//

MidiLayerChangeInstanceAction::MidiLayerChangeInstanceAction(ProjectTreeItem &project,
	String layerId,
	const MidiLayer::Instance &target,
	const MidiLayer::Instance &newParameters) :
	UndoAction(project),
	layerId(std::move(layerId)),
	instanceBefore(target),
	instanceAfter(newParameters)
{
	jassert(target.getId() == newParameters.getId());
}

bool MidiLayerChangeInstanceAction::perform()
{
	if (MidiLayer *layer = 
		this->project.getLayerWithId<MidiLayer>(this->layerId))
	{
		return layer->changeInstance(this->instanceBefore, this->instanceAfter, false);
	}

	return false;
}

bool MidiLayerChangeInstanceAction::undo()
{
	if (MidiLayer *layer =
		this->project.getLayerWithId<MidiLayer>(this->layerId))
	{
		return layer->changeInstance(this->instanceAfter, this->instanceBefore, false);
	}

	return false;
}

int MidiLayerChangeInstanceAction::getSizeInUnits()
{
	return sizeof(MidiLayer::Instance) * 2;
}

UndoAction *MidiLayerChangeInstanceAction::createCoalescedAction(UndoAction *nextAction)
{
	if (MidiLayer *layer =
		this->project.getLayerWithId<MidiLayer>(this->layerId))
	{
		if (MidiLayerChangeInstanceAction *nextChanger = 
			dynamic_cast<MidiLayerChangeInstanceAction *>(nextAction))
		{
			// если прямо объединять события - это портит групповые изменения, так что смотрим по id
			const bool idsAreEqual =
				(this->instanceBefore.getId() == nextChanger->instanceAfter.getId() &&
				this->layerId == nextChanger->layerId);

			if (idsAreEqual)
			{
				return new MidiLayerChangeInstanceAction(this->project,
					this->layerId, this->instanceBefore, nextChanger->instanceAfter);
			}
		}
	}

	(void)nextAction;
	return nullptr;
}

XmlElement *MidiLayerChangeInstanceAction::serialize() const
{
	auto xml = new XmlElement(Serialization::Undo::midiLayerInstanceChangeAction);
	xml->setAttribute(Serialization::Undo::layerId, this->layerId);

	auto instanceBeforeChild = new XmlElement(Serialization::Undo::instanceBefore);
	instanceBeforeChild->prependChildElement(this->instanceBefore.serialize());
	xml->prependChildElement(instanceBeforeChild);

	auto instanceAfterChild = new XmlElement(Serialization::Undo::instanceAfter);
	instanceAfterChild->prependChildElement(this->instanceAfter.serialize());
	xml->prependChildElement(instanceAfterChild);

	return xml;
}

void MidiLayerChangeInstanceAction::deserialize(const XmlElement &xml)
{
	this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);

	auto instanceBeforeChild = xml.getChildByName(Serialization::Undo::instanceBefore);
	auto instanceAfterChild = xml.getChildByName(Serialization::Undo::instanceAfter);

	this->instanceBefore.deserialize(*instanceBeforeChild->getFirstChildElement());
	this->instanceAfter.deserialize(*instanceAfterChild->getFirstChildElement());
}

void MidiLayerChangeInstanceAction::reset()
{
	this->instanceBefore.reset();
	this->instanceAfter.reset();
	this->layerId.clear();
}

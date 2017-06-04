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

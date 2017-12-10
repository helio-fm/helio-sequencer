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
#include "MidiTrackSource.h"
#include "SerializationKeys.h"
#include "MidiTrack.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

MidiTrackRenameAction::MidiTrackRenameAction(MidiTrackSource &source,
    String targetTrackId, String newXPath) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    xPathAfter(std::move(newXPath)) {}

bool MidiTrackRenameAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->xPathBefore = track->getTrackName();
        track->setTrackName(this->xPathAfter);
        return true;
    }
    
    return false;
}

bool MidiTrackRenameAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackName(this->xPathBefore);
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
    auto xml = new XmlElement(Serialization::Undo::midiTrackRenameAction);
    xml->setAttribute(Serialization::Undo::xPathBefore, this->xPathBefore);
    xml->setAttribute(Serialization::Undo::xPathAfter, this->xPathAfter);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    return xml;
}

void MidiTrackRenameAction::deserialize(const XmlElement &xml)
{
    this->xPathBefore = xml.getStringAttribute(Serialization::Undo::xPathBefore);
    this->xPathAfter = xml.getStringAttribute(Serialization::Undo::xPathAfter);
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
}

void MidiTrackRenameAction::reset()
{
    this->xPathBefore.clear();
    this->xPathAfter.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Colour
//===----------------------------------------------------------------------===//

MidiTrackChangeColourAction::MidiTrackChangeColourAction(MidiTrackSource &source,
    String targetTrackId,
    const Colour &newColour) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    colourAfter(newColour)
{
}

bool MidiTrackChangeColourAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->colourBefore = track->getTrackColour();
        track->setTrackColour(this->colourAfter);
        return true;
    }

    return false;
}

bool MidiTrackChangeColourAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackColour(this->colourBefore);
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
    auto xml = new XmlElement(Serialization::Undo::midiTrackChangeColourAction);
    xml->setAttribute(Serialization::Undo::colourBefore, this->colourBefore.toString());
    xml->setAttribute(Serialization::Undo::colourAfter, this->colourAfter.toString());
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    return xml;
}

void MidiTrackChangeColourAction::deserialize(const XmlElement &xml)
{
    this->colourBefore = Colour::fromString(xml.getStringAttribute(Serialization::Undo::colourBefore));
    this->colourAfter = Colour::fromString(xml.getStringAttribute(Serialization::Undo::colourAfter));
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
}

void MidiTrackChangeColourAction::reset()
{
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

MidiTrackChangeInstrumentAction::MidiTrackChangeInstrumentAction(MidiTrackSource &source,
    String targetTrackId,
    String newInstrumentId) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    instrumentIdAfter(std::move(newInstrumentId)) {}

bool MidiTrackChangeInstrumentAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->instrumentIdBefore = track->getTrackInstrumentId();
        track->setTrackInstrumentId(this->instrumentIdAfter);
        return true;
    }

    return false;
}

bool MidiTrackChangeInstrumentAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackInstrumentId(this->instrumentIdBefore);
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
    auto xml = new XmlElement(Serialization::Undo::midiTrackChangeInstrumentAction);
    xml->setAttribute(Serialization::Undo::instrumentIdBefore, this->instrumentIdBefore);
    xml->setAttribute(Serialization::Undo::instrumentIdAfter, this->instrumentIdAfter);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    return xml;
}

void MidiTrackChangeInstrumentAction::deserialize(const XmlElement &xml)
{
    this->instrumentIdBefore = xml.getStringAttribute(Serialization::Undo::instrumentIdBefore);
    this->instrumentIdAfter = xml.getStringAttribute(Serialization::Undo::instrumentIdAfter);
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
}

void MidiTrackChangeInstrumentAction::reset()
{
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

MidiTrackMuteAction::MidiTrackMuteAction(MidiTrackSource &source,
    String targetTrackId,
    bool shouldBeMuted) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    muteStateAfter(shouldBeMuted) {}

bool MidiTrackMuteAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->muteStateBefore = track->isTrackMuted();
        track->setTrackMuted(this->muteStateAfter);
        return true;
    }

    return false;
}

bool MidiTrackMuteAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackMuted(this->muteStateBefore);
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
    auto xml = new XmlElement(Serialization::Undo::midiTrackMuteAction);
    xml->setAttribute(Serialization::Undo::muteStateBefore, boolToString(this->muteStateBefore));
    xml->setAttribute(Serialization::Undo::muteStateAfter, boolToString(this->muteStateAfter));
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    return xml;
}

void MidiTrackMuteAction::deserialize(const XmlElement &xml)
{
    this->muteStateBefore = stringToBool(xml.getStringAttribute(Serialization::Undo::muteStateBefore));
    this->muteStateAfter = stringToBool(xml.getStringAttribute(Serialization::Undo::muteStateAfter));
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
}

void MidiTrackMuteAction::reset()
{
    this->trackId.clear();
}

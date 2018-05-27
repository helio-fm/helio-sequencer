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
    const String &trackId, const String &xPath) noexcept :
    UndoAction(source),
    trackId(trackId),
    xPathAfter(xPath) {}

bool MidiTrackRenameAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->xPathBefore = track->getTrackName();
        track->setTrackName(this->xPathAfter, true);
        return true;
    }
    
    return false;
}

bool MidiTrackRenameAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackName(this->xPathBefore, true);
        return true;
    }
    
    return false;
}

int MidiTrackRenameAction::getSizeInUnits()
{
    return this->xPathBefore.length() + this->xPathAfter.length();
}

ValueTree MidiTrackRenameAction::serialize() const
{
    ValueTree tree(Serialization::Undo::midiTrackRenameAction);
    tree.setProperty(Serialization::Undo::xPathBefore, this->xPathBefore, nullptr);
    tree.setProperty(Serialization::Undo::xPathAfter, this->xPathAfter, nullptr);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    return tree;
}

void MidiTrackRenameAction::deserialize(const ValueTree &tree)
{
    this->xPathBefore = tree.getProperty(Serialization::Undo::xPathBefore);
    this->xPathAfter = tree.getProperty(Serialization::Undo::xPathAfter);
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
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
    const String &trackId, const Colour &newColour) noexcept :
    UndoAction(source),
    trackId(trackId),
    colourAfter(newColour) {}

bool MidiTrackChangeColourAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->colourBefore = track->getTrackColour();
        track->setTrackColour(this->colourAfter, true);
        return true;
    }

    return false;
}

bool MidiTrackChangeColourAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackColour(this->colourBefore, true);
        return true;
    }

    return false;
}

int MidiTrackChangeColourAction::getSizeInUnits()
{
    return sizeof(this->colourBefore) + sizeof(this->colourAfter);
}

ValueTree MidiTrackChangeColourAction::serialize() const
{
    ValueTree tree(Serialization::Undo::midiTrackChangeColourAction);
    tree.setProperty(Serialization::Undo::colourBefore, this->colourBefore.toString(), nullptr);
    tree.setProperty(Serialization::Undo::colourAfter, this->colourAfter.toString(), nullptr);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    return tree;
}

void MidiTrackChangeColourAction::deserialize(const ValueTree &tree)
{
    this->colourBefore = Colour::fromString(tree.getProperty(Serialization::Undo::colourBefore).toString());
    this->colourAfter = Colour::fromString(tree.getProperty(Serialization::Undo::colourAfter).toString());
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
}

void MidiTrackChangeColourAction::reset()
{
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Instrument
//===----------------------------------------------------------------------===//

MidiTrackChangeInstrumentAction::MidiTrackChangeInstrumentAction(MidiTrackSource &source,
    const String &trackId, const String &instrumentId) noexcept :
    UndoAction(source),
    trackId(trackId),
    instrumentIdAfter(instrumentId) {}

bool MidiTrackChangeInstrumentAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->instrumentIdBefore = track->getTrackInstrumentId();
        track->setTrackInstrumentId(this->instrumentIdAfter, true);
        return true;
    }

    return false;
}

bool MidiTrackChangeInstrumentAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackInstrumentId(this->instrumentIdBefore, true);
        return true;
    }

    return false;
}

int MidiTrackChangeInstrumentAction::getSizeInUnits()
{
    return this->instrumentIdAfter.length() + this->instrumentIdBefore.length();
}

ValueTree MidiTrackChangeInstrumentAction::serialize() const
{
    ValueTree tree(Serialization::Undo::midiTrackChangeInstrumentAction);
    tree.setProperty(Serialization::Undo::instrumentIdBefore, this->instrumentIdBefore, nullptr);
    tree.setProperty(Serialization::Undo::instrumentIdAfter, this->instrumentIdAfter, nullptr);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    return tree;
}

void MidiTrackChangeInstrumentAction::deserialize(const ValueTree &tree)
{
    this->instrumentIdBefore = tree.getProperty(Serialization::Undo::instrumentIdBefore);
    this->instrumentIdAfter = tree.getProperty(Serialization::Undo::instrumentIdAfter);
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
}

void MidiTrackChangeInstrumentAction::reset()
{
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Mute/Unmute
//===----------------------------------------------------------------------===//

MidiTrackMuteAction::MidiTrackMuteAction(MidiTrackSource &source,
    const String &trackId, bool shouldBeMuted) noexcept :
    UndoAction(source),
    trackId(trackId),
    muteStateAfter(shouldBeMuted) {}

bool MidiTrackMuteAction::perform()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->muteStateBefore = track->isTrackMuted();
        track->setTrackMuted(this->muteStateAfter, true);
        return true;
    }

    return false;
}

bool MidiTrackMuteAction::undo()
{
    if (MidiTrack *track =
        this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackMuted(this->muteStateBefore, true);
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

ValueTree MidiTrackMuteAction::serialize() const
{
    ValueTree tree(Serialization::Undo::midiTrackMuteAction);
    tree.setProperty(Serialization::Undo::muteStateBefore, boolToString(this->muteStateBefore), nullptr);
    tree.setProperty(Serialization::Undo::muteStateAfter, boolToString(this->muteStateAfter), nullptr);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    return tree;
}

void MidiTrackMuteAction::deserialize(const ValueTree &tree)
{
    this->muteStateBefore = stringToBool(tree.getProperty(Serialization::Undo::muteStateBefore));
    this->muteStateAfter = stringToBool(tree.getProperty(Serialization::Undo::muteStateAfter));
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
}

void MidiTrackMuteAction::reset()
{
    this->trackId.clear();
}

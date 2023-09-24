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
#include "TimeSignatureEvent.h"
#include "SerializationKeys.h"
#include "MidiTrack.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

MidiTrackRenameAction::MidiTrackRenameAction(MidiTrackSource &source,
    const String &trackId, const String &path) noexcept :
    UndoAction(source),
    trackId(trackId),
    pathAfter(path) {}

bool MidiTrackRenameAction::perform()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->pathBefore = track->getTrackName();
        track->setTrackName(this->pathAfter, false, sendNotification);
        return true;
    }

    return false;
}

bool MidiTrackRenameAction::undo()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackName(this->pathBefore, false, sendNotification);
        return true;
    }

    return false;
}

int MidiTrackRenameAction::getSizeInUnits()
{
    return this->pathBefore.length() + this->pathAfter.length();
}

SerializedData MidiTrackRenameAction::serialize() const
{
    SerializedData tree(Serialization::Undo::midiTrackRenameAction);
    tree.setProperty(Serialization::Undo::treePathBefore, this->pathBefore);
    tree.setProperty(Serialization::Undo::treePathAfter, this->pathAfter);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    return tree;
}

void MidiTrackRenameAction::deserialize(const SerializedData &data)
{
    this->pathBefore = data.getProperty(Serialization::Undo::treePathBefore);
    this->pathAfter = data.getProperty(Serialization::Undo::treePathAfter);
    this->trackId = data.getProperty(Serialization::Undo::trackId);
}

void MidiTrackRenameAction::reset()
{
    this->pathBefore.clear();
    this->pathAfter.clear();
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
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->colourBefore = track->getTrackColour();
        track->setTrackColour(this->colourAfter, false, sendNotification);
        return true;
    }

    return false;
}

bool MidiTrackChangeColourAction::undo()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackColour(this->colourBefore, false, sendNotification);
        return true;
    }

    return false;
}

int MidiTrackChangeColourAction::getSizeInUnits()
{
    return sizeof(this->colourBefore) + sizeof(this->colourAfter);
}

SerializedData MidiTrackChangeColourAction::serialize() const
{
    SerializedData tree(Serialization::Undo::midiTrackChangeColourAction);
    tree.setProperty(Serialization::Undo::colourBefore, this->colourBefore.toString());
    tree.setProperty(Serialization::Undo::colourAfter, this->colourAfter.toString());
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    return tree;
}

void MidiTrackChangeColourAction::deserialize(const SerializedData &data)
{
    this->colourBefore = Colour::fromString(data.getProperty(Serialization::Undo::colourBefore).toString());
    this->colourAfter = Colour::fromString(data.getProperty(Serialization::Undo::colourAfter).toString());
    this->trackId = data.getProperty(Serialization::Undo::trackId);
}

void MidiTrackChangeColourAction::reset()
{
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Channel
//===----------------------------------------------------------------------===//

MidiTrackChangeChannelAction::MidiTrackChangeChannelAction(MidiTrackSource &source,
    const String &trackId, const int newChannel) noexcept :
    UndoAction(source),
    trackId(trackId),
    channelAfter(newChannel) {}

bool MidiTrackChangeChannelAction::perform()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->channelBefore = track->getTrackChannel();
        track->setTrackChannel(this->channelAfter, false, sendNotification);
        return true;
    }

    return false;
}

bool MidiTrackChangeChannelAction::undo()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackChannel(this->channelBefore, false, sendNotification);
        return true;
    }

    return false;
}

int MidiTrackChangeChannelAction::getSizeInUnits()
{
    return sizeof(this->channelBefore) + sizeof(this->channelAfter);
}

SerializedData MidiTrackChangeChannelAction::serialize() const
{
    SerializedData tree(Serialization::Undo::midiTrackChangeChannelAction);
    tree.setProperty(Serialization::Undo::channelBefore, this->channelBefore);
    tree.setProperty(Serialization::Undo::channelAfter, this->channelAfter);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    return tree;
}

void MidiTrackChangeChannelAction::deserialize(const SerializedData &data)
{
    this->channelBefore = jlimit(1, 16, int(data.getProperty(Serialization::Undo::channelBefore, 1)));
    this->channelAfter = jlimit(1, 16, int(data.getProperty(Serialization::Undo::channelAfter, 1)));
    this->trackId = data.getProperty(Serialization::Undo::trackId);
}

void MidiTrackChangeChannelAction::reset()
{
    this->channelBefore = this->channelAfter = 1;
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
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->instrumentIdBefore = track->getTrackInstrumentId();
        track->setTrackInstrumentId(this->instrumentIdAfter, false, sendNotification);
        return true;
    }

    return false;
}

bool MidiTrackChangeInstrumentAction::undo()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTrackInstrumentId(this->instrumentIdBefore, false, sendNotification);
        return true;
    }

    return false;
}

int MidiTrackChangeInstrumentAction::getSizeInUnits()
{
    return this->instrumentIdAfter.length() + this->instrumentIdBefore.length();
}

SerializedData MidiTrackChangeInstrumentAction::serialize() const
{
    SerializedData tree(Serialization::Undo::midiTrackChangeInstrumentAction);
    tree.setProperty(Serialization::Undo::instrumentIdBefore, this->instrumentIdBefore);
    tree.setProperty(Serialization::Undo::instrumentIdAfter, this->instrumentIdAfter);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    return tree;
}

void MidiTrackChangeInstrumentAction::deserialize(const SerializedData &data)
{
    this->instrumentIdBefore = data.getProperty(Serialization::Undo::instrumentIdBefore);
    this->instrumentIdAfter = data.getProperty(Serialization::Undo::instrumentIdAfter);
    this->trackId = data.getProperty(Serialization::Undo::trackId);
}

void MidiTrackChangeInstrumentAction::reset()
{
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Time Signature
//===----------------------------------------------------------------------===//

MidiTrackChangeTimeSignatureAction::MidiTrackChangeTimeSignatureAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    timeSignatureAfter(newParameters) {}

MidiTrackChangeTimeSignatureAction::MidiTrackChangeTimeSignatureAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &oldParameters,
    const TimeSignatureEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    timeSignatureBefore(oldParameters),
    timeSignatureAfter(newParameters) {}

bool MidiTrackChangeTimeSignatureAction::perform()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        this->timeSignatureBefore = *track->getTimeSignatureOverride();
        track->setTimeSignatureOverride(this->timeSignatureAfter, false, sendNotification);
        return true;
    }

    return false;
}

bool MidiTrackChangeTimeSignatureAction::undo()
{
    if (auto *track = this->source.findTrackById<MidiTrack>(this->trackId))
    {
        track->setTimeSignatureOverride(this->timeSignatureBefore, false, sendNotification);
        return true;
    }

    return false;
}

int MidiTrackChangeTimeSignatureAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent) * 2;
}

UndoAction *MidiTrackChangeTimeSignatureAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextChanger = dynamic_cast<MidiTrackChangeTimeSignatureAction *>(nextAction))
    {
        if (this->trackId == nextChanger->trackId)
        {
            return new MidiTrackChangeTimeSignatureAction(this->source,
                this->trackId, this->timeSignatureBefore, nextChanger->timeSignatureAfter);
        }
    }

    (void)nextAction;
    return nullptr;
}

SerializedData MidiTrackChangeTimeSignatureAction::serialize() const
{
    SerializedData data(Serialization::Undo::midiTrackChangeTimeSignatureAction);
    data.appendChild(this->timeSignatureBefore.serialize());
    data.appendChild(this->timeSignatureAfter.serialize());
    data.setProperty(Serialization::Undo::trackId, this->trackId);
    return data;
}

void MidiTrackChangeTimeSignatureAction::deserialize(const SerializedData &data)
{
    jassert(data.getNumChildren() == 2);
    if (data.getNumChildren() == 2)
    {
        this->timeSignatureBefore.deserialize(data.getChild(0));
        this->timeSignatureAfter.deserialize(data.getChild(1)); // order matters
    }

    this->trackId = data.getProperty(Serialization::Undo::trackId);
}

void MidiTrackChangeTimeSignatureAction::reset()
{
    this->trackId.clear();
}

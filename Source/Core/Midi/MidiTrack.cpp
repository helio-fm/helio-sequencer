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

#include "Common.h"
#include "MidiTrack.h"
#include "SerializationKeys.h"

int MidiTrack::compareElements(const MidiTrack &first, const MidiTrack &second)
{
    if (&first == &second) { return 0; }
    return first.getTrackName().compareNatural(second.getTrackName());
}

int MidiTrack::compareElements(const MidiTrack *first, const MidiTrack *second)
{
    if (first == second) { return 0; }
    return first->getTrackName().compareNatural(second->getTrackName());
}

void MidiTrack::serializeTrackProperties(XmlElement &xml) const
{
    xml.setAttribute(Serialization::Core::trackId,
        this->getTrackId().toString());
    xml.setAttribute(Serialization::Core::trackColour,
        this->getTrackColour().toString());
    xml.setAttribute(Serialization::Core::trackMuteState,
        this->getTrackMuteStateAsString());
    xml.setAttribute(Serialization::Core::trackChannel,
        this->getTrackChannel());
    xml.setAttribute(Serialization::Core::trackInstrumentId,
        this->getTrackInstrumentId());
    xml.setAttribute(Serialization::Core::trackControllerNumber,
        this->getTrackControllerNumber());
}

void MidiTrack::deserializeTrackProperties(const XmlElement &xml)
{
    auto trackId =
        Uuid(xml.getStringAttribute(Serialization::Core::trackId,
            this->getTrackId().toString()));

    auto colour =
        Colour::fromString(xml.getStringAttribute(Serialization::Core::trackColour,
            this->getTrackColour().toString()));

    auto muted =
        MidiTrack::isTrackMuted(
            xml.getStringAttribute(Serialization::Core::trackMuteState));

    auto channel =
        xml.getIntAttribute(Serialization::Core::trackChannel,
            this->getTrackChannel());

    auto instrumentId =
        xml.getStringAttribute(Serialization::Core::trackInstrumentId,
            this->getTrackInstrumentId());

    auto controllerNumber =
        xml.getIntAttribute(Serialization::Core::trackControllerNumber,
            this->getTrackControllerNumber());

    this->setTrackId(trackId);
    this->setTrackColour(colour);
    this->setTrackInstrumentId(instrumentId);
    this->setTrackControllerNumber(controllerNumber);
    this->setTrackInstrumentId(instrumentId);
    this->setTrackMuted(muted);
}

bool MidiTrack::isTempoTrack() const noexcept
{
    return (this->getTrackControllerNumber() == MidiTrack::tempoController);
}

bool MidiTrack::isSustainPedalTrack() const noexcept
{
    return (this->getTrackControllerNumber() == MidiTrack::sustainPedalController);
}

bool MidiTrack::isOnOffTrack() const noexcept
{
    // hardcoded for now
    return (this->getTrackControllerNumber() >= 64 &&
        this->getTrackControllerNumber() <= 69);
}

String MidiTrack::getTrackMuteStateAsString() const
{
    return (this->isTrackMuted() ? "yes" : "no");
}

bool MidiTrack::isTrackMuted(const String &muteState)
{
    return (muteState == "yes");
}

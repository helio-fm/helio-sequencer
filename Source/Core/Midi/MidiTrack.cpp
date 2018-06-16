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
#include "MidiTrack.h"
#include "SerializationKeys.h"

int MidiTrack::compareElements(const MidiTrack &first, const MidiTrack &second)
{
    if (&first == &second) { return 0; }
    return first.getTrackName().compareNatural(second.getTrackName());
}

int MidiTrack::compareElements(const MidiTrack *const first, const MidiTrack *const second)
{
    if (first == second) { return 0; }
    return first->getTrackName().compareNatural(second->getTrackName());
}

void MidiTrack::serializeTrackProperties(ValueTree &tree) const
{
    using namespace Serialization;
    tree.setProperty(Core::trackId, this->getTrackId(), nullptr);
    tree.setProperty(Core::trackColour, this->getTrackColour().toString(), nullptr);
    tree.setProperty(Core::trackMuteState, this->getTrackMuteStateAsString(), nullptr);
    tree.setProperty(Core::trackChannel, this->getTrackChannel(), nullptr);
    tree.setProperty(Core::trackInstrumentId, this->getTrackInstrumentId(), nullptr);
    tree.setProperty(Core::trackControllerNumber, this->getTrackControllerNumber(), nullptr);
}

void MidiTrack::deserializeTrackProperties(const ValueTree &tree)
{
    using namespace Serialization;

    const auto trackId = tree.getProperty(Core::trackId, this->getTrackId());
    const auto colour = tree.getProperty(Core::trackColour, this->getTrackColour().toString()).toString();
    const auto muted = MidiTrack::isTrackMuted(tree.getProperty(Core::trackMuteState));
    const auto channel = tree.getProperty(Core::trackChannel, this->getTrackChannel());
    const auto instrumentId = tree.getProperty(Core::trackInstrumentId, this->getTrackInstrumentId());
    const auto controllerNumber = tree.getProperty(Core::trackControllerNumber, this->getTrackControllerNumber());

    this->setTrackId(trackId);

    // Do not send notifications:
    // track is not supposed to update listeners meanwhile loading,
    // its up to caller to make sure the views are updated after
    this->setTrackColour(Colour::fromString(colour), false);
    this->setTrackInstrumentId(instrumentId, false);
    this->setTrackControllerNumber(controllerNumber, false);
    this->setTrackInstrumentId(instrumentId, false);
    this->setTrackMuted(muted, false);
}

bool MidiTrack::isTempoTrack() const noexcept
{
    return (this->getTrackControllerNumber() == MidiTrack::tempoController);
}

bool MidiTrack::isOnOffAutomationTrack() const noexcept
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

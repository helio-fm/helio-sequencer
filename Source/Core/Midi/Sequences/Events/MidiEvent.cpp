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
#include "MidiEvent.h"
#include "MidiSequence.h"
#include "MidiTrack.h"

MidiEvent::MidiEvent(const MidiEvent &other) noexcept :
    sequence(other.sequence),
    type(other.type),
    beat(other.beat),
    id(other.id) {}

MidiEvent::MidiEvent(WeakReference<MidiSequence> owner, const MidiEvent &parameters) noexcept :
    sequence(owner),
    type(parameters.type),
    beat(parameters.beat),
    id(parameters.id) {}

MidiEvent::MidiEvent(WeakReference<MidiSequence> owner, Type type, float beatVal) noexcept :
    sequence(owner),
    type(type),
    beat(roundBeat(beatVal))
{
    this->id = this->createId();
}

bool MidiEvent::isValid() const noexcept
{
    return this->sequence != nullptr && this->id.isNotEmpty();
}

MidiSequence *MidiEvent::getSequence() const noexcept
{
    jassert(this->sequence);
    return this->sequence;
}

int MidiEvent::getTrackControllerNumber() const noexcept
{
    jassert(this->sequence);
    return this->sequence->getTrack()->getTrackControllerNumber();
}

int MidiEvent::getTrackChannel() const noexcept
{
    jassert(this->sequence);
    return this->sequence->getTrack()->getTrackChannel();
}

Colour MidiEvent::getTrackColour() const noexcept
{
    jassert(this->sequence);
    return this->sequence->getTrack()->getTrackColour();
}

MidiEvent::Id MidiEvent::getId() const noexcept
{
    return this->id;
}

float MidiEvent::getBeat() const noexcept
{
    return this->beat;
}

MidiEvent::Id MidiEvent::createId() const noexcept
{
    if (this->sequence != nullptr)
    {
        return this->sequence->createUniqueEventId();
    }

    return {};
}

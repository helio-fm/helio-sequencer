/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "Note.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"
#include "KeyboardMapping.h"

Note::Note() noexcept : MidiEvent(nullptr, Type::Note, 0.f) {}

Note::Note(WeakReference<MidiSequence> owner,
    int keyVal, float beatVal,
    float lengthVal, float velocityVal) noexcept :
    MidiEvent(owner, Type::Note, beatVal),
    key(keyVal),
    length(lengthVal),
    velocity(velocityVal) {}

Note::Note(WeakReference<MidiSequence> owner, const Note &parametersToCopy) noexcept :
    MidiEvent(owner, parametersToCopy),
    key(parametersToCopy.key),
    length(parametersToCopy.length),
    velocity(parametersToCopy.velocity),
    tuplet(parametersToCopy.tuplet) {}

void Note::exportMessages(MidiMessageSequence &outSequence, const Clip &clip,
    const KeyboardMapping &keyMap, double timeFactor) const noexcept
{
    const auto keyWithOffset = this->key + clip.getKey();
    const auto finalVolume = this->velocity * clip.getVelocity();
    const auto tupletLength = this->length / float(this->tuplet);
    const auto mapped = keyMap.map(keyWithOffset, this->sequence->getChannel());

    for (int i = 0; i < this->tuplet; ++i)
    {
        const float tupletStart = this->beat + tupletLength * float(i);

        // slightly adjust volume for tuplet sequence: factor fading from 1 to 0.9;
        // this should sound anyway better than the same volume for all tuplets,
        // but, in future user should have some kind of control over it
        // (like implement auto curves for individual notes?)
        const float tupletVolume = finalVolume * (1.f - float(i) / 100.f);

        MidiMessage eventNoteOn(MidiMessage::noteOn(mapped.channel, mapped.key, tupletVolume));
        const double startTime = (tupletStart + clip.getBeat()) * timeFactor;
        eventNoteOn.setTimeStamp(startTime);
        outSequence.addEvent(eventNoteOn);

        // we want to subtract some little time offset from the the note-off
        // timestamps to make sure end/start times of neighbor notes never overlap:
        // one case is the odd tuplet, where note-off event time might end up
        // being slightly after next event's start time, due to rounding errors,
        // e.g. 17.333333969116211 -> 18.666667938232422
        //                            18.666666030883789 -> 20.000000000000000;
        // another case is playing consecutive notes one right after another
        // sometimes (rarely) results in second note silenced immediately;
        // this is the easiest fix, although because of it note-off will not
        // be aligned accurately, so someday we might come up with a better fix:
        constexpr auto noteOffOffset = 1.0 / 1000.0;

        MidiMessage eventNoteOff(MidiMessage::noteOff(mapped.channel, mapped.key));
        const double endTime = (tupletStart + tupletLength + clip.getBeat()) * timeFactor - noteOffOffset;
        eventNoteOff.setTimeStamp(endTime);
        outSequence.addEvent(eventNoteOff);
    }
}

Note Note::withNewId(WeakReference<MidiSequence> owner) const noexcept
{
    Note n(*this);
    if (owner != nullptr)
    {
        n.sequence = owner;
    }
    
    n.id = n.createId();
    return n;
}

Note Note::withKey(Key newKey) const noexcept
{
    Note other(*this);
    other.key = jmax(0, newKey);
    return other;
}

Note Note::withBeat(float newBeat) const noexcept
{
    Note other(*this);
    other.beat = roundBeat(newBeat);
    return other;
}

Note Note::withKeyBeat(Key newKey, float newBeat) const noexcept
{
    Note other(*this);
    other.key = jmax(0, newKey);
    other.beat = roundBeat(newBeat);
    return other;
}

Note Note::withKeyLength(Key newKey, float newLength) const noexcept
{
    Note other(*this);
    other.key = jmax(0, newKey);
    other.length = jmax(Globals::minNoteLength, roundBeat(newLength));
    return other;
}

Note Note::withDeltaBeat(float deltaPosition) const noexcept
{
    Note other(*this);
    other.beat = roundBeat(other.beat + deltaPosition);
    return other;
}

Note Note::withDeltaKey(Key deltaKey) const noexcept
{
    Note other(*this);
    other.key = jmax(0, other.key + deltaKey);
    return other;
}

Note Note::withLength(float newLength) const noexcept
{
    Note other(*this);
    other.length = jmax(Globals::minNoteLength, roundBeat(newLength));
    return other;
}

Note Note::withDeltaLength(float deltaLength) const noexcept
{
    Note other(*this);
    other.length = jmax(Globals::minNoteLength, roundBeat(other.length + deltaLength));
    return other;
}

Note Note::withVelocity(float newVelocity) const noexcept
{
    Note other(*this);
    other.velocity = jlimit(0.f, 1.f, newVelocity);
    return other;
}

Note Note::withDeltaVelocity(float deltaVelocity) const noexcept
{
    Note other(*this);
    other.velocity = jlimit(0.f, 1.f, this->velocity + deltaVelocity);
    return other;
}

Note Note::withTuplet(Tuplet tuplet) const noexcept
{
    Note other(*this);
    // what would be the sane upper limit? like 9
    other.tuplet = jlimit(Tuplet(1), Tuplet(9), tuplet);
    return other;
}

Note Note::withParameters(const SerializedData &parameters) const noexcept
{
    Note n(*this);
    n.deserialize(parameters);
    return n;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

Note::Key Note::getKey() const noexcept
{
    return this->key;
}

float Note::getLength() const noexcept
{
    return this->length;
}

float Note::getVelocity() const noexcept
{
    return this->velocity;
}

Note::Tuplet Note::getTuplet() const noexcept
{
    return this->tuplet;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Note::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Midi::note);
    tree.setProperty(Midi::id, packId(this->id));
    tree.setProperty(Midi::key, this->key);
    tree.setProperty(Midi::timestamp, int(this->beat * Globals::ticksPerBeat));
    tree.setProperty(Midi::length, int(this->length * Globals::ticksPerBeat));
    tree.setProperty(Midi::volume, int(this->velocity * Globals::velocitySaveResolution));
    if (this->tuplet > 1)
    {
        tree.setProperty(Midi::tuplet, this->tuplet);
    }
    return tree;
}

void Note::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;
    this->id = unpackId(data.getProperty(Midi::id));
    this->key = data.getProperty(Midi::key);
    this->beat = float(data.getProperty(Midi::timestamp)) / Globals::ticksPerBeat;
    this->length = float(data.getProperty(Midi::length)) / Globals::ticksPerBeat;
    const auto vol = float(data.getProperty(Midi::volume)) / Globals::velocitySaveResolution;
    this->velocity = jmax(jmin(vol, 1.f), 0.f);
    this->tuplet = Tuplet(int(data.getProperty(Midi::tuplet, 1)));
}

void Note::reset() noexcept {}

void Note::applyChanges(const Note &other) noexcept
{
    jassert(this->id == other.id);
    this->beat = roundBeat(other.beat);
    this->key = other.key;
    this->length = other.length;
    this->velocity = other.velocity;
    this->tuplet = other.tuplet;
}

int Note::compareElements(const Note *const first, const Note *const second) noexcept
{
    if (first == second) { return 0; }

    const float beatDiff = first->beat - second->beat;
    const int beatResult = (beatDiff > 0.f) - (beatDiff < 0.f);
    if (beatResult != 0) { return beatResult; }

    const int keyDiff = first->key - second->key;
    const int keyResult = (keyDiff > 0) - (keyDiff < 0);
    if (keyResult != 0) { return keyResult; }

    return first->getId() - second->getId();
}

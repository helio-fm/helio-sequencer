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
#include "Note.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"

Note::Note() : MidiEvent(nullptr, MidiEvent::Note, 0.f)
{
    // needed for juce's Array to work
    //jassertfalse;
}

Note::Note(WeakReference<MidiSequence> owner,
    int keyVal, float beatVal,
    float lengthVal, float velocityVal) :
    MidiEvent(owner, MidiEvent::Note, beatVal),
    key(keyVal),
    length(lengthVal),
    velocity(velocityVal) {}

Note::Note(const Note &other) :
    MidiEvent(other.sequence, MidiEvent::Note, other.beat),
    key(other.key),
    length(other.length),
    velocity(other.velocity)
{
    this->id = other.getId();
}

Note::Note(WeakReference<MidiSequence> owner, const Note &parametersToCopy) :
    MidiEvent(owner, MidiEvent::Note, parametersToCopy.beat),
    key(parametersToCopy.key),
    length(parametersToCopy.length),
    velocity(parametersToCopy.velocity)
{
    // This constructor assume you know what you're doing,
    // and the id is unique within a target sequence
    this->id = parametersToCopy.getId();
}

Array<MidiMessage> Note::toMidiMessages() const
{
    Array<MidiMessage> result;

    MidiMessage eventNoteOn(MidiMessage::noteOn(this->getChannel(), this->key, velocity));
    const double startTime = round(double(this->beat) * MS_PER_BEAT);
    eventNoteOn.setTimeStamp(startTime);

    MidiMessage eventNoteOff(MidiMessage::noteOff(this->getChannel(), this->key));
    const double endTime = round(double(this->beat + this->length) * MS_PER_BEAT);
    eventNoteOff.setTimeStamp(endTime);

    result.add(eventNoteOn);
    result.add(eventNoteOff);

    return result;
}

Note Note::copyWithNewId(WeakReference<MidiSequence> owner) const
{
    Note n(*this);
    if (owner != nullptr)
    {
        n.sequence = owner;
    }
    
    n.id = n.createId();
    return n;
}

Note Note::withBeat(float newBeat) const
{
    Note other(*this);
    other.beat = roundBeat(newBeat);
    return other;
}

Note Note::withKeyBeat(int newKey, float newBeat) const
{
    Note other(*this);
    other.key = jmin(jmax(newKey, 0), 128);
    other.beat = roundBeat(newBeat);
    return other;
}

Note Note::withDeltaBeat(float deltaPosition) const
{
    Note other(*this);
    other.beat = roundBeat(other.beat + deltaPosition);
    return other;
}

Note Note::withDeltaKey(int deltaKey) const
{
    Note other(*this);
    other.key = jmin(jmax(other.key + deltaKey, 0), 128);
    return other;
}

#define MIN_LENGTH 0.5f

Note Note::withLength(float newLength) const
{
    Note other(*this);
    other.length = jmax(MIN_LENGTH, roundBeat(newLength));
    return other;
}

Note Note::withDeltaLength(float deltaLength) const
{
    Note other(*this);
    other.length = jmax(MIN_LENGTH, roundBeat(other.length + deltaLength));
    return other;
}

Note Note::withVelocity(float newVelocity) const
{
    Note other(*this);
    other.velocity = jmin(jmax(newVelocity, 0.0f), 1.0f);
    return other;
}

Note Note::withParameters(const ValueTree &parameters) const
{
    Note n(*this);
    n.deserialize(parameters);
    return n;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

int Note::getKey() const noexcept
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

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

#define VELOCITY_SAVE_ACCURACY 128.f

ValueTree Note::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Midi::note);
    tree.setProperty(Midi::id, this->id);
    tree.setProperty(Midi::key, this->key);
    tree.setProperty(Midi::beat, this->beat);
    tree.setProperty(Midi::length, this->length);
    tree.setProperty(Midi::velocity, roundFloatToInt(this->velocity * VELOCITY_SAVE_ACCURACY));
    return tree;
}

void Note::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;
    this->id = tree.getProperty(Midi::id);
    this->key = tree.getProperty(Midi::key);;
    this->beat = roundBeat(tree.getProperty(Midi::beat));
    this->length = roundBeat(tree.getProperty(Midi::length));
    const float vel = float(tree.getProperty(Midi::velocity)) / VELOCITY_SAVE_ACCURACY;
    this->velocity = jmax(jmin(vel, 1.f), 0.f);
}

void Note::reset() {}

void Note::applyChanges(const Note &other)
{
    jassert(this->id == other.id);
    this->beat = roundBeat(other.beat);
    this->key = other.key;
    this->length = other.length;
    this->velocity = other.velocity;
}

int Note::compareElements(const MidiEvent *const first, const MidiEvent *const second)
{
    if (first == second) { return 0; }

    const float diff = first->getBeat() - second->getBeat();
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    if (diffResult != 0) { return diffResult; }

    return first->getId().compare(second->getId());
}

int Note::compareElements(const Note *const first, const Note *const second)
{
    if (first == second) { return 0; }

    const float beatDiff = first->getBeat() - second->getBeat();
    const int beatResult = (beatDiff > 0.f) - (beatDiff < 0.f);
    if (beatResult != 0) { return beatResult; }

    const int keyDiff = first->getKey() - second->getKey();
    const int keyResult = (keyDiff > 0) - (keyDiff < 0);
    if (keyResult != 0) { return keyResult; }

    return first->getId().compare(second->getId());
}

int Note::compareElements(const Note &first, const Note &second)
{
    return Note::compareElements(&first, &second);
}

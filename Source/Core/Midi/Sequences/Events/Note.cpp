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
#include "Transport.h"
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
    const float &startTime = this->beat * Transport::millisecondsPerBeat;
    eventNoteOn.setTimeStamp(startTime);

    MidiMessage eventNoteOff(MidiMessage::noteOff(this->getChannel(), this->key));
    const float &endTime = (this->beat + this->length) * Transport::millisecondsPerBeat;
    eventNoteOff.setTimeStamp(endTime);

    result.add(eventNoteOn);
    result.add(eventNoteOff);

    return result;
}

Note Note::copyWithNewId(WeakReference<MidiSequence> owner) const
{
    Note n(*this);
    n.id = this->createId();
    
    if (owner != nullptr)
    {
        n.sequence = owner;
    }
    
    return n;
}

static float roundBeat(float beat)
{
    return roundf(beat * 16.f) / 16.f;
    //return roundf(beat * 1000.f) / 1000.f;
}

Note Note::withBeat(float newBeat) const
{
    Note other(*this);
    //other.beat = newBeat;
    other.beat = roundBeat(newBeat);
    return other;
}

Note Note::withKeyBeat(int newKey, float newBeat) const
{
    Note other(*this);
    other.key = jmin(jmax(newKey, 0), 128);
    //other.beat = newBeat;
    other.beat = roundBeat(newBeat);
    return other;
}

Note Note::withDeltaBeat(float deltaPosition) const
{
    Note other(*this);
    //other.beat = other.beat + deltaPosition;
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
    //other.length = jmax(MIN_LENGTH, newLength);
    other.length = jmax(MIN_LENGTH, roundBeat(newLength));
    return other;
}

Note Note::withDeltaLength(float deltaLength) const
{
    Note other(*this);
    //other.length = jmax(MIN_LENGTH, other.length + deltaLength);
    other.length = jmax(MIN_LENGTH, roundBeat(other.length + deltaLength));
    return other;
}

Note Note::withVelocity(float newVelocity) const
{
    Note other(*this);
    other.velocity = jmin(jmax(newVelocity, 0.0f), 1.0f);
    return other;
}

Note Note::withParameters(const XmlElement &xml) const
{
    Note n(*this);
    n.deserialize(xml);
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

// 128 would be enough, but
#define VELOCITY_SAVE_ACCURACY 1024.f

XmlElement *Note::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::note);
    xml->setAttribute("key", this->key);
    xml->setAttribute("beat", this->beat);
    xml->setAttribute("len", this->length);
    xml->setAttribute("vel", roundFloatToInt(this->velocity * VELOCITY_SAVE_ACCURACY));
    xml->setAttribute("id", this->id);
    return xml;
}

void Note::deserialize(const XmlElement &xml)
{
    this->reset();

    const int xmlKey = xml.getIntAttribute("key");
    const float xmlBeat = float(xml.getDoubleAttribute("beat"));
    const float xmlLength = float(xml.getDoubleAttribute("len"));
    const float xmlVelocity = float(xml.getIntAttribute("vel")) / VELOCITY_SAVE_ACCURACY;
    const String& xmlId = xml.getStringAttribute("id");

    this->key = xmlKey;
    this->beat = xmlBeat;
    this->length = xmlLength;
    this->velocity = jmax(jmin(xmlVelocity, 1.f), 0.f);
    this->id = xmlId;
}

void Note::reset() {}

void Note::applyChanges(const Note &other)
{
    jassert(this->id == other.id);
    //this->id = other.id;
    this->beat = other.beat;
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

int Note::compareElements(Note *const first, Note *const second)
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
    if (&first == &second) { return 0; }

    const float beatDiff = first.getBeat() - second.getBeat();
    const int beatResult = (beatDiff > 0.f) - (beatDiff < 0.f);
    if (beatResult != 0) { return beatResult; }

    const int keyDiff = first.getKey() - second.getKey();
    const int keyResult = (keyDiff > 0) - (keyDiff < 0);
    if (keyResult != 0) { return keyResult; }

    return first.getId().compare(second.getId());
}

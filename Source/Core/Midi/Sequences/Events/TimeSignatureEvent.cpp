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
#include "TimeSignatureEvent.h"
#include "MidiTrack.h"
#include "MidiSequence.h"
#include "Meter.h"
#include "SerializationKeys.h"

TimeSignatureEvent::TimeSignatureEvent() noexcept :
    MidiEvent(nullptr, Type::TimeSignature, 0.f) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiTrack> owner) noexcept :
    MidiEvent(nullptr, Type::TimeSignature, 0.f),
    track(owner) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    float newBeat, int newNumerator, int newDenominator) noexcept :
    MidiEvent(owner, Type::TimeSignature, newBeat),
    track(nullptr),
    numerator(newNumerator),
    denominator(newDenominator) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    const TimeSignatureEvent &parametersToCopy) noexcept :
    MidiEvent(owner, parametersToCopy),
    track(parametersToCopy.track),
    numerator(parametersToCopy.numerator),
    denominator(parametersToCopy.denominator) {}

void TimeSignatureEvent::exportMessages(MidiMessageSequence &outSequence,
    const Clip &clip, const KeyboardMapping &keyMap, double timeOffset, double timeFactor) const noexcept
{
    MidiMessage event(MidiMessage::timeSignatureMetaEvent(this->numerator, this->denominator));
    event.setTimeStamp((this->beat + clip.getBeat()) * timeFactor);
    outSequence.addEvent(event, timeOffset);
}

TimeSignatureEvent TimeSignatureEvent::withDeltaBeat(float beatOffset) const noexcept
{
    TimeSignatureEvent e(*this);
    e.beat = e.beat + beatOffset;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withBeat(float newBeat) const noexcept
{
    TimeSignatureEvent e(*this);
    e.beat = newBeat;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withNumerator(const int newNumerator) const noexcept
{
    TimeSignatureEvent e(*this);
    e.numerator = jlimit(Meter::minNumerator, Meter::maxNumerator, newNumerator);
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withDenominator(const int newDenominator) const noexcept
{
    TimeSignatureEvent e(*this);
    e.denominator = jlimit(Meter::minDenominator, Meter::maxDenominator, newDenominator);
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withParameters(const SerializedData &parameters) const noexcept
{
    TimeSignatureEvent e(*this);
    e.deserialize(parameters);
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withNewId() const noexcept
{
    TimeSignatureEvent e(*this);
    e.id = e.createId();
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withId(MidiEvent::Id id) const noexcept
{
    TimeSignatureEvent e(*this);
    e.id = id;
    return e;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

int TimeSignatureEvent::getNumerator() const noexcept
{
    return this->numerator;
}

int TimeSignatureEvent::getDenominator() const noexcept
{
    return this->denominator;
}

bool TimeSignatureEvent::isValid() const noexcept
{
    return (this->track != nullptr || this->sequence != nullptr) &&
           (this->numerator >= Meter::minNumerator &&
               this->numerator <= Meter::maxNumerator &&
               this->denominator >= Meter::minDenominator &&
               this->denominator <= Meter::maxDenominator);
}

float TimeSignatureEvent::getBarLengthInBeats() const noexcept
{
    jassert(this->isValid());
    return float(this->numerator) / float(this->denominator) * float(Globals::beatsPerBar);
}

int TimeSignatureEvent::getTrackControllerNumber() const noexcept
{
    if (this->track != nullptr)
    {
        return this->track->getTrackControllerNumber();
    }

    return MidiEvent::getTrackControllerNumber();
}

int TimeSignatureEvent::getTrackChannel() const noexcept
{
    if (this->track != nullptr)
    {
        return this->track->getTrackChannel();
    }

    return MidiEvent::getTrackChannel();
}

Colour TimeSignatureEvent::getTrackColour() const noexcept
{
    if (this->track != nullptr)
    {
        return this->track->getTrackColour();
    }

    return MidiEvent::getTrackColour();
}

WeakReference<MidiTrack> TimeSignatureEvent::getTrack() const noexcept
{
    return this->track;
}

String TimeSignatureEvent::toString() const noexcept
{
    return String(this->numerator) + "/" + String(this->denominator);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData TimeSignatureEvent::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Midi::timeSignature);
    tree.setProperty(Midi::id, packId(this->id));
    tree.setProperty(Midi::numerator, this->numerator);
    tree.setProperty(Midi::denominator, this->denominator);
    tree.setProperty(Midi::timestamp, int(this->beat * Globals::ticksPerBeat));
    return tree;
}

void TimeSignatureEvent::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;
    this->numerator = data.getProperty(Midi::numerator, Globals::Defaults::timeSignatureNumerator);
    this->denominator = data.getProperty(Midi::denominator, Globals::Defaults::timeSignatureDenominator);
    this->beat = float(data.getProperty(Midi::timestamp)) / Globals::ticksPerBeat;
    this->id = unpackId(data.getProperty(Midi::id));
}

// resets to invalid state
void TimeSignatureEvent::reset() noexcept
{
    this->beat = 0.f;
    this->numerator = 0;
    this->denominator = 0;
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

void TimeSignatureEvent::applyChanges(const TimeSignatureEvent &parameters) noexcept
{
    this->beat = parameters.beat;
    this->numerator = parameters.numerator;
    this->denominator = parameters.denominator;
}

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
#include "TimeSignatureEvent.h"
#include "MidiTrack.h"
#include "MidiSequence.h"
#include "Meter.h"
#include "SerializationKeys.h"
#include "Config.h"
#include "MetersCollection.h"

TimeSignatureEvent::TimeSignatureEvent() noexcept :
    MidiEvent(nullptr, Type::TimeSignature, 0.f) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiTrack> owner) noexcept :
    MidiEvent(nullptr, Type::TimeSignature, 0.f),
    track(owner) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    float newBeat, int numerator, int denominator) noexcept :
    MidiEvent(owner, Type::TimeSignature, newBeat),
    track(nullptr),
    meter({}, {}, numerator, denominator) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    const TimeSignatureEvent &parametersToCopy) noexcept :
    MidiEvent(owner, parametersToCopy),
    track(parametersToCopy.track),
    meter(parametersToCopy.meter) {}

void TimeSignatureEvent::exportMessages(MidiMessageSequence &outSequence,
    const Clip &clip, const KeyboardMapping &keyMap, double timeFactor) const noexcept
{
    MidiMessage event(MidiMessage::timeSignatureMetaEvent(this->meter.getNumerator(), this->meter.getDenominator()));
    event.setTimeStamp((this->beat + clip.getBeat()) * timeFactor);
    outSequence.addEvent(event);
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

TimeSignatureEvent TimeSignatureEvent::withNumerator(const int numerator) const noexcept
{
    TimeSignatureEvent e(*this);
    e.meter = e.meter.withNumerator(numerator);
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withDenominator(const int denominator) const noexcept
{
    TimeSignatureEvent e(*this);
    e.meter = e.meter.withDenominator(denominator);
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withMeter(const Meter &meter) const noexcept
{
    TimeSignatureEvent e(*this);
    e.meter = meter;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withMetronome(const MetronomeScheme &scheme) const noexcept
{
    TimeSignatureEvent e(*this);
    e.meter = e.meter.withMetronome(scheme);
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
    return this->meter.getNumerator();
}

int TimeSignatureEvent::getDenominator() const noexcept
{
    return this->meter.getDenominator();
}

bool TimeSignatureEvent::isValid() const noexcept
{
    return (this->track != nullptr || this->sequence != nullptr) && this->meter.isValid();
}

float TimeSignatureEvent::getBarLengthInBeats() const noexcept
{
    return this->meter.getBarLengthInBeats();
}

float TimeSignatureEvent::getDenominatorInBeats() const noexcept
{
    return this->meter.getDenominatorInBeats();
}

const Meter &TimeSignatureEvent::getMeter() const noexcept
{
    return this->meter;
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
    return this->meter.getTimeAsString();
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData TimeSignatureEvent::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Midi::timeSignature);

    tree.setProperty(Midi::id, packId(this->id));
    tree.setProperty(Midi::timestamp, int(this->beat * Globals::ticksPerBeat));

    // note: we're not serializing this->meter instead
    // of adding all these 3 fields to support older versions:
    tree.setProperty(Midi::numerator, this->meter.getNumerator());
    tree.setProperty(Midi::denominator, this->meter.getDenominator());
    tree.setProperty(Midi::metronomeScheme, this->meter.getMetronome().toString());

    return tree;
}

void TimeSignatureEvent::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;

    this->id = unpackId(data.getProperty(Midi::id));
    this->beat = float(data.getProperty(Midi::timestamp)) / Globals::ticksPerBeat;

    const int numerator = data.getProperty(Midi::numerator, Globals::Defaults::timeSignatureNumerator);
    const int denominator = data.getProperty(Midi::denominator, Globals::Defaults::timeSignatureDenominator);
    const String metronomeString = data.getProperty(Midi::metronomeScheme);
    this->meter = Meter({}, metronomeString, numerator, denominator);

    if (metronomeString.isEmpty())
    {
        // in the projects created with older versions of this app,
        // time signatures and their meters will miss metronome schemes,
        // so if this is the case, try to find the default scheme;
        // but first, assign a fallback value, which simply matches
        // the meter size, in case we don't find the default scheme:
        this->meter = this->meter.withMetronome(MetronomeScheme().resized(numerator));
        for (const auto it : App::Config().getMeters()->getAll())
        {
            // denominator check is kinda redundant here:
            if (it->getNumerator() == numerator)
            {
                this->meter = this->meter.withMetronome(it->getMetronome());
                break;
            }
        }
    }
}

// resets to invalid state
void TimeSignatureEvent::reset() noexcept
{
    this->beat = 0.f;
    this->meter = {};
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

void TimeSignatureEvent::applyChanges(const TimeSignatureEvent &parameters) noexcept
{
    this->beat = parameters.beat;
    this->meter = parameters.meter;
}

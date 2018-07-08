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
#include "MidiSequence.h"
#include "SerializationKeys.h"

TimeSignatureEvent::TimeSignatureEvent() noexcept : MidiEvent(nullptr, MidiEvent::TimeSignature, 0.f)
{
    //jassertfalse;
}

TimeSignatureEvent::TimeSignatureEvent(const TimeSignatureEvent &other) noexcept :
    MidiEvent(other),
    numerator(other.numerator),
    denominator(other.denominator) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    float newBeat, int newNumerator, int newDenominator) noexcept :
    MidiEvent(owner, MidiEvent::TimeSignature, newBeat),
    numerator(newNumerator),
    denominator(newDenominator) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    const TimeSignatureEvent &parametersToCopy) noexcept :
    MidiEvent(owner, parametersToCopy),
    numerator(parametersToCopy.numerator),
    denominator(parametersToCopy.denominator) {}

void TimeSignatureEvent::parseString(const String &data, int &numerator, int &denominator)
{
    numerator = TIME_SIGNATURE_DEFAULT_NUMERATOR;
    denominator = TIME_SIGNATURE_DEFAULT_DENOMINATOR;

    StringArray sa;
    sa.addTokens(data, "/\\|-", "' \"");

    if (sa.size() == 2)
    {
        const int n = sa[0].getIntValue();
        int d = sa[1].getIntValue();
        // Round to the power of two:
        d = int(pow(2, ceil(log(d) / log(2))));
        // Apply some reasonable constraints:
        denominator = jlimit(2, 32, d);
        numerator = jlimit(2, 64, n);
    }
}


void TimeSignatureEvent::exportMessages(MidiMessageSequence &outSequence, const Clip &clip, double timeOffset, double timeFactor) const
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
    e.numerator = newNumerator;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withDenominator(const int newDenominator) const noexcept
{
    TimeSignatureEvent e(*this);
    e.denominator = newDenominator;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withParameters(const ValueTree &parameters) const noexcept
{
    TimeSignatureEvent e(*this);
    e.deserialize(parameters);
    return e;
}

TimeSignatureEvent TimeSignatureEvent::copyWithNewId() const noexcept
{
    TimeSignatureEvent e(*this);
    e.id = e.createId();
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

String TimeSignatureEvent::toString() const noexcept
{
    return String(this->numerator) + "/" + String(this->denominator);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree TimeSignatureEvent::serialize() const noexcept
{
    using namespace Serialization;
    ValueTree tree(Midi::timeSignature);
    tree.setProperty(Midi::id, this->id, nullptr);
    tree.setProperty(Midi::numerator, this->numerator, nullptr);
    tree.setProperty(Midi::denominator, this->denominator, nullptr);
    tree.setProperty(Midi::timestamp, int(this->beat * TICKS_PER_BEAT), nullptr);
    return tree;
}

void TimeSignatureEvent::deserialize(const ValueTree &tree) noexcept
{
    this->reset();
    using namespace Serialization;
    this->numerator = tree.getProperty(Midi::numerator, TIME_SIGNATURE_DEFAULT_NUMERATOR);
    this->denominator = tree.getProperty(Midi::denominator, TIME_SIGNATURE_DEFAULT_DENOMINATOR);
    this->beat = float(tree.getProperty(Midi::timestamp)) / TICKS_PER_BEAT;
    this->id = tree.getProperty(Midi::id);
}

void TimeSignatureEvent::reset() noexcept {}

void TimeSignatureEvent::applyChanges(const TimeSignatureEvent &parameters) noexcept
{
    jassert(this->id == parameters.id);
    this->beat = parameters.beat;
    this->numerator = parameters.numerator;
    this->denominator = parameters.denominator;
}

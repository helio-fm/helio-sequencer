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

TimeSignatureEvent::TimeSignatureEvent() : MidiEvent(nullptr, MidiEvent::TimeSignature, 0.f)
{
    //jassertfalse;
}

TimeSignatureEvent::TimeSignatureEvent(const TimeSignatureEvent &other) :
    MidiEvent(other),
    numerator(other.numerator),
    denominator(other.denominator) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    float newBeat, int newNumerator, int newDenominator) :
    MidiEvent(owner, MidiEvent::TimeSignature, newBeat),
    numerator(newNumerator),
    denominator(newDenominator) {}

TimeSignatureEvent::TimeSignatureEvent(WeakReference<MidiSequence> owner,
    const TimeSignatureEvent &parametersToCopy) :
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


Array<MidiMessage> TimeSignatureEvent::toMidiMessages() const
{
    Array<MidiMessage> result;
    MidiMessage event(MidiMessage::timeSignatureMetaEvent(this->numerator, this->denominator));
    event.setTimeStamp(round(this->beat * MS_PER_BEAT));
    result.add(event);
    return result;
}

TimeSignatureEvent TimeSignatureEvent::withDeltaBeat(float beatOffset) const
{
    TimeSignatureEvent e(*this);
    e.beat = e.beat + beatOffset;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withBeat(float newBeat) const
{
    TimeSignatureEvent e(*this);
    e.beat = newBeat;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withNumerator(const int newNumerator) const
{
    TimeSignatureEvent e(*this);
    e.numerator = newNumerator;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withDenominator(const int newDenominator) const
{
    TimeSignatureEvent e(*this);
    e.denominator = newDenominator;
    return e;
}

TimeSignatureEvent TimeSignatureEvent::withParameters(const XmlElement &xml) const
{
    TimeSignatureEvent e(*this);
    e.deserialize(xml);
    return e;
}

TimeSignatureEvent TimeSignatureEvent::copyWithNewId() const
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

XmlElement *TimeSignatureEvent::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::timeSignature);
    xml->setAttribute("numerator", this->numerator);
    xml->setAttribute("denominator", this->denominator);
    xml->setAttribute("beat", this->beat);
    xml->setAttribute("id", this->id);
    return xml;
}

void TimeSignatureEvent::deserialize(const XmlElement &xml)
{
    this->reset();
    this->numerator = xml.getIntAttribute("numerator", TIME_SIGNATURE_DEFAULT_NUMERATOR);
    this->denominator = xml.getIntAttribute("denominator", TIME_SIGNATURE_DEFAULT_DENOMINATOR);
    this->beat = float(xml.getDoubleAttribute("beat"));
    this->id = xml.getStringAttribute("id");
}

void TimeSignatureEvent::reset() {}


void TimeSignatureEvent::applyChanges(const TimeSignatureEvent &parameters)
{
    jassert(this->id == parameters.id);
    this->beat = parameters.beat;
    this->numerator = parameters.numerator;
    this->denominator = parameters.denominator;
}

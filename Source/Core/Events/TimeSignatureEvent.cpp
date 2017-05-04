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
#include "MidiLayer.h"
#include "Transport.h"
#include "SerializationKeys.h"


TimeSignatureEvent::TimeSignatureEvent() : MidiEvent(nullptr, 0.f)
{
    //jassertfalse;
}

TimeSignatureEvent::TimeSignatureEvent(const TimeSignatureEvent &other) :
    MidiEvent(other.layer, other.beat),
    numerator(other.numerator),
    denominator(other.denominator)
{
    this->id = other.getID();
}

TimeSignatureEvent::TimeSignatureEvent(MidiLayer *owner,
                                       float newBeat,
                                       int newNumerator,
                                       int newDenominator) :
    MidiEvent(owner, newBeat),
    numerator(newNumerator),
    denominator(newDenominator)
{
}

TimeSignatureEvent::~TimeSignatureEvent()
{
}


Array<MidiMessage> TimeSignatureEvent::getSequence() const
{
	Array<MidiMessage> result;
    // TODO export time signature midi events
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
    e.id = this->createId();
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

void TimeSignatureEvent::reset()
{
}


int TimeSignatureEvent::hashCode() const noexcept
{
    return this->numerator + (100 * this->denominator) + this->id.hashCode();
}

TimeSignatureEvent &TimeSignatureEvent::operator=(const TimeSignatureEvent &right)
{
    //if (this == &right) { return *this; }
    //this->layer = *right.getLayer(); // never do this
    this->id = right.id;
    this->beat = right.beat;
    this->numerator = right.numerator;
    this->denominator = right.denominator;
    return *this;
}

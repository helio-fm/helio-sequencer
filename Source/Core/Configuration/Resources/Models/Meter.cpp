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
#include "Meter.h"
#include "SerializationKeys.h"

Meter::Meter(const Meter &other) noexcept :
    name(other.name), numerator(other.numerator), denominator(other.denominator) {}

Meter::Meter(const String &name, int numerator, int denominator) noexcept :
    name(name), numerator(numerator), denominator(denominator) {}

Meter Meter::withNumerator(const int newNumerator) const noexcept
{
    Meter m(*this);
    m.numerator = jlimit(Meter::minNumerator, Meter::maxNumerator, newNumerator);
    return m;
}

Meter Meter::withDenominator(const int newDenominator) const noexcept
{
    Meter m(*this);
    m.denominator = jlimit(Meter::minDenominator, Meter::maxDenominator, newDenominator);
    return m;
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

void Meter::parseString(const String &data, int &numerator, int &denominator)
{
    numerator = Globals::Defaults::timeSignatureNumerator;
    denominator = Globals::Defaults::timeSignatureDenominator;

    StringArray sa;
    sa.addTokens(data, "/\\|-", "' \"");

    if (sa.size() == 2)
    {
        const int n = sa[0].getIntValue();
        int d = sa[1].getIntValue();

        // Round to the power of two:
        d = int(pow(2, ceil(log(d) / log(2))));

        // Apply some reasonable constraints:
        denominator = jlimit(Meter::minDenominator, Meter::maxDenominator, d);
        numerator = jlimit(Meter::minNumerator, Meter::maxNumerator, n);
    }
}

bool Meter::isValid() const noexcept
{
    return this->numerator >= Meter::minNumerator &&
        this->numerator <= Meter::maxNumerator &&
        this->denominator >= Meter::minDenominator &&
        this->denominator <= Meter::maxDenominator;
}

String Meter::getLocalizedName() const
{
    return TRANS(this->name);
}

String Meter::getTimeAsString() const noexcept
{
    return String(this->numerator) + "/" + String(this->denominator);
}

float Meter::getBarLengthInBeats() const noexcept
{
    if (!this->isValid()) { return 0.f; }
    return float(this->numerator) / float(this->denominator) * float(Globals::beatsPerBar);
}

bool Meter::isCommonTime() const noexcept
{
    return this->numerator == 4 && this->denominator == 4;
}

int Meter::getNumerator() const noexcept
{
    return this->numerator;
}

int Meter::getDenominator() const noexcept
{
    return this->denominator;
}

Meter &Meter::operator=(const Meter &other)
{
    this->name = other.name;
    this->numerator = other.numerator;
    this->denominator = other.denominator;
    return *this;
}

bool operator==(const Meter &l, const Meter &r)
{
    return &l == &r || (l.name == r.name &&
        l.numerator == r.numerator && l.denominator == r.denominator);
}

bool operator!=(const Meter &l, const Meter &r)
{
    return !operator== (l, r);
}

bool Meter::isEquivalentTo(const Meter &other) const
{
    return this->numerator == other.numerator &&
        this->denominator == other.denominator;
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Meter::getResourceId() const noexcept
{
    // Assumed to be unique:
    return this->name;
}

Identifier Meter::getResourceType() const noexcept
{
    return Serialization::Resources::meters;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Meter::serialize() const
{
    SerializedData tree(Serialization::Midi::meter);
    tree.setProperty(Serialization::Midi::meterName, this->name);
    tree.setProperty(Serialization::Midi::meterTime, this->getTimeAsString());
    // todo metronome
    return tree;
}

void Meter::deserialize(const SerializedData &data)
{
    using namespace Serialization;
    const auto root = data.hasType(Midi::meter) ?
        data : data.getChildWithName(Midi::meter);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Midi::meterName, this->name);
    const String timeString = root.getProperty(Midi::meterTime);
    Meter::parseString(timeString, this->numerator, this->denominator);
    // todo metronome
}

void Meter::reset()
{
    this->name = {};
    this->numerator = 0;
    this->denominator = 0;
    // todo metronome
}

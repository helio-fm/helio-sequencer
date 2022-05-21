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
    name(other.name), metronome(other.metronome),
    numerator(other.numerator), denominator(other.denominator) {}

Meter::Meter(const String &name, const String &metronomeScheme,
    int numerator, int denominator) noexcept :
    name(name), numerator(numerator), denominator(denominator)
{
    if (metronomeScheme.isNotEmpty())
    {
        this->metronome.loadString(metronomeScheme);
    }
}

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

Meter Meter::withMetronome(const MetronomeScheme &scheme) const noexcept
{
    Meter m(*this);
    m.metronome = scheme;
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

const MetronomeScheme &Meter::getMetronome() const noexcept
{
    return this->metronome;
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
    this->metronome = other.metronome;
    return *this;
}

bool operator==(const Meter &l, const Meter &r)
{
    return &l == &r || (l.name == r.name &&
        l.numerator == r.numerator &&
        l.denominator == r.denominator &&
        l.metronome == r.metronome);
}

bool operator!=(const Meter &l, const Meter &r)
{
    return !operator==(l, r);
}

bool Meter::isEquivalentTo(const Meter &other) const
{
    return this->numerator == other.numerator &&
        this->denominator == other.denominator &&
        this->metronome == other.metronome;
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
    tree.setProperty(Serialization::Midi::metronomeScheme, this->metronome.toString());
    return tree;
}

void Meter::deserialize(const SerializedData &data)
{
    using namespace Serialization;
    const auto root = data.hasType(Midi::meter) ?
        data : data.getChildWithName(Midi::meter);

    if (!root.isValid()) { return; }

    this->reset();

    const String timeString = root.getProperty(Midi::meterTime);
    Meter::parseString(timeString, this->numerator, this->denominator);

    // if the name is not specified, use the time string (e.g. "5/4") as a name:
    this->name = root.getProperty(Midi::meterName, timeString);

    const String metronomeString = root.getProperty(Midi::metronomeScheme);
    this->metronome.loadString(metronomeString);
}

void Meter::reset()
{
    this->name = {};
    this->numerator = 0;
    this->denominator = 0;
    this->metronome.reset();
}

//===----------------------------------------------------------------------===//
// Metronome scheme
//===----------------------------------------------------------------------===//

bool operator==(const MetronomeScheme &l, const MetronomeScheme &r)
{
    return &l == &r || l.syllables == r.syllables;
}

bool operator!=(const MetronomeScheme &l, const MetronomeScheme &r)
{
    return !operator==(l, r);
}

String MetronomeScheme::toString() const
{
    jassert(!this->syllables.isEmpty());

    String result;

    for (const auto &syllable : this->syllables)
    {
        switch (syllable)
        {
            case Syllable::Oo: result << " oo"; break;
            case Syllable::na: result << "na"; break;
            case Syllable::Pa: result << " pa"; break;
            case Syllable::pa: result << "pa"; break;
        }
    }

    return result.trimStart();
}

void MetronomeScheme::loadString(const String &str)
{
    this->syllables.clearQuick();

    if (str.isEmpty())
    {
        return;
    }

    juce_wchar c;
    bool isStressedSyllable = true;
    auto ptr = str.getCharPointer();
    do
    {
        c = ptr.getAndAdvance();
        switch (c)
        {
            case ' ':
                isStressedSyllable = true;
                break;
            case 'O':
            case 'o':
                c = ptr.getAndAdvance();
                if (c == 'o')
                {
                    this->syllables.add(Syllable::Oo);
                }
                isStressedSyllable = false;
                break;
            case 'N':
            case 'n':
                c = ptr.getAndAdvance();
                if (c == 'a')
                {
                    jassert(!this->syllables.isEmpty());
                    jassert(this->syllables.getLast() == Syllable::Oo || this->syllables.getLast() == Syllable::Pa);
                    this->syllables.add(Syllable::na);
                }
                isStressedSyllable = false;
                break;
            case 'P':
            case 'p':
                c = ptr.getAndAdvance();
                if (c == 'a')
                {
                    if (isStressedSyllable)
                    {
                        this->syllables.add(Syllable::Pa);
                    }
                    else
                    {
                        jassert(!this->syllables.isEmpty());
                        jassert(this->syllables.getLast() == Syllable::Pa || this->syllables.getLast() == Syllable::na);
                        this->syllables.add(Syllable::pa);
                    }
                }
                isStressedSyllable = false;
                break;
            default:
                isStressedSyllable = false;
                break;
        }
    } while (c != 0);
}

void MetronomeScheme::reset()
{
    this->syllables.clearQuick();
}

bool MetronomeScheme::isValid() const noexcept
{
    return this->syllables.size() > 1;
}

MetronomeScheme::Syllable MetronomeScheme::getNextSyllable(Syllable syllable) noexcept
{
    switch (syllable)
    {
        case Syllable::Oo: return Syllable::pa;
        case Syllable::pa: return Syllable::na;
        case Syllable::na: return Syllable::Pa;
        case Syllable::Pa: return Syllable::Oo;
    }

    return Syllable::Oo;
}

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

String &operator<<(String &s1, const Array<MetronomeScheme::Syllable> &syllables)
{
    for (const auto &s : syllables)
    {
        s1 += int(s);
    }

    return s1;
}

class MetronomeSchemeTests final : public UnitTest
{
public:

    MetronomeSchemeTests() :
        UnitTest("Metronome scheme tests", UnitTestCategories::helio) {}

    void runTest() override
    {
        beginTest("Metronome scheme serialization");

        // shortcuts
        const auto Oo = MetronomeScheme::Syllable::Oo;
        const auto na = MetronomeScheme::Syllable::na;
        const auto Pa = MetronomeScheme::Syllable::Pa;
        const auto pa = MetronomeScheme::Syllable::pa;

        MetronomeScheme m;

        // the default
        expectEquals(m.syllables, { Oo, na, Pa, na });
        expectEquals(m.toString(), { "oona pana" });

        // invalid string
        m.loadString("12345");
        expect(!m.isValid());

        // valid string
        m.loadString("Oonapa Panapa Panapa");
        expect(m.isValid());
        expectEquals(m.syllables,
        {
            Oo, na, pa,
            Pa, na, pa,
            Pa, na, pa
        });

        // test the lowercase form with some garbage
        m.loadString("oonapa panapa skip skip skip 12345 panapa pana papa");
        expect(m.isValid());
        expectEquals(m.syllables,
        {
            Oo, na, pa,
            Pa, na, pa,
            Pa, na, pa,
            Pa, na,
            Pa, pa
        });

        expectEquals(m.toString(), { "oonapa panapa panapa pana papa" });
    }
};

static MetronomeSchemeTests metronomeSchemeTests;

#endif

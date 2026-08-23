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
#include "Temperament.h"
#include "SerializationKeys.h"

Temperament::Temperament(const Temperament &other) noexcept :
    id(other.id), name(other.name),
    period(other.period), keysTotal(other.keysTotal),
    middleC(other.middleC), middleA(other.middleA),
    highlighting(other.highlighting), chromaticMap(other.chromaticMap),
    chromaticScales(other.chromaticScales) {}

Temperament::Temperament(Temperament &&other) noexcept :
    id(other.id), name(other.name), keysTotal(other.keysTotal),
    middleC(other.middleC), middleA(other.middleA),
    highlighting(other.highlighting), chromaticMap(other.chromaticMap),
    chromaticScales(other.chromaticScales)
{
    this->period.swapWith(other.period);
}

// key naming is a can of worms, because the piano roll displays both
// the chromatic scale (the grid) and the key signature's scale (the highlighted rows),
// but in the traditional notation language note names can differ between those scales;
// the roll still needs to name notes somehow, so it will stick to chromatic scale
// namings only, allowing to pick one of the enharmonic equivalents in the key signature
// and display custom chromatic scale names for it, if specified in the temperament

String Temperament::getMidiNoteName(Note::Key note, int scaleRootKey,
    const String &rootKeyEnharmonic, int &outPeriodNumber) const noexcept
{
    if (isPositiveAndBelow(note, this->getNumKeys()))
    {
        const auto rootKeyNameFallback = this->period[scaleRootKey % this->getPeriodSize()][0];
        const auto foundChromaticScale = this->chromaticScales.find(
            rootKeyEnharmonic.isEmpty() ? rootKeyNameFallback : rootKeyEnharmonic);

        const auto inScaleKey = Scale::wrapKey(note - scaleRootKey, 0, this->getPeriodSize());

        const auto noteNameFallback = this->period[note % this->getPeriodSize()][0];
        const String result = foundChromaticScale != this->chromaticScales.end() ?
            foundChromaticScale->second[inScaleKey] :
            noteNameFallback;

        outPeriodNumber = note / this->getPeriodSize() +
            (Temperament::displayedPeriodNumForMiddleC - Temperament::periodNumForMiddleC);

        return result;
    }

    outPeriodNumber = 0;
    return {};
}

double Temperament::getNoteInHertz(double noteNumber, double frequencyOfA /*= 440.0*/) const noexcept
{
    return frequencyOfA * std::pow(this->periodRange,
        double(noteNumber - this->middleA) / double(this->getPeriodSize()));
}

Note::Key Temperament::unmapMicrotonalNote(int mappedNoteNumber, int mappedChannel) noexcept
{
    jassert(mappedChannel > 0);
    return this->getPeriodSize() > Globals::twelveTonePeriodSize ?
        mappedNoteNumber + Globals::twelveToneKeyboardSize * (mappedChannel - 1) :
        mappedNoteNumber;
}

Note::Key Temperament::getEquivalentOfTwelveToneInterval(Semitones interval) const noexcept
{
    return this->chromaticMap->getChromaticKey(int(interval), 0, false);
}

//===----------------------------------------------------------------------===//
// ConfigurationResource
//===----------------------------------------------------------------------===//

String Temperament::getResourceId() const noexcept
{
    return this->id;
}

//===----------------------------------------------------------------------===//
// Hard-coded defaults
//===----------------------------------------------------------------------===//

static const String defaultTemperamentId = "12edo";

static StringArray makeChromaticNames(const String &rootKeyEnharmonic,
    const Temperament::Period &period)
{
    int rootIndex = -1;
    for (int i = 0; i < period.size(); ++i)
    {
        if (period.getReference(i).indexOf(rootKeyEnharmonic) >= 0)
        {
            rootIndex = i;
            break;
        }
    }

    if (rootIndex < 0)
    {
        jassertfalse;
        return {};
    }

    const auto isFlatRoot = rootKeyEnharmonic.endsWithChar('b');

    StringArray result;
    result.add(rootKeyEnharmonic);
    for (int i = 1; i < period.size(); ++i)
    {
        const auto index = (i + rootIndex) % period.size();
        const auto &keyTokens = period.getReference(index);

        bool foundToken = false;
        for (const auto &keyToken : keyTokens)
        {
            if (keyToken.length() == 1)
            {
                result.add(keyToken);
                foundToken = true;
                break;
            }

            const auto isFlatKey = keyToken.endsWithChar('b');
            if ((isFlatRoot && isFlatKey) || (!isFlatRoot && !isFlatKey))
            {
                result.add(keyToken);
                foundToken = true;
                break;
            }
        }

        if (!foundToken)
        {
            jassert(!keyTokens.isEmpty());
            result.add(keyTokens[0]);
        }
    }

    return result;
}

static FlatHashMap<String, StringArray, StringHash> makeChromaticScales(const Temperament::Period &period)
{
    FlatHashMap<String, StringArray, StringHash> result;

    for (const auto &keyTokens : period)
    {
        for (const auto &keyName : keyTokens)
        {
            result[keyName] = makeChromaticNames(keyName, period);
        }
    }

    return result;
}

Temperament::Ptr Temperament::makeTwelveToneEqualTemperament() noexcept
{
    Temperament::Ptr t(new Temperament());
    t->id = defaultTemperamentId;
    t->name = "12 equal temperament";
    t->period = {
        StringArray("C", "B#"),
        StringArray("C#", "Db"),
        StringArray("D"),
        StringArray("Eb", "D#"),
        StringArray("E", "Fb"),
        StringArray("F", "E#"),
        StringArray("F#", "Gb"),
        StringArray("G"),
        StringArray("G#", "Ab"),
        StringArray("A"),
        StringArray("Bb", "A#"),
        StringArray("B", "Cb") };
    t->periodRange = 2.0;
    t->highlighting = Scale::makeNaturalMajorScale();
    t->chromaticMap = Scale::makeChromaticScale();
    t->keysTotal = Globals::twelveToneKeyboardSize;
    t->middleC = Globals::twelveTonePeriodSize * Temperament::periodNumForMiddleC;
    t->middleA = t->middleC + Note::Key(Semitones::MajorSixth);
    t->chromaticScales = makeChromaticScales(t->period);
    return t;
}

bool Temperament::isDefault() const noexcept
{
    return this->keysTotal == Globals::twelveToneKeyboardSize &&
        this->getPeriodSize() == Globals::twelveTonePeriodSize &&
        this->id == defaultTemperamentId;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Temperament::serialize() const noexcept
{
    using namespace Serialization;

    SerializedData data(Midi::temperament);

    data.setProperty(Midi::temperamentId, this->id);
    data.setProperty(Midi::temperamentName, this->name);

    StringArray periodString;
    for (const auto &enharmonics : this->period)
    {
        periodString.add(enharmonics.joinIntoString("/"));
    }
    data.setProperty(Midi::temperamentPeriod, periodString.joinIntoString(" "));

    data.setProperty(Midi::temperamentPeriodRange, this->periodRange);
    data.setProperty(Midi::temperamentHighlighting, this->highlighting->getIntervals());
    data.setProperty(Midi::temperamentChromaticMap, this->chromaticMap->getIntervals());

    // hack warning:
    // temperaments are only serialized when they are saved as a part of the project,
    // but upon loading projects treat them as references to the most recent temperament
    // models, which are used instead, or as a fallback if no model with such id is found,
    // this way it's more convenient to play around with notes namings etc;
    // because of this temperament only serializes essential info, e.g. skips chromaticScales

    return data;
}

void Temperament::deserialize(const SerializedData &data) noexcept
{
    using namespace Serialization;

    const auto root = data.hasType(Midi::temperament) ?
        data : data.getChildWithName(Midi::temperament);

    if (!root.isValid()) { return; }

    this->reset();

    this->id = root.getProperty(Midi::temperamentId, this->id);
    this->name = root.getProperty(Midi::temperamentName, this->name);

    const String periodString = root.getProperty(Midi::temperamentPeriod);
    StringArray enharmonicTokens;
    enharmonicTokens.addTokens(periodString, true);
    this->period.clearQuick();
    for (const auto &keyTokens : enharmonicTokens)
    {
        StringArray keys;
        keys.addTokens(keyTokens, "/", "");
        this->period.add(move(keys));
    }

    this->periodRange = root.getProperty(Midi::temperamentPeriodRange, 2.0);

    this->highlighting = Scale::fromIntervalsAndPeriod(
        root.getProperty(Midi::temperamentHighlighting), this->getPeriodSize());

    if (!this->highlighting->isValid())
    {
        this->highlighting = Scale::makeNaturalMajorScale();
    }

    this->chromaticMap = Scale::fromIntervalsAndPeriod(
        root.getProperty(Midi::temperamentChromaticMap), this->getPeriodSize());

    if (!this->chromaticMap->isValid())
    {
        this->chromaticMap = Scale::makeChromaticScale();
    }

    this->chromaticScales = makeChromaticScales(this->period);

    this->keysTotal = int(Globals::numPeriodsInKeyboard * float(this->getPeriodSize()));
    this->middleC = Temperament::periodNumForMiddleC * this->getPeriodSize();
    this->middleA = this->middleC + this->getEquivalentOfTwelveToneInterval(Semitones::MajorSixth);
}

void Temperament::reset() noexcept
{
    this->id = {};
    this->period = {};
    this->period.clearQuick();
    this->highlighting.reset();
    this->chromaticMap.reset();
    this->chromaticScales.clear();
}

Temperament &Temperament::operator=(const Temperament &other)
{
    this->id = other.id;
    this->name = other.name;
    this->period = other.period;
    this->middleC = other.middleC;
    this->keysTotal = other.keysTotal;
    this->highlighting = other.highlighting;
    this->chromaticMap = other.chromaticMap;
    this->chromaticScales = other.chromaticScales;
    return *this;
}

int Temperament::hashCode() const noexcept
{
    return static_cast<int>(this->period.size() + this->id.hash());
}

bool operator==(const Temperament &l, const Temperament &r)
{
    return &l == &r || (l.id == r.id && l.period == r.period);
}

bool operator!=(const Temperament &l, const Temperament &r)
{
    return !operator== (l, r);
}

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

class NoteNamingTests final : public UnitTest
{
public:

    NoteNamingTests() :
        UnitTest("Note naming tests", UnitTestCategories::helio) {}

    static Temperament::Period makePeriod(const String &periodString)
    {
        StringArray enharmonicTokens;
        enharmonicTokens.addTokens(periodString, true);
        Temperament::Period result;
        for (const auto &keyTokens : enharmonicTokens)
        {
            StringArray keys;
            keys.addTokens(keyTokens, "/", "");
            result.add(move(keys));
        }
        return result;
    }

    static String makeScaleString(const String &keyName, const Temperament::Period &period)
    {
        return makeChromaticNames(keyName, period).joinIntoString(" ");
    }

    void runTest() override
    {
        beginTest("12-edo chromatic scales");

        {
            const auto p = makePeriod("C/B# C#/Db D Eb/D# E/Fb F/E# F#/Gb G G#/Ab A Bb/A# B/Cb");
            expect(makeScaleString("A", p) == "A A# B C C# D D# E F F# G G#");
            expect(makeScaleString("Ab", p) == "Ab A Bb B C Db D Eb E F Gb G");
            expect(makeScaleString("A#", p) == "A# B C C# D D# E F F# G G# A");
            expect(makeScaleString("B", p) == "B C C# D D# E F F# G G# A A#");
            expect(makeScaleString("Bb", p) == "Bb B C Db D Eb E F Gb G Ab A");
            expect(makeScaleString("B#", p) == "B# C# D D# E F F# G G# A A# B");
            expect(makeScaleString("C", p) == "C C# D D# E F F# G G# A A# B");
            expect(makeScaleString("Cb", p) == "Cb C Db D Eb E F Gb G Ab A Bb");
            expect(makeScaleString("C#", p) == "C# D D# E F F# G G# A A# B C");
            expect(makeScaleString("D", p) == "D D# E F F# G G# A A# B C C#");
            expect(makeScaleString("Db", p) == "Db D Eb E F Gb G Ab A Bb B C");
            expect(makeScaleString("D#", p) == "D# E F F# G G# A A# B C C# D");
            expect(makeScaleString("E", p) == "E F F# G G# A A# B C C# D D#");
            expect(makeScaleString("Eb", p) == "Eb E F Gb G Ab A Bb B C Db D");
            expect(makeScaleString("E#", p) == "E# F# G G# A A# B C C# D D# E");
            expect(makeScaleString("F", p) == "F F# G G# A A# B C C# D D# E");
            expect(makeScaleString("Fb", p) == "Fb F Gb G Ab A Bb B C Db D Eb");
            expect(makeScaleString("F#", p) == "F# G G# A A# B C C# D D# E F");
            expect(makeScaleString("G", p) == "G G# A A# B C C# D D# E F F#");
            expect(makeScaleString("Gb", p) == "Gb G Ab A Bb B C Db D Eb E F");
            expect(makeScaleString("G#", p) == "G# A A# B C C# D D# E F F# G");
        }

        beginTest("19-edo chromatic scales");

        {
            const auto p = makePeriod("C/Bx C#/Dbb Db/Cx D D#/Ebb Eb/Dx E/Fbb E#/Fb F/Ex F#/Gbb Gb/Fx G G#/Abb Ab/Gx A A#/Bbb Bb/Ax B/Cbb B#/Cb");
            expect(makeScaleString("A", p) == "A A# Ax B B# C C# Cx D D# Dx E E# F F# Fx G G# Gx");
            expect(makeScaleString("Ab", p) == "Ab A Bbb Bb B Cb C Dbb Db D Ebb Eb E Fb F Gbb Gb G Abb");
            expect(makeScaleString("Abb", p) == "Abb Ab A Bbb Bb B Cb C Dbb Db D Ebb Eb E Fb F Gbb Gb G");
            expect(makeScaleString("A#", p) == "A# Ax B B# C C# Cx D D# Dx E E# F F# Fx G G# Gx A");
            expect(makeScaleString("Ax", p) == "Ax B B# C C# Cx D D# Dx E E# F F# Fx G G# Gx A A#");
            expect(makeScaleString("B", p) == "B B# C C# Cx D D# Dx E E# F F# Fx G G# Gx A A# Ax");
            expect(makeScaleString("Bb", p) == "Bb B Cb C Dbb Db D Ebb Eb E Fb F Gbb Gb G Abb Ab A Bbb");
            expect(makeScaleString("Bbb", p) == "Bbb Bb B Cb C Dbb Db D Ebb Eb E Fb F Gbb Gb G Abb Ab A");
            expect(makeScaleString("B#", p) == "B# C C# Cx D D# Dx E E# F F# Fx G G# Gx A A# Ax B");
            expect(makeScaleString("Bx", p) == "Bx C# Cx D D# Dx E E# F F# Fx G G# Gx A A# Ax B B#");
            // hopefully the rest is ok too, too lazy to write them all
        }
    }
};

static NoteNamingTests noteNamingTests;

#endif

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
    middleC(other.middleC),  middleA(other.middleA),
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
        const auto foundUserScale = this->chromaticScales.find(
            rootKeyEnharmonic.isEmpty() ? rootKeyNameFallback : rootKeyEnharmonic);

        const auto inScaleKey = Scale::wrapKey(note - scaleRootKey, 0, this->getPeriodSize());

        const auto noteNameFallback = this->period[note % this->getPeriodSize()][0];
        const String result = foundUserScale != this->chromaticScales.end() ?
            foundUserScale->second[inScaleKey] :
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
// BaseResource
//===----------------------------------------------------------------------===//

String Temperament::getResourceId() const noexcept
{
    return this->id;
}

Identifier Temperament::getResourceType() const noexcept
{
    return Serialization::Resources::temperaments;
}

//===----------------------------------------------------------------------===//
// Hard-coded defaults
//===----------------------------------------------------------------------===//

static const String defaultTemperamentId = "12edo";

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

SerializedData Temperament::serialize() const
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

void Temperament::deserialize(const SerializedData &data)
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

    this->chromaticScales.clear();
    forEachChildWithType(root, e, Midi::temperamentChromaticScale)
    {
        const String keyName = e.getProperty(Midi::key);
        const String scaleTokens = e.getProperty(Midi::scale);

        StringArray chromaticScale;
        chromaticScale.addTokens(scaleTokens, false);

        if (!keyName.isEmpty() && !chromaticScale.isEmpty())
        {
            this->chromaticScales[keyName] = move(chromaticScale);
        }
    }
    
    this->keysTotal = int(Globals::numPeriodsInKeyboard * float(this->getPeriodSize()));
    this->middleC = Temperament::periodNumForMiddleC * this->getPeriodSize();
    this->middleA = this->middleC + this->getEquivalentOfTwelveToneInterval(Semitones::MajorSixth);
}

void Temperament::reset()
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

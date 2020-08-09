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
#include "Temperament.h"
#include "SerializationKeys.h"

Temperament::Temperament(const Temperament &other) noexcept :
    id(other.id), name(other.name),
    period(other.period), middleC(other.middleC) {}

Temperament::Temperament(Temperament &&other) noexcept :
    id(other.id), name(other.name), middleC(other.middleC)
{
    this->period.swapWith(other.period);
}

String Temperament::getMidiNoteName(Note::Key note, bool includePeriod) const noexcept
{
    if (isPositiveAndBelow(note, this->getNumKeys()))
    {
        String result(this->period[note % this->getPeriodSize()]);

        if (includePeriod)
        {
            result << (note / this->getPeriodSize() +
                (Temperament::displayedPeriodNumForMiddleC - Temperament::periodNumForMiddleC));
        }

        return result;
    }

    return {};
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

Temperament::Ptr Temperament::getTwelveToneEqualTemperament()
{
    Temperament::Ptr t(new Temperament());
    t->id = "12edo";
    t->name = "12 equal temperament";
    t->period = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    t->keysTotal = Globals::twelveToneKeyboardSize;
    t->middleC = Globals::twelveTonePeriodSize * Temperament::periodNumForMiddleC;
    return t;
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
    data.setProperty(Midi::temperamentPeriod, this->period.joinIntoString(" "));

    data.setProperty(Midi::temperamentHighlighting, this->highlighting.getIntervals());
    data.setProperty(Midi::temperamentChromaticMap, this->chromaticMap.getIntervals());

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
    this->period.addTokens(periodString, true);

    // other parameters are computed quite straightforward, but let's do it here:
    this->middleC = Temperament::periodNumForMiddleC * this->getPeriodSize();
    this->keysTotal = int(Globals::numPeriodsInKeyboard * float(this->getPeriodSize()));

    this->highlighting = Scale::fromIntervalsAndPeriod(
        root.getProperty(Midi::temperamentHighlighting), this->getPeriodSize());

    this->chromaticMap = Scale::fromIntervalsAndPeriod(
        root.getProperty(Midi::temperamentChromaticMap), this->getPeriodSize());
}

void Temperament::reset()
{
    this->id = {};
    this->period = {};
    this->period.clearQuick();
    this->highlighting.reset();
    this->chromaticMap.reset();
}

Temperament &Temperament::operator=(const Temperament &other)
{
    this->id = other.id;
    this->name = other.name;
    this->period = other.period;
    this->middleC = other.middleC;
    this->keysTotal = other.keysTotal;
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

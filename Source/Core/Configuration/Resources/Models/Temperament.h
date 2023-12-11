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

#pragma once

#include "ConfigurationResource.h"
#include "Scale.h"
#include "Note.h"

// just 12-tone intervals, named to avoid using magic numbers:
enum class Semitones : int8
{
    PerfectUnison = 0,
    MinorSecond = 1,
    MajorSecond = 2,
    MinorThird = 3,
    MajorThird = 4,
    PerfectFourth = 5,
    Tritone = 6,
    PerfectFifth = 7,
    MinorSixth = 8,
    MajorSixth = 9,
    MinorSeventh = 10,
    MajorSeventh = 11,
    PerfectOctave = 12
};

class Temperament final : public ConfigurationResource
{
public:

    Temperament() = default;
    Temperament(Temperament &&other) noexcept;
    Temperament(const Temperament &other) noexcept;

    String getResourceId() const noexcept override;
    Identifier getResourceType() const noexcept override;

    using Period = StringArray;
    using Ptr = ReferenceCountedObjectPtr<Temperament>;

    inline const String &getName() const noexcept { return this->name; }
    inline const Period &getPeriod() const noexcept { return this->period; }
    inline int getPeriodSize() const noexcept { return this->period.size(); }
    inline double getPeriodRange() const noexcept{ return this->periodRange; }
    inline int getNumKeys() const noexcept { return this->keysTotal; }
    inline Note::Key getMiddleC() const noexcept { return this->middleC; }

    bool isDefault() const noexcept;

    const Scale::Ptr getHighlighting() const noexcept { return this->highlighting; }
    const Scale::Ptr getChromaticMap() const noexcept { return this->chromaticMap; }
    Note::Key getEquivalentOfTwelveToneInterval(Semitones interval) const noexcept;

    String getMidiNoteName(Note::Key note, bool includePeriod) const noexcept;

    //===------------------------------------------------------------------===//
    // Hard-coded defaults
    //===------------------------------------------------------------------===//

    static Temperament::Ptr getTwelveToneEqualTemperament();

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Operators
    //===------------------------------------------------------------------===//

    Temperament &operator=(const Temperament &other);
    friend bool operator==(const Temperament &l, const Temperament &r);
    friend bool operator!=(const Temperament &l, const Temperament &r);
    
    int hashCode() const noexcept;

    static constexpr auto periodNumForMiddleC = 5;
    static constexpr auto displayedPeriodNumForMiddleC = 4;

private:

    String id;
    String name;
    Period period;
    double periodRange = 2.0;

    Note::Key middleC = 0;
    int keysTotal = 0;

    Scale::Ptr highlighting;
    Scale::Ptr chromaticMap;

    JUCE_LEAK_DETECTOR(Temperament)
};

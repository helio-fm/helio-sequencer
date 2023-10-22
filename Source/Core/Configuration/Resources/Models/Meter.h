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

#include "Chord.h"
#include "ConfigurationResource.h"

class MetronomeScheme final
{
public:

    enum class Syllable : int
    {
        Oo, na, Pa, pa
    };

    MetronomeScheme &operator=(const MetronomeScheme &r) = default;
    friend bool operator==(const MetronomeScheme &l, const MetronomeScheme &r);
    friend bool operator!=(const MetronomeScheme &l, const MetronomeScheme &r);

    String toString() const;
    void loadString(const String &str);
    void reset();

    bool isValid() const noexcept;
    int getSize() const noexcept;
    Syllable getSyllableAt(int index) const noexcept;
    const Array<Syllable> &getSyllables() const noexcept;

    MetronomeScheme withSyllableAt(int index, Syllable newValue) const;
    MetronomeScheme resized(int size) const;

    static Syllable getNextSyllable(Syllable syllable) noexcept;
    
    // ordered by "loudness" descending:
    static Array<Syllable> getAllOrdered();

    static String syllableToString(Syllable syllable);
    static Syllable syllableFromString(const String &value);

    struct SyllableHash
    {
        inline HashCode operator()(const Syllable &syllable) const noexcept
        {
            return static_cast<HashCode>(syllable);
        }
    };

private:

    // the simple default oona-pana scheme:
    static Array<Syllable> getDefaultScheme();

    Array<Syllable> syllables = getDefaultScheme();
};

class Meter final : public ConfigurationResource
{
public:

    Meter() = default;
    Meter(const Meter &other) noexcept;
    Meter(const String &name, const String &metronomeScheme,
        int numerator, int denominator) noexcept;

    String getResourceId() const noexcept override;
    Identifier getResourceType() const noexcept override;
    using Ptr = ReferenceCountedObjectPtr<Meter>;

    Meter withNumerator(const int newNumerator) const noexcept;
    Meter withDenominator(const int newDenominator) const noexcept;
    Meter withMetronome(const MetronomeScheme &scheme) const noexcept;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    bool isValid() const noexcept;
    String getLocalizedName() const;

    bool isCommonTime() const noexcept;
    int getNumerator() const noexcept;
    int getDenominator() const noexcept;
    float getBarLengthInBeats() const noexcept;
    float getDenominatorInBeats() const noexcept;

    String getTimeAsString() const noexcept;

    const MetronomeScheme &getMetronome() const noexcept;

    static void parseString(const String &data, int &numerator, int &denominator);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Operators
    //===------------------------------------------------------------------===//

    Meter &operator=(const Meter &other);
    friend bool operator==(const Meter &l, const Meter &r);
    friend bool operator!=(const Meter &l, const Meter &r);

    // checks if numerator, denominator and metronome scheme are the same
    bool isEquivalentTo(const Meter &other) const;

private:

    String name;

    int numerator = 0;
    int denominator = 0;

    MetronomeScheme metronome;

    static constexpr auto minNumerator = 1;
    static constexpr auto maxNumerator = 64;

    static constexpr auto minDenominator = 2;
    static constexpr auto maxDenominator = 32;

    JUCE_LEAK_DETECTOR(Meter)
};

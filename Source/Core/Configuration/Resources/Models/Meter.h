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

#pragma once

#include "Chord.h"
#include "ConfigurationResource.h"

class Meter final : public ConfigurationResource
{
public:

    Meter() = default;
    Meter(const Meter &other) noexcept;
    Meter(const String &name, int numerator, int denominator) noexcept;

    String getResourceId() const noexcept override;
    Identifier getResourceType() const noexcept override;
    using Ptr = ReferenceCountedObjectPtr<Meter>;

    Meter withNumerator(const int newNumerator) const noexcept;
    Meter withDenominator(const int newDenominator) const noexcept;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    bool isValid() const noexcept;
    String getLocalizedName() const;

    bool isCommonTime() const noexcept;
    int getNumerator() const noexcept;
    int getDenominator() const noexcept;
    String getTimeAsString() const noexcept;

    float getBarLengthInBeats() const noexcept;

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

private:

    String name;

    int numerator = 0;
    int denominator = 0;

    static constexpr auto minNumerator = 2;
    static constexpr auto maxNumerator = 64;

    static constexpr auto minDenominator = 2;
    static constexpr auto maxDenominator = 32;

    JUCE_LEAK_DETECTOR(Meter)
};

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

#include "Serializable.h"

class Scale final : public Serializable
{
public:

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    int getSize() const noexcept;
    String getName() const noexcept;
    String getLocalizedName() const;

    Array<int> getPowerChord(int base) const;
    Array<int> getTriad(int base) const;
    Array<int> getSeventhChord(int base) const;

    // Flat third considered "minor"-ish
    // (like Aeolian, Phrygian, Locrian, etc.)
    bool seemsMinor() const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    // Key should be from 0 to 6
    // Returned key starts from 0 as well
    int getKey(int key, bool limitToSingleOctave = false) const;

    String name;

    // Simply holds key indices for chromatic scale
    // accessed by index in target scale,
    // e.g. for major: keys[0] = 0, keys[1] = 2, keys[2] = 4, etc
    Array<int> keys;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Scale)
};

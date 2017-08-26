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

class Pattern;

// Just an instance of a midi layer on a certain position
class Clip : public Serializable
{
public:

    using Id = String;

    Clip();
    Clip(const Clip &other);
    explicit Clip(Pattern *owner, float beatVal = 0.f);

    Pattern *getPattern() const noexcept;
    float getStartBeat() const noexcept;
    String getId() const noexcept;

    Clip copyWithNewId(Pattern *newOwner = nullptr) const;
    Clip withParameters(const XmlElement &xml) const;
    Clip withDeltaBeat(float deltaPosition) const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    Clip &operator=(const Clip &right);

    friend inline bool operator==(const Clip &lhs, const Clip &rhs)
    {
        return (&lhs == &rhs || lhs.id == rhs.id);
    }

    static int compareElements(const Clip &first,
        const Clip &second);

    int hashCode() const noexcept;

private:

    Pattern *pattern;

    float startBeat;
    String id;

    static Id createId() noexcept;

    JUCE_LEAK_DETECTOR(Clip);
};

class ClipHashFunction
{
public:

    static int generateHash(Clip clip, const int upperLimit) noexcept
    {
        return static_cast<int>((static_cast<uint32>(clip.hashCode())) % static_cast<uint32>(upperLimit));
    }
};

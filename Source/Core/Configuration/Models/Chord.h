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

#include "BaseResource.h"

class Chord final : public BaseResource
{
public:

    Chord() noexcept;
    Chord(const Chord &other) noexcept;
    explicit Chord(const String &name) noexcept;

    String getResourceId() const noexcept override;
    Identifier getResourceType() const noexcept override;
    using Ptr = ReferenceCountedObjectPtr<Chord>;

    struct Key final
    {
        enum class InScale : int
        {
            I = 0,
            II = 1,
            III = 2,
            IV = 3,
            V = 4,
            VI = 5,
            VII = 6,
            VIII = 7,
            IX = 8,
            X = 9,
            XI = 10,
            XII = 11,
            XIII = 12,
            XIV = 13
        };

        Key() = default;
        Key(InScale key) : key(key) {}; // no explicit for convenience
        Key(InScale key, bool aug, bool dim) :
            key(key), isAugmented(aug), isDiminished(dim) {};

        int getInScaleKey() const noexcept
        {
            return int(this->key);
        }

        int getChromaticOffset() const noexcept
        {
            return this->isAugmented ? 1 : (this->isDiminished ? -1 : 0);
        }

        String getStringValue() const noexcept
        {
            return String(this->getInScaleKey() + 1) +
                (this->isAugmented ? "#" : (this->isDiminished ? "b" : ""));
        }

    private:

        InScale key = InScale::I;

        bool isAugmented = false;
        bool isDiminished = false;

        JUCE_LEAK_DETECTOR(Key)
    };

    const bool isValid() const noexcept;
    const String &getName() const noexcept;
    const Array<Key> &getScaleKeys() const noexcept;

    static Array<String> getLocalizedFunctionNames();

    //===------------------------------------------------------------------===//
    // Hard-coded defaults
    //===------------------------------------------------------------------===//

    static Chord::Ptr getTriad();
    static Chord::Ptr getPowerChord();
    static Chord::Ptr getSeventhChord();

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    String name;

    // Chord keys indices for target scale
    Array<Key> scaleKeys;

    // todo in future:
    //int basePeriod;

    JUCE_LEAK_DETECTOR(Chord)
};

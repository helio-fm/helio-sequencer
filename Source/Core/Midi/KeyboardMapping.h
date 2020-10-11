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
#include "Note.h"

class KeyboardMapping
{
public:

    virtual ~KeyboardMapping() = default;

    virtual void map(Note::Key &key, int &channel) = 0;

};

class SimpleKeyboardMapping final : public KeyboardMapping
{
public:

    void map(Note::Key &key, int &channel) noexcept override
    {
        key = key % Globals::twelveToneKeyboardSize;
        channel += (key / Globals::twelveToneKeyboardSize);
    }
};

class ScalaKeyboardMapping final : public KeyboardMapping, public Serializable
{
public:

    ScalaKeyboardMapping();

    void map(Note::Key &key, int &channel) noexcept override
    {
        const auto keyIndex = key % Globals::twelveToneKeyboardSize;
        const auto channelIndex = (key / Globals::twelveToneKeyboardSize);
        key = this->map[keyIndex][channelIndex];
        channel += channelIndex; // or just = ?
    }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    void loadScalaKbm(const Array<File> &files);

private:

    Note::Key map[Globals::twelveToneKeyboardSize][Globals::numChannels];

};

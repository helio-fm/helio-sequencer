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

class KeyboardMapping : public Serializable
{
public:

    virtual ~KeyboardMapping() = default;

    static constexpr auto maxMappedKeys = 1024;

    virtual void map(Note::Key &key, int &channel) const = 0;

    JUCE_DECLARE_WEAK_REFERENCEABLE(KeyboardMapping)
};

class SimpleKeyboardMapping final : public KeyboardMapping
{
public:

    void map(Note::Key &key, int &channel) const noexcept override
    {
        key = key % Globals::twelveToneKeyboardSize;
        channel = key / Globals::twelveToneKeyboardSize;
    }

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;
};

class CustomKeyboardMapping final : public KeyboardMapping
{
public:

    CustomKeyboardMapping();

    void map(Note::Key &key, int &channel) const noexcept override
    {
        channel = this->index[key].channel;
        key = this->index[key].key;
    }

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    void loadScalaKbm(const Array<File> &files);

private:

    struct MidiKeyAndChannel final
    {
        MidiKeyAndChannel() = default;
        MidiKeyAndChannel(int8 key, int8 channel) :
            key(key), channel(channel) {}

        int8 key = 0;
        int8 channel = 0;
    };

    MidiKeyAndChannel index[maxMappedKeys];

};

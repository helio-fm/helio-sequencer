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

class KeyboardMapping final :
    public Serializable,
    public ChangeBroadcaster // notifies KeyboardMappingPage
{
public:

    KeyboardMapping();

    static constexpr auto numMappedKeys = 
        Globals::numChannels * Globals::twelveToneKeyboardSize;

    struct KeyChannel final
    {
        KeyChannel() = default;
        KeyChannel(const KeyChannel &other) = default;

        KeyChannel(int8 key, int8 channel) :
            key(key), channel(channel) {}

        friend inline bool operator==(const KeyChannel &l, const KeyChannel &r)
        {
            return l.key == r.key && l.channel == r.channel;
        }

        friend inline bool operator!=(const KeyChannel &l, const KeyChannel &r)
        {
            return !(l == r);
        }

        KeyChannel getNextDefault() const noexcept;

        static KeyChannel fromString(const String &str);
        String toString() const noexcept;

        bool isValid() const noexcept;

        int8 key = -1;
        int8 channel = -1;
    };

    void map(Note::Key &key, int &channel) const noexcept
    {
        channel = this->index[key].channel;
        key = this->index[key].key;
    }

    KeyChannel map(const Note::Key &key) const noexcept
    {
        return this->index[key];
    }

    void updateKey(int key, const KeyChannel &keyChannel);
    void updateKey(int key, int8 targetKey, int8 targetChannel);

    // this method will not reset before loading a mapping,
    // because there can be a number of files for multiple channels,
    // so don't forget to reset() before calling this
    void loadScalaKbmFile(InputStream &fileContentStream,
        const String &fileNameWithoutExtension);

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    static KeyChannel getDefaultMappingFor(int key) noexcept;

    KeyChannel index[numMappedKeys];

    JUCE_DECLARE_WEAK_REFERENCEABLE(KeyboardMapping)
};

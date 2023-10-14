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

#include "Serializable.h"
#include "ConfigurationResource.h"
#include "Note.h"

class KeyboardMapping final :
    public ConfigurationResource,
    public ChangeBroadcaster // notifies KeyboardMappingPage
{
public:

    KeyboardMapping();

    static constexpr auto numMappedKeys = 
        Globals::numChannels * Globals::twelveToneKeyboardSize;

    static constexpr auto numMappedChannels = Globals::numChannels;

    using Ptr = ReferenceCountedObjectPtr<KeyboardMapping>;

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
    
    KeyChannel map(Note::Key key, int channel) const noexcept
    {
        assert(key < numMappedKeys);
        assert(channel > 0 && channel <= numMappedChannels);
        return this->index[key][channel - 1]; // expects channel in range [1..16]
    }

    void updateKey(int sourceKey, int sourceChannel, const KeyChannel &keyChannel);
    void updateKey(int sourceKey, int sourceChannel, int8 targetKey, int8 targetChannel);

    // this method will not reset before loading a mapping,
    // because there can be a number of files for multiple channels,
    // so don't forget to reset() before calling this
    void loadScalaKbmFile(InputStream &fileContentStream,
        const String &fileNameWithoutExtension);

    // returns e.g. "0:0/14,30+,0/15,30+,0/16,30+,0/1,30+"
    String toString() const;
    void loadMapFromString(const String &str);
    void loadMapFromPreset(KeyboardMapping *preset);

    const String &getName() const noexcept;
    void setName(const String &name);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // BaseResource
    //===------------------------------------------------------------------===//

    String getResourceId() const noexcept override;
    Identifier getResourceType() const noexcept override;

private:

    KeyChannel getDefaultMappingFor(int key, int channel) const noexcept;

    String name;
    KeyChannel index[numMappedKeys][numMappedChannels];

    JUCE_DECLARE_WEAK_REFERENCEABLE(KeyboardMapping)
};

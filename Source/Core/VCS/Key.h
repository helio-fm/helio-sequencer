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

// 128 * 32bit = 4096-bit key
// or 8 * 64-byte blocks
#define KEY_SIZE 128

namespace VCS
{
    class Key :
        public Serializable
    {
    public:

        Key()
        {
            MemoryOutputStream keyStream(this->key, false);

            Random r;

            for (int i = 0; i < KEY_SIZE; ++i)
            {
                r.setSeedRandomly();
                const bool writeOk = keyStream.writeInt(r.nextInt());
                jassert(writeOk);
            }
        }

        MemoryBlock getKeyData()
        {
            return this->key;
        }
        
        void restoreFromBase64(const String &encodedString)
        {
            this->key.fromBase64Encoding(encodedString);
        }


        //===------------------------------------------------------------------===//
        // Serializable
        //

        XmlElement *serialize() const override
        {
            auto xml = new XmlElement(Serialization::VCS::vcsHistoryKey);
            xml->setAttribute("Data", this->key.toBase64Encoding());
            return xml;
        }

        void deserialize(const XmlElement &xml) override
        {
            this->reset();

            const XmlElement *mainSlot = xml.hasTagName(Serialization::VCS::vcsHistoryKey) ?
                                         &xml : xml.getChildByName(Serialization::VCS::vcsHistoryKey);

            if (mainSlot == nullptr) { return; }

            const String key64Data(mainSlot->getStringAttribute("Data"));

            this->key.fromBase64Encoding(key64Data);
        }

        void reset() override
        {
            this->key.setSize(0);
        }

    private:

        MemoryBlock key;

    private:

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Key);

    };
}  // namespace VCS

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
#include "RevisionItem.h"
#include "Pack.h"

namespace VCS
{
    class Revision :
        public ValueTree,
        public Serializable
    {
    public:

        Revision();

        explicit Revision(Pack::Ptr pack, const String &name = String::empty);

        explicit Revision(ValueTree other);
        
        // стирает свои свойства, копирует новые, игнорируя пак (оставляет свой).
        void copyPropertiesFrom(const Revision &other);

        // стирает свои дельты, копирует новые.
        void copyDeltasFrom(const Revision &other);

        // setProperty, либо копирование дельт, если разные паки
        void copyProperty(Identifier id, const RevisionItem::Ptr itemToCopy);


        RevisionItem::Ptr getItemWithUuid(const Uuid &uuid);

        // moves items' data from memory to pack
        void flushData();

        Pack::Ptr getPackPtr() const;

        String getMessage() const;

        String getUuid() const;

        int64 getVersion() const;

        int64 getTimeStamp() const;

        void incrementVersion();

        MD5 calculateHash() const;

        bool isEmpty() const;


        //===------------------------------------------------------------------===//
        // Serializable
        //

        XmlElement *serialize() const override;

        void deserialize(const XmlElement &xml) override;

        void reset() override;

    private:

        void resetAllDeltas();

        JUCE_LEAK_DETECTOR(Revision);

    };
}  // namespace VCS

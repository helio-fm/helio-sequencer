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

namespace VCS
{
    struct PackDataHeader
    {
        Uuid itemId;
        Uuid deltaId;
        int64 startPosition;
        ssize_t numBytes;
    };

    struct PackDataBlock
    {
        Uuid itemId;
        Uuid deltaId;
        MemoryBlock data;
    };

    class Pack :
        public Serializable,
        public ReferenceCountedObject
    {
    public:

        Pack();

        ~Pack() override;

        void flush(); // пусть vcs вызывает после коммита всех изменений


        //===------------------------------------------------------------------===//
        // DeltaDataSource
        //
        
        bool containsDeltaDataFor(const Uuid &itemId,
                                  const Uuid &deltaId) const;
        
        XmlElement *createDeltaDataFor(const Uuid &itemId,
                                       const Uuid &deltaId) const;


        void setDeltaDataFor(const Uuid &itemId,
                                     const Uuid &deltaId,
                                     const XmlElement &data);


        //===------------------------------------------------------------------===//
        // Serializable
        //

        ValueTree serialize() const override;

        void deserialize(const ValueTree &tree) override;

        void reset() override;


        typedef ReferenceCountedObjectPtr<Pack> Ptr;

    protected:

        XmlElement *createXmlData(const PackDataHeader *header) const;

    private:

        // todo locks?

        OwnedArray<PackDataHeader> headers;

        OwnedArray<PackDataBlock> unsavedData;

        ScopedPointer<File> packFile;

        CriticalSection packStreamLock;

        ScopedPointer<FileInputStream> packStream;

        ScopedPointer<FileOutputStream> packWriteLocker;
        
        CriticalSection packLocker;

        Uuid uuid;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pack);

    };
} // namespace VCS

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

#include "Common.h"
#include "Pack.h"
#include "DocumentHelpers.h"
#include "SerializationKeys.h"

using namespace VCS;

// TODO rename as DeltaCache?

Pack::Pack()
{
    this->packFile = DocumentHelpers::getTempSlot("pack_" + this->uuid.toString() + ".vcs");
}

Pack::~Pack()
{
    this->packStream = nullptr;
    this->packWriteLocker = nullptr;
    this->packFile.deleteFile();
}

//===----------------------------------------------------------------------===//
// DeltaDataSource
//===----------------------------------------------------------------------===//

bool Pack::containsDeltaDataFor(const Uuid &itemId,
                                const Uuid &deltaId) const
{
    if (this->packStream != nullptr)
    {
        // on-disk data
        for (auto header : this->headers)
        {
            if (/*header->itemId == itemId &&*/
                header->deltaId == deltaId)
            {
                return true;
            }
        }
        
        // new in-memory data
        for (auto block : this->unsavedData)
        {
            if (/*block->itemId == itemId &&*/
                block->deltaId == deltaId)
            {
                return true;
            }
        }
    }
    
    return false;
}

ValueTree Pack::createDeltaDataFor(const Uuid &itemId, const Uuid &deltaId) const
{
    const ScopedLock lock(this->packLocker);
    
    if (this->packStream != nullptr)
    {
        // on-disk data
        for (auto header : this->headers)
        {
            if (/*header->itemId == itemId &&*/
                header->deltaId == deltaId)
            {
                return this->createSerializedData(header);
            }
        }

        // in-memory data
        for (auto chunk : this->unsavedData)
        {
            if (/*chunk->itemId == itemId &&*/
                chunk->deltaId == deltaId)
            {
                MemoryInputStream chunkDataStream(chunk->data, false);
                return ValueTree::readFromStream(chunkDataStream);
            }
        }
    }

    jassertfalse;
    return {};
}

void Pack::setDeltaDataFor(const Uuid &itemId, const Uuid &deltaId, const ValueTree &data)
{
    const ScopedLock lock(this->packLocker);

    auto chunk = new DeltaDataChunk();
    //chunk->itemId = itemId;
    chunk->deltaId = deltaId;

    MemoryOutputStream ms(chunk->data, false);
    data.writeToStream(ms);
    ms.flush();

    this->unsavedData.add(chunk);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree VCS::Pack::serialize() const
{
    const ScopedLock lock(this->packLocker);

    ValueTree tree(Serialization::VCS::pack);

    // save on-disk data
    if (this->packStream != nullptr)
    {
        for (auto header : this->headers)
        {
            const auto deltaData(this->createSerializedData(header));
            ValueTree packItem(Serialization::VCS::packItem);
            //packItem.setProperty(Serialization::VCS::packItemRevId, header->itemId.toString(), nullptr);
            packItem.setProperty(Serialization::VCS::packItemDeltaId, header->deltaId.toString(), nullptr);
            packItem.appendChild(deltaData, nullptr);
            tree.appendChild(packItem, nullptr);
        }
    }

    // and in-memory data
    for (auto chunk : this->unsavedData)
    {
        MemoryInputStream chunkDataStream(chunk->data, false);
        const auto deltaData(ValueTree::readFromStream(chunkDataStream));
        ValueTree packItem(Serialization::VCS::packItem);
        //packItem.setProperty(Serialization::VCS::packItemRevId, chunk->itemId.toString(), nullptr);
        packItem.setProperty(Serialization::VCS::packItemDeltaId, chunk->deltaId.toString(), nullptr);
        packItem.appendChild(deltaData, nullptr);
        tree.appendChild(packItem, nullptr);
    }

    return tree;
}

void VCS::Pack::deserialize(const ValueTree &tree)
{
    const ScopedLock lock(this->packLocker);

    this->reset();

    const auto root = tree.hasType(Serialization::VCS::pack) ?
        tree : tree.getChildWithName(Serialization::VCS::pack);

    if (!root.isValid()) { return; }

    forEachValueTreeChildWithType(root, e, Serialization::VCS::packItem)
    {
        // first, load the data
        auto block = new DeltaDataChunk();
        //block->itemId = e.getProperty(Serialization::VCS::packItemRevId);
        block->deltaId = e.getProperty(Serialization::VCS::packItemDeltaId);

        MemoryOutputStream ms(block->data, false);
        const auto firstChild(e.getChild(0));

        if (firstChild.isValid())
        {
            firstChild.writeToStream(ms);
        }

        ms.flush();
        this->unsavedData.add(block);
    }

    // then dump it on the disk
    this->flush();
}

void Pack::reset()
{
    const ScopedLock lock(this->packStreamLock);

    this->headers.clear();
    this->unsavedData.clear();
    this->packStream = nullptr;
    this->packWriteLocker = nullptr;
    this->packFile.deleteFile();
}


//===----------------------------------------------------------------------===//
// Protected
//===----------------------------------------------------------------------===//

void Pack::flush()
{
    // TODO: check for cache size and only flush it to the disk when it exceeds some limit
    /*
    const ScopedLock lock(this->packStreamLock);

    TemporaryFile tempFile(this->packFile);
    ScopedPointer<FileOutputStream> tempOutputStream(tempFile.getFile().createOutputStream());

    jassert(tempOutputStream->openedOk());

    this->packWriteLocker = nullptr;

    if (this->packStream != nullptr)
    {
        this->packStream->setPosition(0);
        tempOutputStream->writeFromInputStream(*this->packStream, -1);
        this->packStream = nullptr;
    }

    // add unsaved data and set up headers
    for (auto block : this->unsavedData)
    {
        const int64 position = tempOutputStream->getPosition();
        tempOutputStream->write(block->data.getData(), block->data.getSize());

        auto newHeader = new DeltaDataHeader();
        //newHeader->itemId = block->itemId;
        newHeader->deltaId = block->deltaId;
        newHeader->startPosition = position;
        newHeader->numBytes = block->data.getSize();

        this->headers.add(newHeader);
    }

    this->unsavedData.clear();

    tempOutputStream = nullptr;

    if (tempFile.overwriteTargetFileWithTemporary())
    {
        this->packStream = this->packFile.createInputStream();
        jassert(this->packStream->openedOk());

        this->packWriteLocker = this->packFile.createOutputStream();
        jassert(this->packWriteLocker->openedOk());
    }
    else
    {
        jassertfalse;
    }
    */
}

ValueTree Pack::createSerializedData(const DeltaDataHeader *header) const
{
    const ScopedLock lock(this->packStreamLock);
    this->packStream->setPosition(header->startPosition);
    return ValueTree::readFromStream(*this->packStream);
}

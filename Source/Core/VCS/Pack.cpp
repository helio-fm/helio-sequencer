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

#include "FileUtils.h"
#include "DataEncoder.h"
#include "SerializationKeys.h"

using namespace VCS;

#define VCS_PACK_DEBUGGING 0

//
// пак - это такая штука, где хранятся все тяжеловесные данные,
// которые можно достать по требованию, по id revisionItem + его дельты
// и все это - в самом файле проекта
//
// при загрузке\десериализации - скидываем все содержимое пака во временный файл.
// при добавлении данных - добавляем в память (+ метод flush(), который скидывает все на диск)
// при сохранении\сериализации - сохраняем еще несохраненное и записываем данные файла
//
// минусы: надо следить, чтоб файл не исчез (блокируем его открытыми потоками на чтение и запись)
// файл будет во временном каталоге
//
// в памяти держим только хэдер, где записано: пара id и смещения в файле.
//

Pack::Pack()
{
    // todo иногда пишет в корень диска c: ? wtf

    this->packFile =
        new File(FileUtils::getTempSlot("pack_" + this->uuid.toString() + ".vcs"));
}

Pack::~Pack()
{
    this->packStream = nullptr;
    this->packWriteLocker = nullptr;
    this->packFile->deleteFile();
}


//===----------------------------------------------------------------------===//
// DeltaDataSource
//===----------------------------------------------------------------------===//

bool Pack::containsDeltaDataFor(const Uuid &itemId,
                                const Uuid &deltaId) const
{
    if (this->packStream != nullptr)
    {
        // данные могут быть на диске
        for (auto header : this->headers)
        {
            if (header->itemId == itemId && header->deltaId == deltaId)
            {
                return true;
            }
        }
        
        // а могут быть и в памяти
        for (auto block : this->unsavedData)
        {
            if (block->itemId == itemId && block->deltaId == deltaId)
            {
                return true;
            }
        }
    }
    
    return false;
}

XmlElement *Pack::createDeltaDataFor(const Uuid &itemId,
                                     const Uuid &deltaId) const
{
    ScopedLock lock(this->packLocker);
    
    if (this->packStream != nullptr)
    {
        // данные могут быть на диске
        for (auto header : this->headers)
        {
            if (header->itemId == itemId && header->deltaId == deltaId)
            {
                return this->createXmlData(header);
            }
        }

        // а могут быть и в памяти
        for (auto block : this->unsavedData)
        {
            if (block->itemId == itemId && block->deltaId == deltaId)
            {
                return XmlDocument::parse(block->data.toString());
            }
        }
    }

    jassertfalse;
    return nullptr;
}

void Pack::setDeltaDataFor(const Uuid &itemId,
                           const Uuid &deltaId,
                           const XmlElement &data)
{
    ScopedLock lock(this->packLocker);

    auto block = new PackDataBlock();
    block->itemId = itemId;
    block->deltaId = deltaId;

    // несохраненные данные в памяти незачем обфусцировать
    MemoryOutputStream ms(block->data, false);
    data.writeToStream(ms, "", true, false);
    ms.flush();

    this->unsavedData.add(block);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Pack::serialize() const
{
    ScopedLock lock(this->packLocker);

    auto xml = new XmlElement(Serialization::VCS::pack);

    // скидываем временный файл
    if (this->packStream != nullptr)
    {
        for (auto header : this->headers)
        {
            XmlElement *deltaData = this->createXmlData(header);

            auto packItem = new XmlElement(Serialization::VCS::packItem);
            packItem->setAttribute(Serialization::VCS::packItemRevId, header->itemId.toString());
            packItem->setAttribute(Serialization::VCS::packItemDeltaId, header->deltaId.toString());
            packItem->addChildElement(deltaData);

            xml->prependChildElement(packItem);
        }
    }

    // и все новые данные
    for (auto block : this->unsavedData)
    {
        XmlElement *deltaData = XmlDocument::parse(block->data.toString());

        auto packItem = new XmlElement(Serialization::VCS::packItem);
        packItem->setAttribute(Serialization::VCS::packItemRevId, block->itemId.toString());
        packItem->setAttribute(Serialization::VCS::packItemDeltaId, block->deltaId.toString());
        packItem->addChildElement(deltaData);

        xml->prependChildElement(packItem);
    }

    return xml;
}

void Pack::deserialize(const XmlElement &xml)
{
    ScopedLock lock(this->packLocker);

    this->reset();

    const XmlElement *root = xml.hasTagName(Serialization::VCS::pack) ?
                             &xml : xml.getChildByName(Serialization::VCS::pack);

    if (root == nullptr) { return; }

    forEachXmlChildElementWithTagName(*root, e, Serialization::VCS::packItem)
    {
        // грузим все в память
        auto block = new PackDataBlock();
        block->itemId = e->getStringAttribute(Serialization::VCS::packItemRevId);
        block->deltaId = e->getStringAttribute(Serialization::VCS::packItemDeltaId);

        MemoryOutputStream ms(block->data, false);

        if (XmlElement *firstChild = e->getFirstChildElement())
        {
            firstChild->writeToStream(ms, "", true, false);
        }

        ms.flush();

        this->unsavedData.add(block);
    }

    // и сливаем на диск
    this->flush();
}

void Pack::reset()
{
    ScopedLock lock(this->packStreamLock);

    this->headers.clear();
    this->unsavedData.clear();
    this->packStream = nullptr;
    this->packWriteLocker = nullptr;
    this->packFile->deleteFile();
}


//===----------------------------------------------------------------------===//
// Protected
//===----------------------------------------------------------------------===//

void Pack::flush()
{
    ScopedLock lock(this->packStreamLock);

    TemporaryFile tempFile(*this->packFile);
    ScopedPointer<FileOutputStream> tempOutputStream(tempFile.getFile().createOutputStream());

    jassert(tempOutputStream->openedOk());

    this->packWriteLocker = nullptr;

    // если нужно - скопируем существующий файл,
    // и сразу закроем входной поток
    if (this->packStream != nullptr)
    {
        this->packStream->setPosition(0);
        tempOutputStream->writeFromInputStream(*this->packStream, -1);
        this->packStream = nullptr;
    }

    // добавляем unsavedData
    // достаточно обфусцировать их, дописать в конец файла
    // и добавить в свой список PackDataHeader'ы с получившимися смещениями
    for (auto block : this->unsavedData)
    {
        
#if VCS_PACK_DEBUGGING
        const String &obfuscated = block->data.toString();
#else
        const String &obfuscated = DataEncoder::obfuscateString(block->data.toString());
#endif

        const int64 position = tempOutputStream->getPosition();
        const ssize_t numBytes = obfuscated.getNumBytesAsUTF8();

        tempOutputStream->write(obfuscated.toRawUTF8(), numBytes);

        auto newHeader = new PackDataHeader();
        newHeader->itemId = block->itemId;
        newHeader->deltaId = block->deltaId;
        newHeader->startPosition = position;
        newHeader->numBytes = numBytes;

        this->headers.add(newHeader);
    }

    this->unsavedData.clear();

    tempOutputStream = nullptr;

    if (tempFile.overwriteTargetFileWithTemporary())
    {
        this->packStream = this->packFile->createInputStream();
        jassert(this->packStream->openedOk());

        this->packWriteLocker = this->packFile->createOutputStream();
        jassert(this->packWriteLocker->openedOk());
    }
    else
    {
        jassertfalse;
    }
}

XmlElement *Pack::createXmlData(const PackDataHeader *header) const
{
    ScopedLock lock(this->packStreamLock);
    MemoryBlock mb;
    this->packStream->setPosition(header->startPosition);
    this->packStream->readIntoMemoryBlock(mb, header->numBytes);

#if VCS_PACK_DEBUGGING
    const String &xmlData = mb.toString();
#else
    const String &xmlData = DataEncoder::deobfuscateString(mb.toString());
#endif

    return XmlDocument::parse(xmlData);
}

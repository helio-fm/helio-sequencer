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
#include "DocumentHelpers.h"

#include "JsonSerializer.h"
#include "XmlSerializer.h"
#include "BinarySerializer.h"
#include "LegacySerializer.h"

String DocumentHelpers::getTemporaryFolder()
{
    const File tempFolder(File::getSpecialLocation(File::tempDirectory).
        getFullPathName() + "/Helio");

    if (tempFolder.existsAsFile())
    { tempFolder.deleteFile(); }

    //DBG("DocumentHelpers::getTemporaryFolder :: " + tempFolder.getFullPathName());

    return tempFolder.getFullPathName();
}

static File getFirstSlot(String location1, String location2, const String &fileName)
{
    File result;
    bool slotExists = false;

#if HELIO_DESKTOP

    const String helioSubfolder = "Helio";
    File file1(File(location1).getChildFile(helioSubfolder));
    File file2(File(location2).getChildFile(helioSubfolder));

    // 1й вариант: какой-то из слотов существует - выбираем его.
    if (file1.getChildFile(fileName).existsAsFile())
    {
        slotExists = true;
        result = file1.getChildFile(fileName);
    }
    else if (file2.getChildFile(fileName).existsAsFile())
    {
        slotExists = true;
        result = file2.getChildFile(fileName);
    }
    else
    {
        // 2й вариант: не существует ни одного из файлов:
        // выбираем первый доступный для записи слот
        // если подпапок еще нет, пробуем создать
        if (! file1.isDirectory()) { file1.createDirectory(); }
        const File slot1(file1.getChildFile(fileName));
        if (slot1.create())
        {
            slot1.deleteFile(); // пустой файл раньше времени нам не нужен.
            result = slot1;
        }
        else
        {
            if (! file2.isDirectory()) { file2.createDirectory(); }
            const File slot2(file2.getChildFile(fileName));
            if (slot2.create())
            {
                slot2.deleteFile();
                result = slot2;
            }
        }
    }

#elif HELIO_MOBILE

    const File slot1(File(location1).getChildFile(fileName));
    const File slot2(File(location2).getChildFile(fileName));

    // 1й вариант: какой-то из слотов существует - выбираем его.
    if (slot1.existsAsFile())
    {
        slotExists = true;
        result = slot1;
    }
    else if (slot2.existsAsFile())
    {
        slotExists = true;
        result = slot2;
    }
    else
    {
        // 2й вариант: не существует ни одного из файлов:
        // выбираем первый доступный для записи слот
        if (slot1.create())
        {
            slot1.deleteFile(); // пустой файл раньше времени нам не нужен.
            result = slot1;
        }
        else if (slot2.create())
        {
            slot2.deleteFile();
            result = slot2;
        }
    }

#endif

    if (slotExists)
    {
        DBG("Opening file: " + result.getFullPathName());
    }

    return result;
}

File DocumentHelpers::getConfigSlot(const String &fileName)
{
    const auto location1(File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName());
    const auto location2(File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName());
    return getFirstSlot(location1, location2, fileName);
}

File DocumentHelpers::getDocumentSlot(const String &fileName)
{
    const auto location1(File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName());
    const auto location2(File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName());
    return getFirstSlot(location1, location2, fileName);
}

File DocumentHelpers::getTempSlot(const String &fileName)
{
    const auto tempPath(File::getSpecialLocation(File::tempDirectory).getFullPathName());
    return getFirstSlot(tempPath, tempPath, fileName);
}

static const OwnedArray<Serializer> &getSerializers()
{
    static OwnedArray<Serializer> serializers;
    if (serializers.isEmpty())
    {
        serializers.add(new XmlSerializer());
        serializers.add(new JsonSerializer());
        serializers.add(new BinarySerializer());
        serializers.add(new LegacySerializer());
    }

    return serializers;
}

static const Array<Serializer *> getSerializersForExtension(const String &extension)
{
    Array<Serializer *> result;

    for (const auto serializer : getSerializers())
    {
        if (serializer->supportsFileWithExtension(extension))
        {
            result.add(serializer);
        }
    }

    return result;
}

static const Array<Serializer *> getSerializersForHeader(const String &header)
{
    Array<Serializer *> result;

    for (const auto serializer : getSerializers())
    {
        if (serializer->supportsFileWithHeader(header))
        {
            result.add(serializer);
        }
    }

    return result;
}

ValueTree DocumentHelpers::load(const File &file)
{
    ValueTree result;

    if (!file.existsAsFile())
    {
        return result;
    }

    const String extension(file.getFileExtension());
    const auto onesThatSupportExtension(getSerializersForExtension(extension));

    // if exactly one serializer reports to support target file extension, then use that one
    if (onesThatSupportExtension.size() == 1)
    {
        ValueTree tree;
        onesThatSupportExtension.getFirst()->loadFromFile(file, tree);
        return tree;
    }

    // if none of more that one of serializers support that extension, try to check file header
    MemoryOutputStream header(8);
    {
        FileInputStream in(file);
        if (in.openedOk())
        {
            for (int i = 0; i < 8; ++i)
            {
                auto c = in.readByte();
                header.writeByte(c);
            }
        }
    }

    const auto onesThatSupportHeader(getSerializersForHeader(header.toUTF8()));
    if (!onesThatSupportHeader.isEmpty())
    {
        ValueTree tree;
        onesThatSupportHeader.getFirst()->loadFromFile(file, tree);
        return tree;
    }

    // Default to binary serialization
    return DocumentHelpers::load<BinarySerializer>(file);
}

ValueTree DocumentHelpers::load(const String &string)
{
    ValueTree result;
    const String header(string.substring(0, 8));

    const auto onesThatSupportHeader(getSerializersForHeader(header));
    if (!onesThatSupportHeader.isEmpty())
    {
        ValueTree tree;
        onesThatSupportHeader.getFirst()->loadFromString(string, tree);
        return tree;
    }

    // Default to XML serialization
    return DocumentHelpers::load<XmlSerializer>(string);

}

static File createTempFileForSaving(const File &parentDirectory, String name, const String& suffix)
{
    return parentDirectory.getNonexistentChildFile(name, suffix, false);
}

DocumentHelpers::TempDocument::TempDocument(const File &target) :
    temporaryFile(createTempFileForSaving(target.getParentDirectory(),
        target.getFileNameWithoutExtension() + "_temp_" + String::toHexString(Random::getSystemRandom().nextInt()),
        target.getFileExtension())),
    targetFile(target)
{
    jassert(targetFile != File());
}

DocumentHelpers::TempDocument::~TempDocument()
{
    if (!this->deleteTemporaryFile())
    {
        jassertfalse;
    }
}

const File &DocumentHelpers::TempDocument::getFile() const noexcept
{
    return temporaryFile;
}

const File &DocumentHelpers::TempDocument::getTargetFile() const noexcept
{
    return targetFile;
}

bool DocumentHelpers::TempDocument::overwriteTargetFileWithTemporary() const
{
    jassert(targetFile != File());

    if (temporaryFile.exists())
    {
        for (int i = 4; --i >= 0;)
        {
            if (temporaryFile.moveFileTo(targetFile))
            {
                return true;
            }

            Thread::sleep(100);
        }

        // здесь был баг - иногда под виндой moveFileTo не срабатывает, не знаю, почему
        for (int i = 2; --i >= 0;)
        {
            if (temporaryFile.copyFileTo(targetFile))
            {
                return true;
            }

            Thread::sleep(100);
        }
    }
    else
    {
        jassertfalse;
    }

    return false;
}

bool DocumentHelpers::TempDocument::deleteTemporaryFile() const
{
    for (int i = 5; --i >= 0;)
    {
        if (temporaryFile.deleteFile())
        {
            return true;
        }

        Thread::sleep(50);
    }

    return false;
}

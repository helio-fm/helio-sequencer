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

    //Logger::writeToLog("DocumentHelpers::getTemporaryFolder :: " + tempFolder.getFullPathName());

    return tempFolder.getFullPathName();
}

static File getFirstSlot(String location1, String location2, const String &fileName)
{
    File result;

#if HELIO_DESKTOP

    const String helioSubfolder = "Helio";
    File file1(File(location1).getChildFile(helioSubfolder));
    File file2(File(location2).getChildFile(helioSubfolder));

    // 1й вариант: какой-то из слотов существует - выбираем его.
    if (file1.getChildFile(fileName).existsAsFile())
    {
        result = file1.getChildFile(fileName);
    }
    else if (file2.getChildFile(fileName).existsAsFile())
    {
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
        result = slot1;
    }
    else if (slot2.existsAsFile())
    {
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

    Logger::writeToLog(fileName + " initialized at: " + result.getFullPathName());
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

ValueTree DocumentHelpers::load(const File &file)
{
    ValueTree result;

    if (!file.existsAsFile())
    {
        return result;
    }

    //const String extension(file.getFileExtension());

    //Array<Serializer> serializers;
    //Array<Serializer> foundSerializers;
    //Serializer *serializer = nullptr;

    //for (const auto &s : serializers)
    //{
    //    if (s.supportsFileWithExtension(extension))
    //    {
    //        foundSerializers.add(s);
    //    }
    //}

    //if (foundSerializers.size() == 1)
    //{
    //    foundSerializers.getFirst().loadFromFile(file, result);
    //    return result;
    //}

    //// TODO
    ////XmlSerializer xs;
    ////xs.supportsFileWithExtension(extension);

    //// first check an extension:
    //// if exactly one serializer reports to support it, then use that one
    //// if none of more that one of serializers support that extension,
    //// read file header and ask 

    //MemoryBlock headerBlock;

    //{
    //    FileInputStream in(file);
    //    headerBlock = in.openedOk() ? in.readIntoMemoryBlock(headerBlock, 16) : MemoryBlock();
    //}

    //const String h = headerBlock.toString();

    return DocumentHelpers::load<BinarySerializer>(file);
    //return result;
}

static File createTempFileForSaving(const File &parentDirectory, String name, const String& suffix)
{
    return parentDirectory.getNonexistentChildFile(name, suffix, false);
}

DocumentHelpers::TempDocument::TempDocument(const File &target, int optionFlags /*= 0*/) :
    temporaryFile(createTempFileForSaving(target.getParentDirectory(),
        target.getFileNameWithoutExtension()
        + "_temp_" + String::toHexString(Random::getSystemRandom().nextInt()),
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
        for (int i = 5; --i >= 0;)
        {
            if (temporaryFile.moveFileTo(targetFile))
            {
                return true;
            }
            // здесь был баг - иногда под виндой moveFileTo не срабатывает, не знаю, почему
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

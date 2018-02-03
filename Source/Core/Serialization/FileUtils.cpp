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
#include "FileUtils.h"


void FileUtils::fixCurrentWorkingDirectory()
{
    File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().setAsCurrentWorkingDirectory();
}

String FileUtils::getTemporaryFolder()
{
    const File tempFolder(File::getSpecialLocation(File::tempDirectory).
                          getFullPathName() + "/Helio");

    if (tempFolder.existsAsFile())
    { tempFolder.deleteFile(); }

    if (!tempFolder.exists())
    { tempFolder.createDirectory(); }

    //Logger::writeToLog("FileUtils::getTemporaryFolder :: " + tempFolder.getFullPathName());

    return tempFolder.getFullPathName();
}

static File getFirstSlot(String location1,
                         String location2,
                         const String &fileName)
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

File FileUtils::getConfigSlot(const String &fileName)
{
    auto location1 = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName();
    auto location2 = File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName();
    return getFirstSlot(location1, location2, fileName);
}

File FileUtils::getDocumentSlot(const String &fileName)
{
    const auto location1 = File::getSpecialLocation(File::userDocumentsDirectory).getFullPathName();
    const auto location2 = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName();
    return getFirstSlot(location1, location2, fileName);
}

File FileUtils::getTempSlot(const String &fileName)
{
    const auto tempPath = File::getSpecialLocation(File::tempDirectory).getFullPathName();
    return getFirstSlot(tempPath, tempPath, fileName);
}

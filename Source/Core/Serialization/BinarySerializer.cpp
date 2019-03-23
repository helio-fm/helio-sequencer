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
#include "BinarySerializer.h"

static const char *kHelioHeaderV2String = "Helio2::";
static const uint64 kHelioHeaderV2 = ByteOrder::littleEndianInt64(kHelioHeaderV2String);

Result BinarySerializer::saveToFile(File file, const ValueTree &tree) const
{
    FileOutputStream fileStream(file);
    if (fileStream.openedOk())
    {
        fileStream.setPosition(0);
        fileStream.truncate();
        fileStream.writeInt64(kHelioHeaderV2);
        tree.writeToStream(fileStream);
        return Result::ok();
    }

    return Result::fail("Failed to save");
}

Result BinarySerializer::loadFromFile(const File &file, ValueTree &tree) const
{
    FileInputStream fileStream(file);
    if (fileStream.openedOk())
    {
        const auto magicNumber = static_cast<uint64>(fileStream.readInt64());
        if (magicNumber == kHelioHeaderV2)
        {
            // without a buffered stream, loading speed sucks soo hard on Windows:
            BufferedInputStream bufferedStream(fileStream, 4096);
            tree = ValueTree::readFromStream(bufferedStream);
            return Result::ok();
        }
    }

    return Result::fail("Failed to load");
}

Result BinarySerializer::saveToString(String &string, const ValueTree &tree) const
{
    MemoryOutputStream memStream;
    memStream.writeInt64(kHelioHeaderV2);
    tree.writeToStream(memStream);
    string = memStream.toUTF8();
    return Result::ok();
}

Result BinarySerializer::loadFromString(const String &string, ValueTree &tree) const
{
    if (string.isNotEmpty())
    {
        tree = ValueTree::readFromData(string.toUTF8(), string.getNumBytesAsUTF8());
        return Result::ok();
    }

    return Result::fail("Failed to load");
}

bool BinarySerializer::supportsFileWithExtension(const String &extension) const
{
    return extension.endsWithIgnoreCase("hp") ||
        extension.endsWithIgnoreCase("helio");
}

bool BinarySerializer::supportsFileWithHeader(const String &header) const
{
    return header.startsWith(kHelioHeaderV2String);
}

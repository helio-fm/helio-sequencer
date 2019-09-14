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

Result BinarySerializer::saveToFile(File file, const SerializedData &tree) const
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

SerializedData BinarySerializer::loadFromFile(const File &file) const
{
    // here's the thing: reading from FileInputStream is slow asfuck (at least, on Windows);
    // adding BufferedInputStream bufferedStream(fileStream) - kinda helps, but:
    // ValueTree::readFromStream still calls getTotalLength() quite often, which
    // ends up calling File::getSize(), which, in turn, consumes a lot time.

    // so instead we'll just read the whole file into memory and deserialize from it;
    // somewhat ugly, but works, and saved files should never be really large anyway.
    MemoryBlock mb;
    if (file.loadFileAsData(mb))
    {
        MemoryInputStream inputStream(mb, false);
        const auto magicNumber = static_cast<uint64>(inputStream.readInt64());
        if (magicNumber == kHelioHeaderV2)
        {
            return SerializedData::readFromStream(inputStream);
        }
    }

    return {};
}

Result BinarySerializer::saveToString(String &string, const SerializedData &tree) const
{
    MemoryOutputStream memStream;
    memStream.writeInt64(kHelioHeaderV2);
    tree.writeToStream(memStream);
    string = memStream.toUTF8();
    return Result::ok();
}

SerializedData BinarySerializer::loadFromString(const String &string) const
{
    if (string.isNotEmpty())
    {
        return SerializedData::readFromData(string.toUTF8(), string.getNumBytesAsUTF8());
    }

    return {};
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

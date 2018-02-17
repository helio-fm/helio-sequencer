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

Result BinarySerializer::saveToFile(File file, const ValueTree &tree) const
{
    FileOutputStream os(file);
    if (os.openedOk())
    {
        os.setPosition(0);
        os.truncate();
        tree.writeToStream(os);
        return Result::ok();
    }

    return Result::fail("Failed to serialize");
}

Result BinarySerializer::loadFromFile(const File &file, ValueTree &tree) const
{
    return Result::fail("not implemented");
}

Result BinarySerializer::saveToString(String &string, const ValueTree &tree) const
{
    return Result::fail("not implemented");
}

Result BinarySerializer::loadFromString(const String &string, ValueTree &tree) const
{
    return Result::fail("not implemented");
}

bool BinarySerializer::supportsFileWithExtension(const String &extension) const
{
    return extension.endsWithIgnoreCase("helio");
}

bool BinarySerializer::supportsFileWithHeader(const String &header) const
{
    // TODO
    return false;
}

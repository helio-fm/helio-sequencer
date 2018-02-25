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
#include "JsonSerializer.h"

Result JsonSerializer::saveToFile(File file, const ValueTree &tree) const
{
    return Result::fail("not implemented");
}

Result JsonSerializer::loadFromFile(const File &file, ValueTree &tree) const
{
    return Result::fail("not implemented");
}

Result JsonSerializer::saveToString(String &string, const ValueTree &tree) const
{
    return Result::fail("not implemented");
}

Result JsonSerializer::loadFromString(const String &string, ValueTree &tree) const
{
    return Result::fail("not implemented");
    //var json;
    //Result result = JSON::parse(string, json);
    //if (result.wasOk())
    //{
    //    // TODO
    //    return result;
    //}
}

bool JsonSerializer::supportsFileWithExtension(const String &extension) const
{
    return extension.endsWithIgnoreCase("json");
}

bool JsonSerializer::supportsFileWithHeader(const String &header) const
{
    // Enough for all our cases:
    return header.startsWithChar('[') || header.startsWithChar('{');
}

/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class Serializable;
class SerializedData;

class Serializer
{
public:

    virtual ~Serializer() = default;

    virtual Result saveToFile(File file, const SerializedData &tree) const = 0;
    virtual SerializedData loadFromFile(const File &file) const = 0;

    virtual Result saveToString(String &string, const SerializedData &tree) const = 0;
    virtual SerializedData loadFromString(const String &string) const = 0;

    virtual bool supportsFileWithExtension(const String &extension) const = 0;
    virtual bool supportsFileWithHeader(const String &header) const = 0;

};

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

#pragma once

class Serializable;

class Serializer
{
public:

    virtual ~Serializer() {}

    virtual Result saveToFile(File file, const ValueTree &tree) const = 0;
    virtual Result loadFromFile(const File &file, ValueTree &tree) const = 0;

    virtual Result saveToString(String &string, const ValueTree &tree) const = 0;
    virtual Result loadFromString(const String &string, ValueTree &tree) const = 0;

    virtual bool supportsFileWithExtension(const String &extension) const = 0;
    virtual bool supportsFileWithHeader(const String &header) const = 0;

};

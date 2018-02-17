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

#include "Serializer.h"

class LegacySerializer final : public Serializer
{
public:

    Result saveToFile(File file, const ValueTree &tree) const override;
    Result loadFromFile(const File &file, ValueTree &tree) const override;

    Result saveToString(String &string, const ValueTree &tree) const override;
    Result loadFromString(const String &string, ValueTree &tree) const override;

    bool supportsFileWithExtension(const String &extension) const override;
    bool supportsFileWithHeader(const String &header) const override;

};

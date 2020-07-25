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
#include "Temperament.h"

Temperament::Temperament(const Temperament &other) noexcept
{
    jassertfalse;
}

Temperament::Temperament(const String &name) noexcept
{
    jassertfalse;
}

String Temperament::getResourceId() const noexcept
{
    jassertfalse;
    return {};
}

Identifier Temperament::getResourceType() const noexcept
{
    jassertfalse;
    return {};
}

SerializedData Temperament::serialize() const
{
    jassertfalse;
    return {};
}

void Temperament::deserialize(const SerializedData &data)
{
    jassertfalse;
}

void Temperament::reset()
{
    jassertfalse;
}

Temperament &Temperament::operator=(const Temperament &other)
{
    jassertfalse;
    return *this;
}

int Temperament::hashCode() const noexcept
{
    jassertfalse;
    return {};
}

bool operator==(const Temperament &l, const Temperament &r)
{
    jassertfalse;
    return {};
}

bool operator!=(const Temperament &l, const Temperament &r)
{
    jassertfalse;
    return {};
}

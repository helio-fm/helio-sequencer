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

#include "Common.h"
#include "Delta.h"

namespace VCS
{

static const String undefinedDelta = "undefined";

int64 DeltaDescription::defaultNumChanges = -1;

Delta *Delta::createCopy() const
{
    return new Delta(*this);
}

Delta::Delta(const Delta &other) :
    type(other.type),
    description(other.description) {}

String Delta::getHumanReadableText() const
{
    return this->description.getFullText();
}

DeltaDescription Delta::getDescription() const
{
    return this->description;
}

void Delta::setDescription(const DeltaDescription &newDescription)
{
    this->description = newDescription;
}

Identifier Delta::getType() const noexcept
{
    return this->type;
}

bool Delta::hasType(const Identifier &id) const noexcept
{
    return (this->type == id);
}

SerializedData Delta::serialize() const
{
    SerializedData tree(Serialization::VCS::delta);
    
    if (this->description.stringToTranslate.isNotEmpty())
    {
        tree.setProperty(Serialization::VCS::deltaName, this->description.stringToTranslate);
    }

    if (this->description.stringParameter.isNotEmpty())
    {
        tree.setProperty(Serialization::VCS::deltaStringParam, this->description.stringParameter);
    }

    if (this->description.intParameter != DeltaDescription::defaultNumChanges)
    {
        tree.setProperty(Serialization::VCS::deltaIntParam, String(this->description.intParameter));
    }

    return tree;
}

void Delta::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root = data.hasType(Serialization::VCS::delta) ?
        data : data.getChildWithName(Serialization::VCS::delta);

    if (!root.isValid()) { return; }

    if (root.getNumChildren() == 1)
    {
        this->type = root.getChild(0).getType();
    }
    else
    {
        this->type = root.getProperty(Serialization::VCS::deltaTypeId, undefinedDelta).toString();
    }

    const String descriptionName = root.getProperty(Serialization::VCS::deltaName, {});
    const String descriptionStringParam = root.getProperty(Serialization::VCS::deltaStringParam, {});
    const int64 descriptionIntParam = root.getProperty(Serialization::VCS::deltaIntParam, String(DeltaDescription::defaultNumChanges));
    
    this->description = { descriptionName, descriptionIntParam, descriptionStringParam };
}

void Delta::reset() {}

}

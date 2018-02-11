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
#include "Delta.h"

using namespace VCS;

static const String undefinedDelta = "undefined";

int64 DeltaDescription::defaultNumChanges = -1;

Delta::Delta(const Delta &other) :
    type(other.type),
    description(other.description),
    vcsUuid(other.vcsUuid)
{
}

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

Uuid Delta::getUuid() const
{
    return this->vcsUuid;
}

Identifier VCS::Delta::getType() const
{
    return this->type;
}

bool VCS::Delta::hasType(const Identifier &id) const
{
    return (this->type == id);
}

ValueTree VCS::Delta::serialize() const
{
    ValueTree tree(Serialization::VCS::delta);
    tree.setProperty(Serialization::VCS::deltaType, this->type.toString());
    tree.setProperty(Serialization::VCS::deltaName, this->description.stringToTranslate);
    tree.setProperty(Serialization::VCS::deltaStringParam, this->description.stringParameter);
    tree.setProperty(Serialization::VCS::deltaIntParam, String(this->description.intParameter));
    tree.setProperty(Serialization::VCS::deltaId, this->vcsUuid.toString());
    return tree;
}

void VCS::Delta::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::VCS::delta) ?
        tree : tree.getChildWithName(Serialization::VCS::delta);

    if (!root.isValid()) { return; }

    this->vcsUuid = root.getProperty(Serialization::VCS::deltaId, this->vcsUuid.toString());
    this->type = root.getProperty(Serialization::VCS::deltaType, undefinedDelta).toString();

    const String descriptionName = root.getProperty(Serialization::VCS::deltaName, String::empty);
    const String descriptionStringParam = root.getProperty(Serialization::VCS::deltaStringParam, String::empty);
    const int64 descriptionIntParam =
        root.getProperty(Serialization::VCS::deltaIntParam, String(DeltaDescription::defaultNumChanges));
    
    this->description = DeltaDescription(descriptionName, descriptionIntParam, descriptionStringParam);
}

void Delta::reset() {}

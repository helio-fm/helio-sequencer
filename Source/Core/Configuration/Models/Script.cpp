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
#include "Script.h"
#include "SerializationKeys.h"
#include "XmlSerializer.h"

Script &Script::operator=(const Script &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->name = other.name;
    this->type = other.type;
    //this->content.replaceAllContent(other.content.getAllContent());
    return *this;
}

bool operator==(const Script &l, const Script &r)
{
    return &l == &r || (l.name == r.name);
}

Script::Script(const String &name, const Identifier &type, const String &content)
{
    // TODO
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Script::getResourceId() const noexcept
{
    return this->name;
}

Identifier Script::getResourceType() const noexcept
{
    return Serialization::Resources::scripts;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Script::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Scripts::script);

    tree.setProperty(Scripts::name, this->name, nullptr);
    tree.setProperty(Scripts::type, this->type.toString(), nullptr);

    return tree;
}

void Script::deserialize(const ValueTree &tree)
{
    using namespace Serialization;

    const auto root = tree.hasType(Scripts::script) ?
        tree : tree.getChildWithName(Scripts::script);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Scripts::name);
    this->type = root.getProperty(Scripts::type).toString();
    //this->mapper = createMapperOfType(this->type);
}

void Script::reset()
{
    this->name.clear();
    // TODO
}

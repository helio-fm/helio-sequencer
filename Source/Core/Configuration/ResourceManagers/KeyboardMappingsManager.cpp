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
#include "KeyboardMappingsManager.h"
#include "SerializationKeys.h"
#include "Config.h"

KeyboardMappingsManager::KeyboardMappingsManager() :
    ResourceManager(Serialization::Resources::keyboardMappings) {}

void KeyboardMappingsManager::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::keyboardMappings) ?
        tree : tree.getChildWithName(Serialization::Resources::keyboardMappings);

    if (!root.isValid())
    {
        return;
    }

    forEachChildWithType(root, node, Serialization::Midi::KeyboardMappings::keyboardMapping)
    {
        KeyboardMapping::Ptr km(new KeyboardMapping());
        km->deserialize(node);
        outResources[km->getResourceId()] = km;
    }
}

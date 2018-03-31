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
#include "ScalesManager.h"
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "App.h"
#include "Config.h"

ScalesManager::ScalesManager() :
    ResourceManager(Serialization::Resources::scales) {}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ScalesManager::serialize() const
{
    ValueTree tree(Serialization::Midi::scales);
    
    Resources::Iterator i(this->resources);
    while (i.next())
    {
        tree.appendChild(i.getValue()->serialize(), nullptr);
    }
    
    return tree;
}

void ScalesManager::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Midi::scales) ?
        tree : tree.getChildWithName(Serialization::Midi::scales);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, scaleNode, Serialization::Midi::scale)
    {
        Scale::Ptr scale(new Scale());
        scale->deserialize(scaleNode);
        this->resources.set(scale->getResourceId(), scale);
    }

    // Logger::writeToLog("Number of scales available: " + String(this->resources.size()));
}

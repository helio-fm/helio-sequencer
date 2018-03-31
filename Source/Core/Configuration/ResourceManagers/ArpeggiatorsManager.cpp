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
#include "ArpeggiatorsManager.h"
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "App.h"
#include "Config.h"

ArpeggiatorsManager::ArpeggiatorsManager() :
    ResourceManager(Serialization::Resources::arpeggiators) {}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ArpeggiatorsManager::serialize() const
{
    ValueTree tree(Serialization::Arps::arpeggiators);

    Resources::Iterator i(this->resources);
    while (i.next())
    {
        tree.appendChild(i.getValue()->serialize(), nullptr);
    }
    
    return tree;
}

void ArpeggiatorsManager::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Arps::arpeggiators) ?
        tree : tree.getChildWithName(Serialization::Arps::arpeggiators);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, arpNode, Serialization::Arps::arpeggiator)
    {
        Arpeggiator::Ptr arp(new Arpeggiator());
        arp->deserialize(arpNode);
        this->resources.set(arp->getResourceId(), arp);
    }
}

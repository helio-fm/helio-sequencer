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
#include "ArpeggiatorsCollection.h"
#include "SerializationKeys.h"

ArpeggiatorsCollection::ArpeggiatorsCollection() :
    ConfigurationResourceCollection(Serialization::Resources::arpeggiators) {}

void ArpeggiatorsCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::arpeggiators) ?
        tree : tree.getChildWithName(Serialization::Resources::arpeggiators);

    if (!root.isValid()) { return; }

    forEachChildWithType(root, arpNode, Serialization::Arps::arpeggiator)
    {
        Arpeggiator::Ptr arp(new Arpeggiator());
        arp->deserialize(arpNode);
        outResources[arp->getResourceId()] = arp;
    }
}

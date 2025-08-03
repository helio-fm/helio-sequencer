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
#include "ScalesCollection.h"
#include "SerializationKeys.h"

ScalesCollection::ScalesCollection() :
    ConfigurationResourceCollection(Serialization::Resources::scales),
    scalesComparator(this->order) {}

ScalesCollection::ScalesComparator::ScalesComparator(const FlatHashMap<String, int, StringHash> &order) :
    order(order) {}

int ScalesCollection::ScalesComparator::compareElements(const ConfigurationResource::Ptr first,
    const ConfigurationResource::Ptr second) const
{
    const auto i1 = this->order.find(first->getResourceId());
    const auto i2 = this->order.find(second->getResourceId());

    if ((i2 == this->order.end()) || (i1 == this->order.end()))
    {
        //jassertfalse; // happens after adding a scale manually
        return (i2 != this->order.end()) - (i1 != this->order.end());
    }

    const auto diff = i1->second - i2->second;
    return (diff > 0) - (diff < 0);
}

const ConfigurationResource &ScalesCollection::getResourceComparator() const
{
    return this->scalesComparator;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData ScalesCollection::serializeResources(const Resources &resources)
{
    Array<typename Scale::Ptr> sortedResult;
    for (const auto &resource : resources)
    {
        sortedResult.addSorted(this->getResourceComparator(),
            typename Scale::Ptr(static_cast<Scale *>(resource.second.get())));
    }

    SerializedData data(Serialization::Resources::scales);
    for (const auto &resource : sortedResult)
    {
        data.appendChild(resource->serialize());
    }

    return data;
}

void ScalesCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    this->orderedScalesCache.clearQuick();

    const auto root = tree.hasType(Serialization::Resources::scales) ?
        tree : tree.getChildWithName(Serialization::Resources::scales);

    if (!root.isValid()) { return; }

    forEachChildWithType(root, scaleNode, Serialization::Midi::scale)
    {
        Scale::Ptr scale(new Scale());
        scale->deserialize(scaleNode);
        const auto scaleId = scale->getResourceId();
        outResources[scaleId] = scale;
        this->order[scaleId] = int(this->order.size());
    }
}

void ScalesCollection::reset()
{
    this->order.clear();
    this->orderedScalesCache.clearQuick();
    ConfigurationResourceCollection::reset();
}

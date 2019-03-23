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
#include "Config.h"

ScalesManager::ScalesManager() :
    ResourceManager(Serialization::Resources::scales),
    scalesComparator(this->order) {}

ScalesManager::ScalesComparator::ScalesComparator(const StringArray &order) :
    order(order) {}

// Any other scales, including user's, are displayed below and sorted alphabetically:
int ScalesManager::ScalesComparator::compareElements(const BaseResource::Ptr first,
    const BaseResource::Ptr second) const
{
    const int i1 = this->order.indexOf(first->getResourceId());
    const int i2 = this->order.indexOf(second->getResourceId());

    const int mixedDiff = (i2 != -1) - (i1 != -1);
    if (mixedDiff != 0) { return mixedDiff; }

    const int indexDiff = ((i1 - i2) > 0) - ((i1 - i2) < 0);
    if (indexDiff != 0) { return indexDiff; }

    return first->getResourceId().compare(second->getResourceId());
}

const BaseResource &ScalesManager::getResourceComparator() const
{
    return this->scalesComparator;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

// The comparator just makes sure that some most common scale names
// have priorities over others, and such scales, if present,
// should be displayed at the top of the list with the following order.
#define NUM_ORDERED_SCALES 17

void ScalesManager::deserializeResources(const ValueTree &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::scales) ?
        tree : tree.getChildWithName(Serialization::Resources::scales);

    if (!root.isValid()) { return; }

    forEachValueTreeChildWithType(root, scaleNode, Serialization::Midi::scale)
    {
        Scale::Ptr scale(new Scale());
        scale->deserialize(scaleNode);
        outResources[scale->getResourceId()] = scale;

        if (this->order.size() < NUM_ORDERED_SCALES)
        {
            this->order.add(scale->getResourceId());
        }
    }
}

void ScalesManager::reset()
{
    this->order.clearQuick();
    ResourceManager::reset();
}

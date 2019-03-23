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
#include "ChordsManager.h"
#include "SerializationKeys.h"

ChordsManager::ChordsManager() :
    ResourceManager(Serialization::Resources::chords),
    chordsComparator(this->order) {}

ChordsManager::ChordsComparator::ChordsComparator(const StringArray &order) :
    order(order) {}

int ChordsManager::ChordsComparator::compareElements(const BaseResource::Ptr first,
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

const BaseResource &ChordsManager::getResourceComparator() const
{
    return this->chordsComparator;
}

void ChordsManager::deserializeResources(const ValueTree &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::chords) ?
        tree : tree.getChildWithName(Serialization::Resources::chords);

    if (!root.isValid()) { return; }

    forEachValueTreeChildWithType(root, chordNode, Serialization::Midi::chord)
    {
        Chord::Ptr chord(new Chord());
        chord->deserialize(chordNode);
        outResources[chord->getResourceId()] = chord;
        this->order.add(chord->getResourceId());
    }
}

void ChordsManager::reset()
{
    this->order.clearQuick();
    ResourceManager::reset();
}

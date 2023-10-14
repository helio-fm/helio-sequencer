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
#include "MetersCollection.h"
#include "SerializationKeys.h"

MetersCollection::MetersCollection() :
    ConfigurationResourceCollection(Serialization::Resources::meters) {}

void MetersCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::meters) ?
        tree : tree.getChildWithName(Serialization::Resources::meters);

    if (!root.isValid())
    {
        return;
    }

    forEachChildWithType(root, node, Serialization::Midi::meter)
    {
        Meter::Ptr meter(new Meter());
        meter->deserialize(node);
        outResources[meter->getResourceId()] = meter;
    }
}

int MetersCollection::MetersComparator::compareElements(const ConfigurationResource::Ptr first, const ConfigurationResource::Ptr second) const
{
    const auto *l = static_cast<Meter *>(first.get());
    const auto *r = static_cast<Meter *>(second.get());

    // common time always goes first:
    const int commonTimeComparison = r->isCommonTime() - l->isCommonTime();
    if (commonTimeComparison != 0) { return commonTimeComparison; }

    const int numeratorDiff = l->getNumerator() - r->getNumerator();
    const int numeratorComparison = (numeratorDiff > 0) - (numeratorDiff < 0);
    if (numeratorComparison != 0) { return numeratorComparison; }

    const int denominatorDiff = l->getDenominator() - r->getDenominator();
    const int denominatorComparison = (denominatorDiff > 0) - (denominatorDiff < 0);
    if (denominatorComparison != 0) { return denominatorComparison; }

    return first->getResourceId().compare(second->getResourceId());
}

const ConfigurationResource &MetersCollection::getResourceComparator() const
{
    return this->metersComparator;
}

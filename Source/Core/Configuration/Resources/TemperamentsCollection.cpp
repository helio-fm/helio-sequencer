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
#include "SerializationKeys.h"
#include "TemperamentsCollection.h"

TemperamentsCollection::TemperamentsCollection() :
    ConfigurationResourceCollection(Serialization::Resources::temperaments) {}

void TemperamentsCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::temperaments) ?
        tree : tree.getChildWithName(Serialization::Resources::temperaments);

    if (!root.isValid()) { return; }

    forEachChildWithType(root, temperamentNode, Serialization::Midi::temperament)
    {
        Temperament::Ptr temperament(new Temperament());
        temperament->deserialize(temperamentNode);
        outResources[temperament->getResourceId()] = temperament;
    }
}

const Scale::Ptr TemperamentsCollection::findHighlightingFor(Temperament::Ptr temperament) const
{
    const auto ownHighlighting = temperament->getHighlighting();

    if (ownHighlighting != nullptr && ownHighlighting->isValid())
    {
        return ownHighlighting;
    }

    jassertfalse;

    // play safe: if the temperament doesn't describe highlighting
    // for whatever reason, try to find it among other resources
    for (const auto &t : this->getAll())
    {
        if (t->getResourceId() == temperament->getResourceId())
        {
            const auto otherHighlighting = t->getHighlighting();
            if (otherHighlighting != nullptr && otherHighlighting->isValid())
            {
                return otherHighlighting;
            }
        }
    }

    // and fallback to 12-tone major
    return Scale::makeNaturalMajorScale();
}

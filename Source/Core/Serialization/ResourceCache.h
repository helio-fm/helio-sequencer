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

#pragma once

#include "DocumentHelpers.h"

template<typename T>
class ResourceCache final
{
public:

    ResourceCache() {}

    static ResourceCache<T> &getInstance()
    {
        static ResourceCache<T> Instance;
        return Instance;
    }

    Array<T> get(const Identifier &tag, const char *const resourceName)
    {
        if (!this->cache.isEmpty())
        {
            return this->cache;
        }

        int numBytes;
        const String resourceDocument =
            BinaryData::getNamedResource(resourceName, numBytes);

        const ValueTree tree(DocumentHelpers::load(resourceDocument));
        if (!tree.isValid())
        {
            return this->cache;
        }

        forEachValueTreeChildWithType(tree, resourceRoot, tag)
        {
            T t;
            t.deserialize(resourceRoot);
            this->cache.add(t);
        }

        return this->cache;
    }

private:

    Array<T> cache;

    JUCE_DECLARE_NON_COPYABLE(ResourceCache<T>)
};

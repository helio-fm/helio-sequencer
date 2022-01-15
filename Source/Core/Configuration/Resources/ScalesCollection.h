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

#include "Scale.h"
#include "ConfigurationResourceCollection.h"

class ScalesCollection final : public ConfigurationResourceCollection
{
public:
  
    ScalesCollection();
    
    ConfigurationResource::Ptr createResource() const override
    {
        return { new Scale() };
    }

    inline const Array<Scale::Ptr> getAll() const
    {
        return this->getAllResources<Scale>();
    }

private:

    void deserializeResources(const SerializedData &tree, Resources &outResources) override;
    void reset() override;

    struct ScalesComparator final : public DummyConfigurationResource
    {
        ScalesComparator(const StringArray &order);
        int compareElements(const ConfigurationResource::Ptr first,
            const ConfigurationResource::Ptr second) const override;
        const StringArray &order;
    };

    ScalesComparator scalesComparator;
    const ConfigurationResource &getResourceComparator() const override;
    StringArray order;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalesCollection)
};

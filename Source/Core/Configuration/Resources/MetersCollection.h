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

#pragma once

#include "Meter.h"
#include "ConfigurationResourceCollection.h"

class MetersCollection final : public ConfigurationResourceCollection
{
public:
  
    MetersCollection();
    
    ConfigurationResource::Ptr createResource() const override
    {
        return { new Meter() };
    }

    inline const Array<Meter::Ptr> getAll() const
    {
        return this->getAllResources<Meter>();
    }

private:

    void deserializeResources(const SerializedData &tree, Resources &outResources) override;

    struct MetersComparator final : public DummyConfigurationResource
    {
        int compareElements(const ConfigurationResource::Ptr first,
            const ConfigurationResource::Ptr second) const override;
    };

    MetersComparator metersComparator;
    const ConfigurationResource &getResourceComparator() const override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetersCollection)
};

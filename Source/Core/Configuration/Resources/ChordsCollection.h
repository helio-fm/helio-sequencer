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

#include "Chord.h"
#include "ConfigurationResourceCollection.h"

class ChordsCollection final : public ConfigurationResourceCollection
{
public:

    ChordsCollection();

    ConfigurationResource::Ptr createResource() const override
    {
        return { new Chord() };
    }

    inline const Array<Chord::Ptr> getAll() const
    {
        return this->getAllResources<Chord>();
    }

private:

    void deserializeResources(const SerializedData &tree, Resources &outResources) override;
    void reset() override;

    struct ChordsComparator final : public DummyConfigurationResource
    {
        ChordsComparator(const StringArray &order);
        int compareElements(const ConfigurationResource::Ptr first,
            const ConfigurationResource::Ptr second) const override;
        const StringArray &order;
    };

    ChordsComparator chordsComparator;
    const ConfigurationResource &getResourceComparator() const override;
    StringArray order;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordsCollection)
};

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

#include "BaseResource.h"
#include "DocumentHelpers.h"
#include "BinarySerializer.h"
#include "JsonSerializer.h"
#include "XmlSerializer.h"

// A thing to note: all subclasses should make their
// deserialization method able to append overridden changes.
// And they should not reset in a while - base reloadResources() will manage proper resetting.

// TODO: monitor user's file changes!

class ResourceManager : public Serializable, public ChangeBroadcaster
{
public:

    ResourceManager(const Identifier &resourceName);

    virtual void initialise();
    virtual void shutdown();

    template<typename T>
    const Array<T> getResources() const
    {
        Array<T> result;
        Resources::Iterator i(this->resources);
        while (i.next())
        {
            result.addSorted(this->comparator, static_cast<T>(i.getValue()));
        }

        return result;
    }

    void updateBaseResource(const ValueTree &resource);
    void updateUserResource(const BaseResource::Ptr resource);

protected:

    void reset() override;

    void reloadResources();

    // Loads base and user's config.
    // Assumes that sub-classes won't reset on deserialization.
    virtual void loadBaseResource(const ValueTree &tree);
    virtual void loadUserResource(const ValueTree &tree);

    virtual File getDownloadedResourceFile() const;
    virtual File getUsersResourceFile() const;
    virtual String getBuiltInResourceString() const;

    typedef HashMap<String, BaseResource::Ptr> Resources;
    Resources resources;

private: 

    void saveUpdatedUserResource(const ValueTree &resource);

    const Identifier resourceName;

    struct BaseResourceComparator final : public BaseResource
    {
        String getResourceId() const override { return {}; }
        ValueTree serialize() const override { return {}; }
        void deserialize(const ValueTree &tree) override {}
        void reset() override {}
    };

    BaseResourceComparator comparator;

};

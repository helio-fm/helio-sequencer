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
    const Array<typename T::Ptr> getResources(bool sorted = true) const
    {
        Array<T::Ptr> result;
        Resources::Iterator i(this->resources);
        while (i.next())
        {
            if (sorted)
            {
                result.addSorted(this->comparator, T::Ptr(static_cast<T *>(i.getValue().get())));
            }
            else
            {
                result.add(T::Ptr(static_cast<T *>(i.getValue().get())));
            }
        }

        return result;
    }

    template<typename T>
    const T getResourceById(const String &resourceId) const
    {
        return static_cast<T>(this->resources[resourceId]);
    }

    const int size() const noexcept
    {
        return this->resources.size();
    }

    void updateBaseResource(const ValueTree &resource);
    void updateUserResource(const BaseResource::Ptr resource);

protected:

    void reset() override;

    void reloadResources();

    virtual File getDownloadedResourceFile() const;
    virtual File getUsersResourceFile() const;
    virtual String getBuiltInResourceString() const;

    using Resources = HashMap<String, BaseResource::Ptr>;
    Resources resources;
    
private: 

    const Identifier resourceName;

    struct BaseResourceComparator final : public BaseResource
    {
        String getResourceId() const override { return {}; }
        Identifier getResourceIdProperty() const override { return {}; }
        ValueTree serialize() const override { return {}; }
        void deserialize(const ValueTree &tree) override {}
        void reset() override {}
    };

    // Just keep user's data so that we are able to append
    // more to it and re-save back:
    ValueTree userResources;

    BaseResourceComparator comparator;

};

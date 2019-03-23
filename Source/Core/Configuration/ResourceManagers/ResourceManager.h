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

class ResourceManager : public ChangeBroadcaster
{
public:

    explicit ResourceManager(const Identifier &resourceType);
    ~ResourceManager() override;

    void reloadResources();

    template<typename T = BaseResource>
    const Array<typename T::Ptr> getAllResources() const
    {
        Array<typename T::Ptr> result;

        for (const auto &baseConfig : this->baseResources)
        {
            if (!this->userResources.contains(baseConfig.first))
            {
                result.addSorted(this->getResourceComparator(),
                    typename T::Ptr(static_cast<T *>(baseConfig.second.get())));
            }
        }

        for (const auto &userConfig : this->userResources)
        {
            result.addSorted(this->getResourceComparator(),
                typename T::Ptr(static_cast<T *>(userConfig.second.get())));
        }

        return result;
    }

    template<typename T = BaseResource>
    const Array<typename T::Ptr> getUserResources() const
    {
        Array<typename T::Ptr> result;

        for (const auto &userConfig : this->userResources)
        {
            result.addSorted(this->getResourceComparator(),
                typename T::Ptr(static_cast<T *>(userConfig.second.get())));
        }

        return result;
    }

    template<typename T = BaseResource>
    const typename T::Ptr getResourceById(const String &resourceId) const
    {
        const auto foundUserResource = this->userResources.find(resourceId);
        if (foundUserResource != this->userResources.end())
        {
            return typename T::Ptr(static_cast<T *>(foundUserResource->second.get()));
        }

        const auto foundBaseResource = this->baseResources.find(resourceId);
        if (foundBaseResource != this->baseResources.end())
        {
            return typename T::Ptr(static_cast<T *>(foundBaseResource->second.get()));
        }

        return nullptr;
    }

    template<typename T = BaseResource>
    const typename T::Ptr getUserResourceById(const String &resourceId) const
    {
        const auto foundUserResource = this->userResources.find(resourceId);
        if (foundUserResource != this->userResources.end())
        {
            return typename T::Ptr(static_cast<T *>(foundUserResource->second.get()));
        }
        
        return nullptr;
    }

    template<typename T = BaseResource>
    const bool containsUserResourceWithId(const String &resourceId) const
    {
        const auto foundUserResource = this->userResources.find(resourceId);
        return foundUserResource != this->userResources.end();
    }

    virtual BaseResource::Ptr createResource() const = 0;

    void updateBaseResource(const ValueTree &resource);
    void updateUserResource(const BaseResource::Ptr resource);

protected:

    virtual File getDownloadedResourceFile() const;
    virtual File getUsersResourceFile() const;
    virtual String getBuiltInResourceString() const;
    virtual const BaseResource &getResourceComparator() const;

    using Resources = FlatHashMap<String, BaseResource::Ptr, StringHash>;
    Resources baseResources;
    Resources userResources;

    // customized Serializable:
    virtual ValueTree serializeResources(const Resources &resources);
    virtual void deserializeResources(const ValueTree &tree,
        Resources &outResources) = 0;
    virtual void reset();

private: 

    const Identifier resourceType;
    const DummyBaseResource comparator;

    JUCE_DECLARE_WEAK_REFERENCEABLE(ResourceManager)
};

using ResourceManagerLookup = FlatHashMap<Identifier, WeakReference<ResourceManager>, IdentifierHash>;

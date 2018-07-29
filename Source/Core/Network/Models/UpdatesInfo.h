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

#include "ApiModel.h"

class UpdatesInfo final : public ApiModel
{
public:

    UpdatesInfo() : ApiModel({}) {}
    UpdatesInfo(const ValueTree &tree) : ApiModel(tree) {}

    struct VersionInfo final
    {
        Identifier getPlatformId() const noexcept
        { return Identifier(this->data.getProperty(Serialization::Api::V1::Resources::platformId)); }

        String getVersion() const noexcept
        { return this->data.getProperty(Serialization::Api::V1::Resources::version); }

        String getLink() const noexcept
        { return this->data.getProperty(Serialization::Api::V1::Resources::link); }

        ValueTree data;
    };

    struct ResourceInfo final
    {
        String getName() const noexcept
        { return this->data.getProperty(Serialization::Api::V1::Resources::resourceName); }

        String getHash() const noexcept
        { return this->data.getProperty(Serialization::Api::V1::Resources::hash); }

        ValueTree data;
    };

    Array<VersionInfo> getVersions() const
    { return this->getChildren<VersionInfo>(Serialization::Api::V1::Resources::versionInfo); }

    Array<ResourceInfo> getResources() const
    { return this->getChildren<ResourceInfo>(Serialization::Api::V1::Resources::resourceInfo); }

    // True if caches differ for the same resource,
    // or if new resource is not even listed here
    bool seemsOutdatedFor(const ResourceInfo &newResource) const
    {
        bool hasInfoForNewResource = false;
        forEachValueTreeChildWithType(this->data, resourceData, Serialization::Api::V1::Resources::resourceInfo)
        {
            const ResourceInfo oldResource({ resourceData });
            if (oldResource.getName() == newResource.getName())
            {
                hasInfoForNewResource = true;
                if (oldResource.getHash() != newResource.getHash())
                {
                    return true;
                }
            }
        }

        return !hasInfoForNewResource;
    }

    JUCE_LEAK_DETECTOR(UpdatesInfo)
};

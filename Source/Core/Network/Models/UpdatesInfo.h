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

struct UpdatesInfo final : ApiModel
{
    UpdatesInfo() : ApiModel({}) {}
    UpdatesInfo(const ValueTree &tree) : ApiModel(tree) {}

    struct VersionInfo final
    {
        String getLink() const noexcept { return API_MODEL_DATA(Resources::link); }
        String getVersion() const noexcept { return API_MODEL_DATA(Resources::version); }
        Identifier getPlatformType() const noexcept { return API_MODEL_DATA(Resources::platformId); }
        ValueTree data;
    };

    struct ResourceInfo final
    {
        String getType() const noexcept { return API_MODEL_DATA(Resources::resourceType); }
        String getHash() const noexcept { return API_MODEL_DATA(Resources::hash); }
        ValueTree data;
    };

    Array<VersionInfo> getVersions() const { return API_MODEL_CHILDREN(VersionInfo, Resources::versionInfo); }
    Array<ResourceInfo> getResources() const { return API_MODEL_CHILDREN(ResourceInfo, Resources::resourceInfo); }

    // True if caches differ for the same resource,
    // or if new resource is not even listed here
    bool seemsOutdatedFor(const ResourceInfo &newResource) const
    {
        bool hasInfoForNewResource = false;
        forEachValueTreeChildWithType(this->data, resourceData, Serialization::Api::V1::Resources::resourceInfo)
        {
            const ResourceInfo oldResource({ resourceData });
            if (oldResource.getType() == newResource.getType())
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

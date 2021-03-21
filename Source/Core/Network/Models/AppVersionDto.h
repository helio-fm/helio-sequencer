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

#if !NO_NETWORK

#include "ApiModel.h"

class AppVersionDto final : public ApiModel
{
public:

    AppVersionDto(const SerializedData &tree) noexcept : ApiModel(tree) {}

    String getArchitecture() const noexcept { return DTO_PROPERTY(Versions::architecture); }
    String getBranch() const noexcept { return DTO_PROPERTY(Versions::branch); }
    String getBuildType() const noexcept { return DTO_PROPERTY(Versions::buildType); }
    String getLink() const noexcept { return DTO_PROPERTY(Versions::link); }
    String getVersion() const noexcept { return DTO_PROPERTY(Versions::version); }
    String getPlatformType() const noexcept { return DTO_PROPERTY(Versions::platformType); }

    bool isDevelopmentBuild() const noexcept
    {
        return this->getBranch().startsWithIgnoreCase("dev");
    }

    String getHumanReadableDescription() const noexcept
    {
        return (this->isDevelopmentBuild() ? "" : this->getVersion() + " ") +
            this->getBranch() + ", " + this->getBuildType() +
            (this->getArchitecture().startsWithIgnoreCase("all") ? "" : ", " + this->getArchitecture());
    }

    Array<int> getVersionComponents(const String &versionString) const
    {
        Array<int> result;
        StringArray components;
        components.addTokens(versionString, ".", "");
        for (const auto &c : components)
        {
            result.add(c.getIntValue());
        }
        while (result.size() < 3)
        {
            result.add(0);
        }
        return result;
    }

    bool isLaterThanCurrentVersion() const
    {
        if (this->isDevelopmentBuild())
        {
            return true;
        }
        
        static const auto v0 = this->getVersionComponents(ProjectInfo::versionString);
        const auto v1 = this->getVersionComponents(this->getVersion());

        return (v1[0] > v0[0]) || (v1[0] == v0[0] && v1[1] > v0[1]) ||
            (v1[0] == v0[0] && v1[1] == v0[1] && v1[2] > v0[2]);
    }

    JUCE_LEAK_DETECTOR(AppVersionDto)
};

#endif

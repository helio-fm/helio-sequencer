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

struct AppVersionDto final : ApiModel
{
    AppVersionDto(const ValueTree &tree) : ApiModel(tree) {}

    String getArchitecture() const noexcept { return DTO_PROPERTY(Versions::architecture); }
    String getBranch() const noexcept { return DTO_PROPERTY(Versions::branch); }
    String getBuildType() const noexcept { return DTO_PROPERTY(Versions::buildType); }
    String getLink() const noexcept { return DTO_PROPERTY(Versions::link); }
    String getVersion() const noexcept { return DTO_PROPERTY(Versions::version); }
    String getPlatformType() const noexcept { return DTO_PROPERTY(Versions::platformType); }

    String getHumanReadableDescription() const noexcept
    {
        return (this->getBranch().startsWithIgnoreCase("dev") ? "" : this->getVersion() + " ") +
            this->getBranch() + ", " + this->getBuildType() +
            (this->getArchitecture().startsWithIgnoreCase("all") ? "" : ", " + this->getArchitecture());
    }

    Array<int> getVersionComponents() const
    {
        Array<int> result;
        StringArray components;
        components.addTokens(this->getVersion(), ".", "");
        for (const auto &c : components)
        {
            result.add(c.getIntValue());
        }
        return result;
    }

    JUCE_LEAK_DETECTOR(AppVersionDto)
};

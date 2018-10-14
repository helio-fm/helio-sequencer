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

struct UserSessionDto final : ApiModel
{
    UserSessionDto(const ValueTree &tree) : ApiModel(tree) {}

    String getDeviceId() const noexcept { return DTO_PROPERTY(Sessions::deviceId); }
    String getPlatformId() const noexcept { return DTO_PROPERTY(Sessions::platformId); }
    int64 getCreateTime() const noexcept { return DTO_PROPERTY(Sessions::createdAt); }
    int64 getUpdateTime() const noexcept { return DTO_PROPERTY(Sessions::updatedAt); }

    JUCE_LEAK_DETECTOR(UserSessionDto)
};

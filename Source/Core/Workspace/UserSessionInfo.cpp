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

#include "Common.h"
#include "UserSessionInfo.h"

UserSessionInfo::UserSessionInfo(const UserSessionDto &dto) :
    deviceId(dto.getDeviceId()),
    platformId(dto.getPlatformId()),
    createdAt(dto.getCreateTime()),
    updatedAt(dto.getUpdateTime()) {}

void UserSessionInfo::updateRemoteInfo(const UserSessionDto &dto)
{
    jassert(this->deviceId == dto.getDeviceId());
    jassert(this->platformId == dto.getPlatformId());
    this->createdAt = Time(dto.getCreateTime());
    this->updatedAt = Time(dto.getUpdateTime());
}

String UserSessionInfo::getDeviceId() const noexcept
{
    return this->deviceId;
}

String UserSessionInfo::getPlatformId() const noexcept
{
    return this->platformId;
}

Time UserSessionInfo::getCreateTime() const noexcept
{
    return this->createdAt;
}

Time UserSessionInfo::getUpdateTime() const noexcept
{
    return this->updatedAt;
}

int UserSessionInfo::compareElements(UserSessionInfo *first, UserSessionInfo *second)
{
    jassert(first != nullptr && second != nullptr);
    if (first == second || first->deviceId == second->deviceId)
    {
        return 0;
    }

    return int(first->updatedAt.toMilliseconds() - second->updatedAt.toMilliseconds());
}

ValueTree UserSessionInfo::serialize() const
{
    using namespace Serialization::User;
    ValueTree root(Sessions::session);

    root.setProperty(Sessions::deviceId, this->deviceId, nullptr);
    root.setProperty(Sessions::platformId, this->platformId, nullptr);
    root.setProperty(Sessions::createdAt, this->createdAt.toMilliseconds(), nullptr);
    root.setProperty(Sessions::updatedAt, this->updatedAt.toMilliseconds(), nullptr);

    return root;
}

void UserSessionInfo::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization::User;

    const auto root = tree.hasType(Sessions::session) ?
        tree : tree.getChildWithName(Sessions::session);

    if (!root.isValid()) { return; }

    this->deviceId = root.getProperty(Sessions::deviceId);
    this->platformId = root.getProperty(Sessions::platformId);
    this->createdAt = Time(root.getProperty(Sessions::createdAt));
    this->updatedAt = Time(root.getProperty(Sessions::updatedAt));
}

void UserSessionInfo::reset() {}

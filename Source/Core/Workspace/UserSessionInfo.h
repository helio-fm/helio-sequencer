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

#include "Serializable.h"
#include "UserSessionDto.h"

class UserSessionInfo final : public Serializable,
                              public ReferenceCountedObject
{
public:

    UserSessionInfo(const UserSessionDto &dto) :
        deviceId(dto.getDeviceId()),
        platformId(dto.getPlatformId()),
        createdAt(dto.getCreateTime()),
        updatedAt(dto.getUpdateTime()) {}

    String getDeviceId() const noexcept
    {
        return this->deviceId;
    }

    String getPlatformId() const noexcept
    {
        return this->platformId;
    }

    Time getCreateTime() const noexcept
    {
        return this->createdAt;
    }

    Time getUpdateTime() const noexcept
    {
        return this->updatedAt;
    }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override
    {
        ValueTree root(Serialization::UserProfile::session);

        return root;
    }

    virtual void deserialize(const ValueTree &tree) override
    {
        this->reset();
        using namespace Serialization;

        const auto root = tree.hasType(UserProfile::session) ?
            tree : tree.getChildWithName(UserProfile::session);

        if (!root.isValid()) { return; }

        // TODO
    }

    virtual void reset() override {}

private:

    const String deviceId;
    const String platformId;
    const Time createdAt;
    const Time updatedAt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserSessionInfo)
};

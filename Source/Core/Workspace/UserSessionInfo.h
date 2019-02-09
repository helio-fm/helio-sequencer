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

    UserSessionInfo() = default;
    UserSessionInfo(const UserSessionDto &dto);

    void updateRemoteInfo(const UserSessionDto &dto);

    String getDeviceId() const noexcept;
    String getPlatformId() const noexcept;
    Time getCreateTime() const noexcept;
    Time getUpdateTime() const noexcept;

    using Ptr = ReferenceCountedObjectPtr<UserSessionInfo>;
    static int compareElements(UserSessionInfo *first, UserSessionInfo *second);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override;
    virtual void deserialize(const ValueTree &tree) override;
    virtual void reset() override;

private:

    String deviceId;
    String platformId;
    Time createdAt;
    Time updatedAt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserSessionInfo)
};

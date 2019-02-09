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
#include "BaseResource.h"
#include "UserResourceDto.h"

class SyncedConfigurationInfo final : public Serializable,
                                      public ReferenceCountedObject
{
public:

    SyncedConfigurationInfo() = default;
    SyncedConfigurationInfo(const UserResourceDto &remote);

    Identifier getType() const noexcept;
    String getName() const noexcept;

    bool equals(const BaseResource::Ptr resource) const noexcept;

    using Ptr = ReferenceCountedObjectPtr<SyncedConfigurationInfo>;

    static int compareElements(const SyncedConfigurationInfo *first,
        const SyncedConfigurationInfo *second);
    static int compareElements(const Identifier &type, const String &id,
        const SyncedConfigurationInfo *obj);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override;
    virtual void deserialize(const ValueTree &tree) override;
    virtual void reset() override;

private:

    Identifier type;
    String name;
    String hash;
    Time updatedAt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncedConfigurationInfo)
};

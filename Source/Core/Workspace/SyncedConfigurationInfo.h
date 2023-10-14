/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Serializable.h"
#include "ConfigurationResource.h"
#include "UserResourceDto.h"

class SyncedConfigurationInfo final : public Serializable,
                                      public ReferenceCountedObject
{
public:

    SyncedConfigurationInfo() = default;

#if !NO_NETWORK
    SyncedConfigurationInfo(const UserResourceDto &remote);
#endif

    Identifier getType() const noexcept;
    String getName() const noexcept;

    bool equals(const ConfigurationResource::Ptr resource) const noexcept;

    using Ptr = ReferenceCountedObjectPtr<SyncedConfigurationInfo>;

    static int compareElements(const SyncedConfigurationInfo *first,
        const SyncedConfigurationInfo *second);
    static int compareElements(const Identifier &type, const String &id,
        const SyncedConfigurationInfo *obj);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual SerializedData serialize() const override;
    virtual void deserialize(const SerializedData &data) override;
    virtual void reset() override;

private:

    Identifier type;
    String name;
    String hash;
    Time updatedAt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncedConfigurationInfo)
};

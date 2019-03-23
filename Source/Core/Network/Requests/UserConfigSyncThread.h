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

#include "BackendRequest.h"
#include "SyncedConfigurationInfo.h"
#include "BaseResource.h"

class UserConfigSyncThread final : public Thread, public WaitableEvent
{
public:

    UserConfigSyncThread();
    ~UserConfigSyncThread() override;

    Function<void()> onQueueEmptied;
    Function<void(const Array<String> &errors)> onSyncError;
    Function<void(const UserResourceDto resource)> onResourceFetched;
    Function<void(const UserResourceDto resource)> onResourceUpdated;
    Function<void(const Identifier &type, const String &name)> onResourceDeleted;

    void queueGetConfiguration(const SyncedConfigurationInfo::Ptr resource);
    void queuePutConfiguration(const BaseResource::Ptr resource);
    void queueDeleteConfiguration(const BaseResource::Ptr resource);

private:

    void run() override;

    ReferenceCountedArray<SyncedConfigurationInfo, CriticalSection> resourcesToGet;
    ReferenceCountedArray<BaseResource, CriticalSection> resourcesToPut;
    ReferenceCountedArray<BaseResource, CriticalSection> resourcesToDelete;

    bool areQueuesEmpty() const;

    BackendRequest::Response response;
    Identifier deletedConfigType;
    String deletedConfigName;

    friend class BackendService;

    JUCE_DECLARE_WEAK_REFERENCEABLE(UserConfigSyncThread)
};

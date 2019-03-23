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

#include "UpdatesCheckThread.h"
#include "BaseConfigSyncThread.h"
#include "UserConfigSyncThread.h"

#include "BackendService.h"
#include "SyncedConfigurationInfo.h"
#include "BaseResource.h"

class ResourceSyncService final : private BackendService
{
public:

    ResourceSyncService();

    void queueSync(const BaseResource::Ptr resource);
    void queueDelete(const BaseResource::Ptr resource);
    void queueFetch(const SyncedConfigurationInfo::Ptr resource);

private:

    UserConfigSyncThread *prepareSyncThread();
    WeakReference<UserConfigSyncThread> synchronizer;

    UpdatesCheckThread *prepareUpdatesCheckThread();
    BaseConfigSyncThread *prepareResourceRequestThread();

};

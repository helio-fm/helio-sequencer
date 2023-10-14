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

#if !NO_NETWORK

#include "UpdatesCheckThread.h"
#include "BaseConfigSyncThread.h"
#include "UserConfigSyncThread.h"

#include "BackendService.h"
#include "SyncedConfigurationInfo.h"
#include "ConfigurationResource.h"

class ResourceSyncService final : private BackendService
{
public:

    ResourceSyncService();

    void queueSync(const ConfigurationResource::Ptr resource);
    void queueDelete(const ConfigurationResource::Ptr resource);
    void queueFetch(const SyncedConfigurationInfo::Ptr resource);

private:

    UserConfigSyncThread *prepareSyncThread();
    WeakReference<UserConfigSyncThread> synchronizer;

    UpdatesCheckThread *prepareUpdatesCheckThread();
    BaseConfigSyncThread *prepareResourceRequestThread();

};

#endif

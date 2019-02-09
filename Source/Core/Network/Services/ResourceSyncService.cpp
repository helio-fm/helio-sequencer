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
#include "ResourceSyncService.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "UserProfile.h"
#include "SuccessTooltip.h"

// Try to update resources and versions info after:
#define UPDATE_INFO_TIMEOUT_MS (1000 * 10)

ResourceSyncService::ResourceSyncService() :
    synchronizer(this->prepareSyncThread())
{
    this->prepareUpdatesCheckThread()->checkForUpdates(UPDATE_INFO_TIMEOUT_MS);
    this->synchronizer->startThread(6);

    // todo subscribe on user profile changes
    // detect resources missing locally
    // and put them to download queue
}

void ResourceSyncService::queueSync(const BaseResource::Ptr resource)
{
    jassert(this->synchronizer != nullptr);
    this->synchronizer->queuePutConfiguration(resource);
    DBG("Queued uploading configuration resource: " +
        resource->getResourceType() + "/" + resource->getResourceId());
}

void ResourceSyncService::queueDelete(const BaseResource::Ptr resource)
{
    jassert(this->synchronizer != nullptr);
    this->synchronizer->queueDeleteConfiguration(resource);
    DBG("Queued deleting configuration resource: " +
        resource->getResourceType() + "/" + resource->getResourceId());
}

void ResourceSyncService::queueFetch(const SyncedConfigurationInfo::Ptr resource)
{
    jassert(this->synchronizer != nullptr);
    this->synchronizer->queueGetConfiguration(resource);
    DBG("Queued fetching configuration resource: " +
        resource->getType() + "/" + resource->getName());
}

UserConfigSyncThread *ResourceSyncService::prepareSyncThread()
{
    auto *thread = this->getNewThreadFor<UserConfigSyncThread>();

    // each callback is supposed to process the result and then resume
    // queue processing by calling WaitableEvent::signal()

    thread->onQueueEmptied = [this]()
    {
        // todo test silent mode
        //auto &layout = App::Layout();
        //layout.hideModalComponentIfAny();
        //layout.showModalComponentUnowned(new SuccessTooltip());
    };

    thread->onSyncError = [this](const Array<String> &errors)
    {
        this->synchronizer->signal();
    };

    thread->onResourceFetched = [this](const UserResourceDto dto)
    {
        auto &configs = App::Config().getAllResources();
        if (configs.contains(dto.getType()))
        {
            auto config = configs.at(dto.getType());
            // create a specific object based on resource->getType()
            BaseResource::Ptr resource(config->createResource());
            // then deserialize using this->response.getBody()
            resource->deserialize(dto.getData());
            config->updateUserResource(resource);
        }

        this->synchronizer->signal();
    };

    thread->onResourceUpdated = [this](const UserResourceDto resource)
    {
        auto &profile = App::Workspace().getUserProfile();
        profile.onConfigurationInfoUpdated(resource);
        this->synchronizer->signal();
    };

    thread->onResourceDeleted = [this](const Identifier &type, const String &name)
    {
        auto &profile = App::Workspace().getUserProfile();
        profile.onConfigurationInfoReset(type, name);
        this->synchronizer->signal();
    };

    return thread;
}

BaseConfigSyncThread *ResourceSyncService::prepareResourceRequestThread()
{
    auto *thread = this->getNewThreadFor<BaseConfigSyncThread>();

    thread->onRequestResourceOk = [this](const Identifier &resourceType, const ValueTree &resource)
    {
        if (App::Config().getAllResources().contains(resourceType))
        {
            App::Config().getAllResources().at(resourceType)->updateBaseResource(resource);
        }
    };

    return thread;
}

UpdatesCheckThread *ResourceSyncService::prepareUpdatesCheckThread()
{
    auto *thread = this->getNewThreadFor<UpdatesCheckThread>();

    thread->onUpdatesCheckOk = [this](const AppInfoDto info)
    {
        // check if any available resource has a hash different from stored one
        // then start threads to fetch those resources (with somewhat random delays)
        Random r;
        AppInfoDto lastUpdatesInfo;
        App::Config().load(&lastUpdatesInfo, Serialization::Config::lastUpdatesInfo);
        bool everythingIsUpToDate = true;
        for (const auto &newResource : info.getResources())
        {
            if (lastUpdatesInfo.resourceSeemsOutdated(newResource))
            {
                // I just don't want to fire all requests at once:
                const auto delay = r.nextInt(5) * 1000;
                this->prepareResourceRequestThread()->requestResource(newResource.getType(), delay);
                everythingIsUpToDate = false;
            }
        }

        if (everythingIsUpToDate)
        {
            DBG("All resources are up to date");
        }

        // save all anyway, as versions info might have changed:
        App::Config().save(&info, Serialization::Config::lastUpdatesInfo);
    };

    thread->onUpdatesCheckFailed = [](const Array<String> &errors)
    {
        DBG("onUpdatesCheckFailed: " + errors.getFirst());
    };

    return thread;
}

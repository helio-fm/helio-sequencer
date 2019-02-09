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
#include "UserConfigSyncThread.h"
#include "SerializationKeys.h"
#include "Network.h"

namespace ApiKeys = Serialization::Api::V1;
namespace ApiRoutes = Routes::Api;

UserConfigSyncThread::UserConfigSyncThread() :
    Thread("Sync") {}

UserConfigSyncThread::~UserConfigSyncThread()
{
    this->signalThreadShouldExit();
    this->signal();
    this->stopThread(1000);
}

void UserConfigSyncThread::queueGetConfiguration(const SyncedConfigurationInfo::Ptr resource)
{
    this->resourcesToGet.addIfNotAlreadyThere(resource);
    this->signal();
}

void UserConfigSyncThread::queuePutConfiguration(const BaseResource::Ptr resource)
{
    this->resourcesToDelete.removeObject(resource);
    this->resourcesToPut.addIfNotAlreadyThere(resource);
    this->signal();
}

void UserConfigSyncThread::queueDeleteConfiguration(const BaseResource::Ptr resource)
{
    this->resourcesToPut.removeObject(resource);
    this->resourcesToDelete.addIfNotAlreadyThere(resource);
    this->signal();
}

void UserConfigSyncThread::run()
{
    WaitableEvent::wait();

    while (!this->threadShouldExit())
    {
        // need to wait after each operation,
        // so that callback receives target resource object and says signal()

        if (const auto resource = this->resourcesToGet.getLast())
        {
            const String configurationRoute(ApiRoutes::customResource
                .replace(":resourceType", resource->getType())
                .replace(":resourceId", resource->getName()));

            const BackendRequest syncRequest(configurationRoute);
            this->response = syncRequest.get();
            if (this->response.is2xx())
            {
                callbackOnMessageThread(UserConfigSyncThread, onResourceFetched, { self->response.getBody() });
            }
            else
            {
                DBG("Failed to update resource: " + this->response.getErrors().getFirst());
                callbackOnMessageThread(UserConfigSyncThread, onSyncError, self->response.getErrors());
            }

            this->resourcesToGet.removeObject(resource);

            WaitableEvent::wait();
        }

        if (const auto resource = this->resourcesToPut.getLast())
        {
            const String configurationRoute(ApiRoutes::customResource
                .replace(":resourceType", resource->getResourceType())
                .replace(":resourceId", resource->getResourceId()));

            ValueTree payload(ApiKeys::Resources::resource);
            ValueTree data(ApiKeys::Resources::data);
            data.appendChild(resource->serialize(), nullptr);
            payload.appendChild(data, nullptr);

            const BackendRequest syncRequest(configurationRoute);
            this->response = syncRequest.put(payload);
            if (this->response.is2xx())
            {
                callbackOnMessageThread(UserConfigSyncThread, onResourceUpdated, { self->response.getBody() });
            }
            else
            {
                DBG("Failed to update resource: " + this->response.getErrors().getFirst());
                callbackOnMessageThread(UserConfigSyncThread, onSyncError, self->response.getErrors());
            }

            this->resourcesToPut.removeObject(resource);

            WaitableEvent::wait();
        }

        if (const auto resource = this->resourcesToDelete.getLast())
        {
            this->deletedConfigType = resource->getResourceType();
            this->deletedConfigName = resource->getResourceId();

            const String configurationRoute(ApiRoutes::customResource
                .replace(":resourceType", this->deletedConfigType)
                .replace(":resourceId", this->deletedConfigName));

            const BackendRequest syncRequest(configurationRoute);
            this->response = syncRequest.del();

            if (this->response.is(204) || this->response.is(404))
            {
                callbackOnMessageThread(UserConfigSyncThread, onResourceDeleted,
                    self->deletedConfigType, self->deletedConfigName);
            }
            else
            {
                DBG("Failed to delete resource: " + this->response.getErrors().getFirst());
                callbackOnMessageThread(UserConfigSyncThread, onSyncError, self->response.getErrors());
            }

            this->resourcesToDelete.removeObject(resource);

            WaitableEvent::wait();
        }

        if (this->areQueuesEmpty())
        {
            callbackOnMessageThread(UserConfigSyncThread, onQueueEmptied);
            WaitableEvent::wait();
        }
    }
}

bool UserConfigSyncThread::areQueuesEmpty() const
{
    return this->resourcesToGet.isEmpty()
        && this->resourcesToPut.isEmpty()
        && this->resourcesToDelete.isEmpty();
}

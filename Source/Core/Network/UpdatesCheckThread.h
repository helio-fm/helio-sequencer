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

#include "HelioApiRoutes.h"
#include "HelioApiRequest.h"
#include "Config.h"
#include "SerializationKeys.h"

struct UpdateInfo final
{
    struct VersionInfo final
    {
        String getPlatformId() const noexcept 
        { return this->info.getProperty(Serialization::Api::V1::platformId); }

        String getVersion() const noexcept 
        { return this->info.getProperty(Serialization::Api::V1::version); }

        String getLink() const noexcept 
        { return this->info.getProperty(Serialization::Api::V1::link); }

        const ValueTree info;
    };

    struct ResourceInfo final
    {
        String getResourceName() const noexcept 
        { return this->info.getProperty(Serialization::Api::V1::resourceName); }

        String getResourceHash() const noexcept 
        { return this->info.getProperty(Serialization::Api::V1::hash); }

        const ValueTree info;
    };

    Array<VersionInfo> getReleases() const
    {
        Array<VersionInfo> result;
        forEachValueTreeChildWithType(info, release, Serialization::Api::V1::versionInfo)
        { result.add({release}); }
        return result;
    }

    Array<ResourceInfo> getResources() const
    {
        Array<ResourceInfo> result;
        forEachValueTreeChildWithType(info, release, Serialization::Api::V1::resourceInfo)
        { result.add({release}); }
        return result;
    }

    ValueTree info;
};

class UpdatesCheckThread final : private Thread
{
public:

    UpdatesCheckThread() : Thread("UpdatesCheck"), listener(nullptr) {}

    ~UpdatesCheckThread() override
    {
        this->stopThread(1000);
    }

    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void updatesCheckOk(const UpdateInfo info) = 0;
        virtual void updatesCheckFailed(const Array<String> &errors) = 0;
        friend class UpdatesCheckThread;
    };
    
    void login(UpdatesCheckThread::Listener *listener)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->listener = listener;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        const HelioApiRequest request(HelioFM::Api::V1::requestUpdatesInfo);
        this->response = request.get();

        if (this->response.parseResult.failed() ||
            this->response.statusCode != 200 || !this->response.body.isValid())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<UpdatesCheckThread *>(ptr);
                self->listener->updatesCheckFailed(self->response.errors);
                return nullptr;
            }, this);
            return;
        }
        
        this->updateInfo = {this->response.body};

        MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
        {
            const auto self = static_cast<UpdatesCheckThread *>(ptr);
            self->listener->updatesCheckOk(self->updateInfo);
            return nullptr;
        }, this);
    }
    
    UpdateInfo updateInfo;
    HelioApiRequest::Response response;
    UpdatesCheckThread::Listener *listener;
    
    friend class SessionManager;
};

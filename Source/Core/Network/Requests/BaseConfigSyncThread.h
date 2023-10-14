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

#include "BackendRequest.h"
#include "Network.h"

class BaseConfigSyncThread final : public Thread
{
public:
    
    BaseConfigSyncThread() : Thread("BaseConfigSync") {}
    ~BaseConfigSyncThread() override
    {
        this->stopThread(1000);
    }
    
    Function<void(const Identifier &resourceType, const SerializedData &resource)> onRequestResourceOk;
    Function<void(const Identifier &resourceType, const Array<String> &errors)> onRequestResourceFailed;
    
    void requestResource(const Identifier &resourceType, uint32 delayMs)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->delay = delayMs;
        this->resourceType = resourceType;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        const auto targetTime = Time::getMillisecondCounter() + this->delay;
        while (Time::getMillisecondCounter() < targetTime)
        {
            Thread::sleep(100);
            if (this->threadShouldExit())
            {
                return;
            }
        }

        const String uri = Routes::Api::baseResource.replace(":resourceType", this->resourceType);
        const BackendRequest request(uri);
        this->response = request.get();

        if (!this->response.hasValidBody() || !this->response.is2xx())
        {
            callbackOnMessageThread(BaseConfigSyncThread, onRequestResourceFailed,
                self->resourceType, self->response.getErrors());
            return;
        }

        callbackOnMessageThread(BaseConfigSyncThread, onRequestResourceOk,
            self->resourceType, self->response.getChild(self->resourceType));
    }
    
    uint32 delay = 0;
    Identifier resourceType;
    BackendRequest::Response response;
    
    friend class BackendService;
};

#endif

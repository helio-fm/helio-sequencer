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

class RequestResourceThread final : public Thread
{
public:
    
    RequestResourceThread() : Thread("RequestResource") {}
    ~RequestResourceThread() override
    {
        this->stopThread(1000);
    }
    
    Function<void(const Identifier &resourceId, const ValueTree &resource)> onRequestResourceOk;
    Function<void(const Identifier &resourceId, const Array<String> &errors)> onRequestResourceFailed;
    
    void requestResource(const Identifier &resourceId, uint32 delayMs)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->delay = delayMs;
        this->resourceId = resourceId;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        namespace ApiRoutes = Routes::HelioFM::Api;

        Time::waitForMillisecondCounter(Time::getMillisecondCounter() + this->delay);

        const String uri = ApiRoutes::requestResource + "/" + this->resourceId;
        const HelioApiRequest request(uri);
        this->response = request.get();

        if (!this->response.isValid() || !this->response.is2xx())
        {
            callbackOnMessageThread(RequestResourceThread, onRequestResourceFailed,
                self->resourceId, self->response.getErrors());
            return;
        }

        callbackOnMessageThread(RequestResourceThread, onRequestResourceOk,
            self->resourceId, self->response.getChild(self->resourceId));
    }
    
    uint32 delay;
    Identifier resourceId;
    HelioApiRequest::Response response;
    
    friend class BackendService;
};

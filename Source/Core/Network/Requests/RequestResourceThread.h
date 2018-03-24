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

class RequestResourceThread final : private Thread
{
public:
    
    RequestResourceThread() : Thread("RequestResource"), listener(nullptr) {}
    ~RequestResourceThread() override
    {
        this->stopThread(1000);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void requestResourceOk(const Identifier &resourceId, const ValueTree &resource) = 0;
        virtual void requestResourceFailed(const Identifier &resourceId, const Array<String> &errors) = 0;
        friend class RequestResourceThread;
    };
    
    void requestResource(RequestResourceThread::Listener *listener, const Identifier &resourceId, uint32 delayMs)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->delay = delayMs;
        this->listener = listener;
        this->resourceId = resourceId;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        Time::waitForMillisecondCounter(Time::getMillisecondCounter() + this->delay);

        const String uri = HelioFM::Api::V1::requestResource + "/" + this->resourceId;
        const HelioApiRequest request(uri);
        this->response = request.get();

        if (!this->response.isValid() || !this->response.is2xx())
        {
            callRequestListener(RequestResourceThread, requestResourceFailed,
                self->resourceId, self->response.getErrors());
            return;
        }

        callRequestListener(RequestResourceThread, requestResourceOk,
            self->resourceId, self->response.getChild(self->resourceId));
    }
    
    uint32 delay;
    Identifier resourceId;
    HelioApiRequest::Response response;
    RequestResourceThread::Listener *listener;
    
    friend class BackendService;
};

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
#include "UpdatesInfo.h"

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
        virtual void updatesCheckOk(const UpdatesInfo info) = 0;
        virtual void updatesCheckFailed(const Array<String> &errors) = 0;
        friend class UpdatesCheckThread;
    };
    
    void checkForUpdates(UpdatesCheckThread::Listener *listener)
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
        namespace ApiRoutes = Routes::HelioFM::Api::V1;

        const HelioApiRequest request(ApiRoutes::requestUpdatesInfo);
        this->response = request.get();

        if (!this->response.isValid() || !this->response.is2xx())
        {
            callRequestListener(UpdatesCheckThread, updatesCheckFailed, self->response.getErrors());
            return;
        }
        
        callRequestListener(UpdatesCheckThread, updatesCheckOk, { self->response.getBody() });
    }
    
    HelioApiRequest::Response response;
    UpdatesCheckThread::Listener *listener;
    
    friend class BackendService;
};

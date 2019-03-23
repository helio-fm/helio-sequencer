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
#include "Config.h"
#include "SerializationKeys.h"
#include "AppInfoDto.h"
#include "Network.h"

class UpdatesCheckThread final : public Thread
{
public:

    UpdatesCheckThread() : Thread("UpdatesCheck") {}

    ~UpdatesCheckThread() override
    {
        this->stopThread(1000);
    }

    Function<void(const AppInfoDto info)> onUpdatesCheckOk;
    Function<void(const Array<String> &errors)> onUpdatesCheckFailed;
        
    void checkForUpdates(uint32 delayMs)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->delay = delayMs;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        namespace ApiRoutes = Routes::Api;

        Time::waitForMillisecondCounter(Time::getMillisecondCounter() + this->delay);

        const BackendRequest request(ApiRoutes::updatesInfo);
        this->response = request.get();

        if (!this->response.isValid() || !this->response.is2xx())
        {
            callbackOnMessageThread(UpdatesCheckThread, onUpdatesCheckFailed, self->response.getErrors());
            return;
        }
        
        callbackOnMessageThread(UpdatesCheckThread, onUpdatesCheckOk, { self->response.getBody() });
    }
    
    uint32 delay;
    BackendRequest::Response response;
    
    friend class BackendService;
};

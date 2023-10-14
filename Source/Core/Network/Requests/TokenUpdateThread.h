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
#include "Config.h"
#include "SerializationKeys.h"

class TokenUpdateThread final : public Thread
{
public:
    
    TokenUpdateThread() : Thread("TokenUpdate") {}
    ~TokenUpdateThread() override
    {
        this->stopThread(1000);
    }

    Function<void(const String &newToken)> onTokenUpdateOk;
    Function<void(const Array<String> &errors)> onTokenUpdateFailed;

    void updateToken(uint32 delayMs)
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
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::Api;

        const auto targetTime = Time::getMillisecondCounter() + this->delay;
        while (Time::getMillisecondCounter() < targetTime)
        {
            Thread::sleep(100);
            if (this->threadShouldExit())
            {
                return;
            }
        }

        SerializedData session(ApiKeys::session);
        session.setProperty(ApiKeys::deviceId, App::getDeviceId());
        session.setProperty(ApiKeys::platformId, SystemStats::getOperatingSystemName());

        const BackendRequest request(ApiRoutes::tokenUpdate);
        this->response = request.post(session);

        if (!this->response.hasValidBody() ||
            !this->response.is2xx() ||
            !this->response.hasProperty(ApiKeys::token))
        {
            callbackOnMessageThread(TokenUpdateThread, onTokenUpdateFailed, self->response.getErrors());
            return;
        }

        callbackOnMessageThread(TokenUpdateThread, onTokenUpdateOk,
            self->response.getProperty(ApiKeys::token));
    }
    
    uint32 delay = 0;
    BackendRequest::Response response;
    
    friend class BackendService;
};

#endif

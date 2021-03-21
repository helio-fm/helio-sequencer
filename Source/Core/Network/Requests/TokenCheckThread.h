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

#if !NO_NETWORK

#include "BackendRequest.h"
#include "SerializationKeys.h"

class TokenCheckThread final : public Thread
{
public:

    TokenCheckThread() : Thread("TokenCheck"), listener(nullptr) {}
    ~TokenCheckThread() override
    {
        this->stopThread(1000);
    }
    
    Function<void()> onTokenCheckOk;
    Function<void(const Array<String> &errors)> onTokenCheckFailed;

    void login(TokenCheckThread::Listener *listener)
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
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::Api;

        const BackendRequest request(ApiRoutes::tokenCheck);
        this->response = request.get();

        if (!this->response.is2xx())
        {
            callbackOnMessageThread(TokenCheckThread, onTokenCheckFailed, self->response.getErrors());
            return;
        }

        callbackOnMessageThread(TokenCheckThread, onTokenCheckOk);
    }

    BackendRequest::Response response;
    TokenCheckThread::Listener *listener;
    
    friend class BackendService;
};

#endif

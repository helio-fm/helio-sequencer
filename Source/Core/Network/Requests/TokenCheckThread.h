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
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void tokenCheckOk() = 0;
        virtual void tokenCheckFailed(const Array<String> &errors) = 0;
        virtual void tokenCheckNoResponse() = 0;
        friend class TokenCheckThread;
    };
    
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
        const BackendRequest request(HelioFM::Api::V1::tokenCheck);
        this->response = request.get();

        if (this->response.receipt.failed() || this->response.statusCode == 500)
        {
            callMessageThreadListenerFrom(TokenCheckThread, tokenCheckNoResponse);
            return;
        }

        if (this->response.receipt.failed() || this->response.statusCode != 200)
        {
            callMessageThreadListenerFrom(TokenCheckThread, tokenCheckFailed, self->response.errors);
            return;
        }

        callMessageThreadListenerFrom(TokenCheckThread, tokenCheckOk);
    }

    BackendRequest::Response response;
    TokenCheckThread::Listener *listener;
    
    friend class BackendService;
};

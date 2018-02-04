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

#include "DataEncoder.h"
#include "HelioApiRoutes.h"
#include "HelioApiRequest.h"
#include "SerializationKeys.h"

class TokenCheckThread final : private Thread
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
        virtual void tokenCheckConnectionFailed() = 0;
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
        const HelioApiRequest request(HelioFM::Api::V1::tokenCheck);
        this->response = request.get();

        if (this->response.result.failed())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<TokenCheckThread *>(ptr);
                self->listener->tokenCheckConnectionFailed();
                return nullptr;
            }, this);
            return;
        }

        if (this->response.statusCode != 200)
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<TokenCheckThread *>(ptr);
                self->listener->tokenCheckFailed(self->response.errors);
                return nullptr;
            }, this);
            return;
        }

        MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
        {
            const auto self = static_cast<TokenCheckThread *>(ptr);
            self->listener->tokenCheckOk();
            return nullptr;
        }, this);
    }

    HelioApiRequest::Response response;
    TokenCheckThread::Listener *listener;
    
    friend class SessionManager;
};

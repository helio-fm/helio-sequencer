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

#include "Config.h"
#include "SerializationKeys.h"

class TokenUpdateThread final : private Thread
{
public:
    
    TokenUpdateThread() : Thread("TokenUpdate"), listener(nullptr) {}
    ~TokenUpdateThread() override
    {
        this->stopThread(1000);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void tokenUpdateOk(const String &newToken) = 0;
        virtual void tokenUpdateFailed(const Array<String> &errors) = 0;
        virtual void tokenUpdateConnectionFailed() = 0;
        friend class TokenUpdateThread;
    };
    
    void login(TokenUpdateThread::Listener *listener)
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
        const HelioApiRequest request(HelioFM::Api::V1::tokenUpdate);
        this->response = request.post(var(payload));

        if (this->response.result.failed())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<TokenUpdateThread *>(ptr);
                self->listener->tokenUpdateConnectionFailed();
                return nullptr;
            }, this);
            return;
        }

        this->newToken = this->response.jsonBody.getProperty(Serialization::Network::token, {});

        if (this->response.statusCode != 200 || this->newToken.isEmpty())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<TokenUpdateThread *>(ptr);
                self->listener->tokenUpdateFailed(self->response.errors);
                return nullptr;
            }, this);
            return;
        }

        MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
        {
            const auto self = static_cast<TokenUpdateThread *>(ptr);
            self->listener->tokenUpdateOk(self->newToken);
            return nullptr;
        }, this);
    }
    
    String newToken;
    HelioApiRequest::Response response;
    TokenUpdateThread::Listener *listener;
    
    friend class SessionManager;
};

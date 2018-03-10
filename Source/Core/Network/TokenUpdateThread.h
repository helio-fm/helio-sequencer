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
        friend class TokenUpdateThread;
    };
    
    void updateToken(TokenUpdateThread::Listener *listener, String lastValidToken)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->oldToken = lastValidToken;
        this->listener = listener;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        using namespace Serialization;

        // Construct payload object:
        DynamicObject::Ptr session(new DynamicObject());
        session->setProperty(Api::V1::bearer, this->oldToken);
        session->setProperty(Api::V1::deviceId, Config::getDeviceId());
        session->setProperty(Api::V1::platformId, SystemStats::getOperatingSystemName());

        DynamicObject::Ptr payload(new DynamicObject());
        payload->setProperty(Api::V1::session, var(session));

        const HelioApiRequest request(HelioFM::Api::V1::tokenUpdate);
        this->response = request.post(var(payload));

        const bool hasToken = this->response.body.hasProperty(Api::V1::token);
        if (this->response.parseResult.failed() ||
            this->response.statusCode != 200 || !hasToken)
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
            const auto newToken = self->response.body.getProperty(Api::V1::token);
            self->listener->tokenUpdateOk(newToken);
            return nullptr;
        }, this);
    }
    
    String oldToken;
    HelioApiRequest::Response response;
    TokenUpdateThread::Listener *listener;
    
    friend class SessionManager;
};

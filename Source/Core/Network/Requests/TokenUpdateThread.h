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

class TokenUpdateThread final : public Thread
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
        virtual void tokenUpdateNoResponse() = 0;
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
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::HelioFM::Api;

        // Construct payload object:
        ValueTree session(ApiKeys::session);
        session.setProperty(ApiKeys::bearer, this->oldToken, nullptr);
        session.setProperty(ApiKeys::deviceId, Config::getDeviceId(), nullptr);
        session.setProperty(ApiKeys::platformId, SystemStats::getOperatingSystemName(), nullptr);

        const HelioApiRequest request(ApiRoutes::tokenUpdate);
        this->response = request.post(session);

        if (!this->response.isValid())
        {
            callRequestListener(TokenUpdateThread, tokenUpdateNoResponse);
            return;
        }

        if (!this->response.is2xx() || !this->response.hasProperty(ApiKeys::token))
        {
            callRequestListener(TokenUpdateThread, tokenUpdateFailed, self->response.getErrors());
            return;
        }

        callRequestListener(TokenUpdateThread, tokenUpdateOk,
            self->response.getProperty(ApiKeys::token));
    }
    
    String oldToken;
    HelioApiRequest::Response response;
    TokenUpdateThread::Listener *listener;
    
    friend class BackendService;
};

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
#include "SerializationKeys.h"
#include "AuthSession.h"
#include "Config.h"

class AuthThread final : private Thread
{
public:
    
    AuthThread() : Thread("Auth"), listener(nullptr) {}
    ~AuthThread() override
    {
        this->stopThread(1000);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void authSessionInitiated(const AuthSession session) = 0;
        virtual void authSessionFinished(const AuthSession session) = 0;
        virtual void authSessionFailed(const Array<String> &errors) = 0;
        friend class AuthThread;
    };
    
    void requestWebAuth(AuthThread::Listener *listener, String provider = "github")
    {
        if (this->isThreadRunning())
        {
            Logger::writeToLog("Warning: failed to start auth thread, already running");
            return;
        }

        this->provider = provider;
        this->listener = listener;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        jassert(this->provider.isNotEmpty());

        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::HelioFM::Api::V1;

        // Construct payload object:
        ValueTree session(ApiKeys::session);
        session.setProperty(ApiKeys::deviceId, Config::getDeviceId(), nullptr);
        session.setProperty(ApiKeys::platformId, SystemStats::getOperatingSystemName(), nullptr);
        
        const String postUri = ApiRoutes::requestWebAuth + "/" + this->provider;
        const HelioApiRequest initWebAuthRequest(postUri);
        this->response = initWebAuthRequest.post(session);

        const int noContent = 204;
        if (!this->response.isValid() ||
            !this->response.is(noContent))
        {
            callRequestListener(AuthThread, authSessionFailed, self->response.getErrors());
            return;
        }

        // Session manager will redirect user in a browser
        callRequestListener(AuthThread, authSessionInitiated, { self->response.getBody() });

        // Now check once a second if user has finished authentication
        const AuthSession authSession(this->response.getBody());
        const String getUri = ApiRoutes::requestWebAuth + "/" + authSession.getSessionId();
        const HelioApiRequest checkWebAuthRequest(getUri);

        do
        {
            Thread::sleep(1000);
            this->response = checkWebAuthRequest.get();
        } while (this->response.is(noContent));

        if (!this->response.isValid() ||
            !this->response.is200() ||
            !this->response.hasProperty(ApiKeys::token))
        {
            callRequestListener(AuthThread, authSessionFailed, self->response.getErrors());
            return;
        }

        callRequestListener(AuthThread, authSessionFinished, { self->response.getBody() });
    }
    
    String provider;
    HelioApiRequest::Response response;

    AuthThread::Listener *listener;

    friend class BackendService;
};

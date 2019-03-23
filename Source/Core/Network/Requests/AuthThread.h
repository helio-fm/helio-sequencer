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
#include "SerializationKeys.h"
#include "AuthSessionDto.h"
#include "Network.h"
#include "Config.h"

class AuthThread final : public Thread
{
public:
    
    AuthThread() : Thread("Auth") {}
    ~AuthThread() override
    {
        this->stopThread(1000);
    }
    
    Function<void(const AuthSessionDto session, const String &redirect)> onAuthSessionInitiated;
    Function<void(const AuthSessionDto session)> onAuthSessionFinished;
    Function<void(const Array<String> &errors)> onAuthSessionFailed;
    
    void requestWebAuth(String provider = "Github")
    {
        if (this->isThreadRunning())
        {
            DBG("Warning: failed to start auth thread, already running");
            return;
        }

        this->provider = provider;
        this->startThread(3);
    }

private:
    
    void run() override
    {
        jassert(this->provider.isNotEmpty());

        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::Api;

        // Construct payload object:
        ValueTree initSession(ApiKeys::AuthSession::session);
        initSession.setProperty(ApiKeys::AuthSession::provider, this->provider, nullptr);
        initSession.setProperty(ApiKeys::AuthSession::appName, "Helio", nullptr);
        initSession.setProperty(ApiKeys::AuthSession::appVersion, App::getAppReadableVersion(), nullptr);
        initSession.setProperty(ApiKeys::AuthSession::appPlatform, SystemStats::getOperatingSystemName(), nullptr);
        initSession.setProperty(ApiKeys::AuthSession::deviceId, App::getDeviceId(), nullptr);

        const BackendRequest initWebAuthRequest(ApiRoutes::initWebAuth);
        this->response = initWebAuthRequest.post(initSession);

        if (!this->response.isValid() ||
            !this->response.is(201))
        {
            DBG("Failed to init web auth: " + this->response.getErrors().getFirst());
            callbackOnMessageThread(AuthThread, onAuthSessionFailed, self->response.getErrors());
            return;
        }

        // Session manager will redirect user in a browser
        DBG("Initialized web auth, redirecting to: " + this->response.getRedirect());
        callbackOnMessageThread(AuthThread, onAuthSessionInitiated, { self->response.getBody() }, self->response.getRedirect());

        // Now check once a second if user has finished authentication
        const AuthSessionDto authSession(this->response.getBody());
        const BackendRequest checkWebAuthRequest(ApiRoutes::finaliseWebAuth);

        ValueTree finaliseSession(ApiKeys::AuthSession::session);
        finaliseSession.setProperty(ApiKeys::AuthSession::id, authSession.getSessionId(), nullptr);
        finaliseSession.setProperty(ApiKeys::AuthSession::secret, authSession.getSecret(), nullptr);

        // 204 response clearly indicates that a valid session exists,
        // and it hasn't failed, but still there's no token available:
        static const int noContent = 204;
        const int maxAttempts = 60 * 5;
        int numAttempts = 0;
        do
        {
            // Wait some time, but keep checking meanwhile if the thread should stop
            for (int i = 0; i < 30; ++i)
            {
                Thread::sleep(100);
                if (this->threadShouldExit())
                {
                    DBG("Canceled web auth process, exiting.");
                    return; // canceled by user, no callbacks
                }
            }

            this->response = checkWebAuthRequest.post(finaliseSession);
        } while (this->response.is(noContent) && numAttempts < maxAttempts);

        if (!this->response.isValid() ||
            !this->response.is200() ||
            !this->response.hasProperty(ApiKeys::AuthSession::token))
        {
            DBG("Failed to finalize web auth: " + this->response.getErrors().getFirst());
            callbackOnMessageThread(AuthThread, onAuthSessionFailed, self->response.getErrors());
            return;
        }

        callbackOnMessageThread(AuthThread, onAuthSessionFinished, { self->response.getBody() });
    }
    
    String provider;
    BackendRequest::Response response;

    friend class BackendService;
};

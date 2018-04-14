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
#include "Config.h"

class SignInThread final : private Thread
{
public:
    
    SignInThread() : Thread("SignIn"), listener(nullptr) {}
    ~SignInThread() override
    {
        this->stopThread(1000);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void signInOk(const String &userEmail, const String &newToken) = 0;
        virtual void signInFailed(const Array<String> &errors) = 0;
        friend class SignInThread;
    };
    
    void signIn(SignInThread::Listener *listener, String userEmail, String userPassword)
    {
        if (this->isThreadRunning())
        {
            Logger::writeToLog("Warning: failed to start sign-in thread, already running");
            return;
        }

        this->email = userEmail;
        this->password = userPassword;
        this->listener = listener;

        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        namespace ApiKeys = Serialization::Api::V1;
        namespace ApiRoutes = Routes::HelioFM::Api::V1;

        // Construct payload object:
        ValueTree session(ApiKeys::session);
        session.setProperty(ApiKeys::email, this->email, nullptr);
        session.setProperty(ApiKeys::password, this->password, nullptr);
        session.setProperty(ApiKeys::deviceId, Config::getDeviceId(), nullptr);
        session.setProperty(ApiKeys::platformId, SystemStats::getOperatingSystemName(), nullptr);

        // Clear password just not to keep it in the memory:
        this->password = {};

        const HelioApiRequest request(ApiRoutes::login);
        this->response = request.post(session);

        if (!this->response.isValid() ||
            !this->response.is2xx() ||
            !this->response.hasProperty(ApiKeys::token))
        {
            callRequestListener(SignInThread, signInFailed, self->response.getErrors());
            return;
        }

        callRequestListener(SignInThread, signInOk,
            self->email, self->response.getProperty(ApiKeys::token));
    }
    
    String email;
    String password;
    HelioApiRequest::Response response;

    SignInThread::Listener *listener;

    friend class BackendService;
};

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
        // Construct payload object:
        DynamicObject::Ptr session(new DynamicObject());
        session->setProperty(Serialization::Api::V1::email, this->email);
        session->setProperty(Serialization::Api::V1::password, this->password);
        session->setProperty(Serialization::Api::V1::deviceId, Config::getDeviceId());
        session->setProperty(Serialization::Api::V1::platformId, SystemStats::getOperatingSystemName());

        DynamicObject::Ptr payload(new DynamicObject());
        payload->setProperty(Serialization::Api::V1::session, var(session));

        // Clear password just not to keep it in the memory:
        this->password = {};

        const HelioApiRequest request(HelioFM::Api::V1::login);
        this->response = request.post(var(payload));

        if (!this->response.isValid() ||
            !this->response.is2xx() ||
            !this->response.hasProperty(Serialization::Api::V1::token))
        {
            callRequestListener(SignInThread, signInFailed, self->response.getErrors());
            return;
        }

        callRequestListener(SignInThread, signInOk,
            self->email, self->response.getProperty(Serialization::Api::V1::token));
    }
    
    String email;
    String password;
    HelioApiRequest::Response response;

    SignInThread::Listener *listener;

    friend class BackendService;
};

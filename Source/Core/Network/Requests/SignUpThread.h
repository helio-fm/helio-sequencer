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

class SignUpThread final : private Thread
{
public:
    
    SignUpThread() : Thread("SignUp"), listener(nullptr) {}
    ~SignUpThread() override
    {
        this->stopThread(1000);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void signUpOk(const String &userEmail, const String &newToken) = 0;
        virtual void signUpFailed(const Array<String> &errors) = 0;
        friend class SignUpThread;
    };
    
    void signUp(SignUpThread::Listener *listener,
        String userName, String userLogin,
        String userEmail, String userPassword)
    {
        if (this->isThreadRunning())
        {
            Logger::writeToLog("Warning: failed to start sign-up thread, already running");
            return;
        }

        this->name = userName;
        this->login = userLogin;
        this->email = userEmail;
        this->password = userPassword;
        this->listener = listener;

        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        using namespace Serialization;

        // Construct payload object:
        DynamicObject::Ptr user(new DynamicObject());
        user->setProperty(Api::V1::name, this->name);
        user->setProperty(Api::V1::email, this->email);
        user->setProperty(Api::V1::login, this->login);
        user->setProperty(Api::V1::password, this->password);
        user->setProperty(Api::V1::passwordConfirmation, this->password);

        DynamicObject::Ptr payload(new DynamicObject());
        payload->setProperty(Api::V1::user, var(user));

        // Clear password just not to keep it in the memory:
        this->password = {};

        const HelioApiRequest request(HelioFM::Api::V1::join);
        this->response = request.post(var(payload));

        if (!this->response.isValid() ||
            !this->response.is2xx() ||
            !this->response.hasProperty(Api::V1::token))
        {
            callRequestListener(SignUpThread, signUpFailed, self->response.getErrors());
            return;
        }

        callRequestListener(SignUpThread, signUpOk,
            self->email, self->response.getProperty(Api::V1::token));
    }
    
    String email;
    String name;
    String login;
    String password;
    HelioApiRequest::Response response;

    SignUpThread::Listener *listener;

    friend class BackendService;
};

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
        virtual void signUpConnectionFailed() = 0;
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
        // Construct payload object:
        DynamicObject::Ptr user(new DynamicObject());
        user->setProperty(Serialization::Api::V1::name, this->name);
        user->setProperty(Serialization::Api::V1::email, this->email);
        user->setProperty(Serialization::Api::V1::login, this->login);
        user->setProperty(Serialization::Api::V1::password, this->password);
        user->setProperty(Serialization::Api::V1::passwordConfirmation, this->password);

        DynamicObject::Ptr payload(new DynamicObject());
        payload->setProperty(Serialization::Api::V1::user, var(user));

        // Clear password just not to keep it in the memory:
        this->password = {};

        const HelioApiRequest request(HelioFM::Api::V1::join);
        this->response = request.post(var(payload));

        if (this->response.result.failed())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<SignUpThread *>(ptr);
                self->listener->signUpConnectionFailed();
                return nullptr;
            }, this);
            return;
        }

        const bool hasToken = this->response.jsonBody.contains(Serialization::Api::V1::token);
        if (this->response.statusCode != 200 || !hasToken)
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
            {
                const auto self = static_cast<SignUpThread *>(ptr);
                self->listener->signUpFailed(self->response.errors);
                return nullptr;
            }, this);
            return;
        }

        MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void*
        {
            const auto self = static_cast<SignUpThread *>(ptr);
            const auto token = self->response.jsonBody[Serialization::Api::V1::token];
            self->listener->signUpOk(self->email, token);
            return nullptr;
        }, this);
    }
    
    String email;
    String name;
    String login;
    String password;
    HelioApiRequest::Response response;

    SignUpThread::Listener *listener;
};

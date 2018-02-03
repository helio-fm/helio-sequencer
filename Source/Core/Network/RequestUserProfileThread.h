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

class RequestUserProfileThread : private Thread
{
public:
    
#define NUM_LOGIN_ATTEMPTS 3

    RequestUserProfileThread() : Thread("RequestUserProfile"), listener(nullptr) {}

    ~RequestUserProfileThread() override
    {
        this->stopThread(1000);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void loginOk(const String &userEmail, const String &newToken) = 0;
        virtual void loginAuthorizationFailed() = 0;
        virtual void loginConnectionFailed() = 0;
        friend class LoginThread;
    };
    
    void login(LoginThread::Listener *authListener,
        String userEmail, String userPasswordHash)
    {
        if (this->isThreadRunning())
        {
            return;
        }

        this->listener = authListener;
        this->email = userEmail;
        this->passwordHash = userPasswordHash;
        this->startThread(3);
    }
    
private:
    
    void run() override
    {
        const String deviceId(Config::getMachineId());
        const URL url = URL(HelioFM::Api::V1::login)
            .withParameter(Serialization::Network::email, this->email)
            .withParameter(Serialization::Network::passwordHash, this->passwordHash)
            .withParameter(Serialization::Network::deviceId, deviceId);

        const HelioApiRequest request(url);
        const auto response = request.request();

        if (response.result.failed())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
            {
                LoginThread *self = static_cast<LoginThread *>(data);
                self->listener->loginConnectionFailed();
                return nullptr;
            }, this);
            return;
        }

        this->token = response.jsonBody.getProperty(Serialization::Network::token, {});

        if (response.statusCode != 200 || this->token.isEmpty())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
            {
                LoginThread *self = static_cast<LoginThread *>(data);
                self->listener->loginAuthorizationFailed();
                return nullptr;
            }, this);
            return;
        }


        const Identifier idProperty("id");
        const Identifier keyProperty("key");
        const Identifier titleProperty("title");
        const Identifier timeProperty("time");
        const Identifier emailProperty("email");

        if (json.isArray())
        {
            for (int i = 0; i < json.size(); ++i)
            {
                var &child(json[i]);
                jassert(!child.isVoid());

                if (DynamicObject *obj = child.getDynamicObject())
                {
                    NamedValueSet &props(obj->getProperties());
                    RemoteProjectDescription description;

                    for (int j = 0; j < props.size(); ++j)
                    {
                        const Identifier key(props.getName(j));
                        var value(props[key]);

                        if (key == idProperty)
                        {
                            description.projectId = value;
                            //Logger::writeToLog("::" + description.projectId + "::");
                        }
                        else if (key == keyProperty)
                        {
                            description.projectKey = DataEncoder::deobfuscateString(value);
                            //Logger::writeToLog("::" + description.projectKey + "::");
                        }
                        else if (key == titleProperty)
                        {
                            description.projectTitle = value;
                            //Logger::writeToLog("::" + description.projectTitle + "::");
                        }
                        else if (key == timeProperty)
                        {
                            description.lastModifiedTime = int64(value) * 1000;
                        }
                        else if (key == emailProperty)
                        {
                            this->userEmail = value;
                        }
                    }

                    if (description.projectId.isNotEmpty() &&
                        description.projectKey.isNotEmpty())
                    {
                        const ScopedWriteLock lock(this->listLock);
                        this->projectsList.add(description);
                    }
                }
            }
        }

        if (this->userEmail.isEmpty())
        {
            MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
            {
                RequestProjectsListThread *self = static_cast<RequestProjectsListThread *>(data);
                self->listener->listRequestAuthorizationFailed();
                return nullptr;
            }, this);

            return;
        }

        MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
        {
            LoginThread *self = static_cast<LoginThread *>(data);
            self->listener->loginOk(self->email, self->token);
            return nullptr;
        }, this);
    }
    
    String email;
    String passwordHash;
    String token;
    LoginThread::Listener *listener;
    
    friend class SessionManager;
};

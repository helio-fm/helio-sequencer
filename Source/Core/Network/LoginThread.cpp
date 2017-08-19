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

#include "Common.h"
#include "LoginThread.h"

#include "DataEncoder.h"
#include "HelioServerDefines.h"

#include "Config.h"

#include "Supervisor.h"
#include "SerializationKeys.h"

#define NUM_CONNECT_ATTEMPTS 3


LoginThread::LoginThread() :
    Thread("Login"),
    listener(nullptr)
{
}

LoginThread::~LoginThread()
{
    this->stopThread(1000);
}

void LoginThread::login(LoginThread::Listener *authListener,
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

void LoginThread::run()
{
    const String deviceId(Config::getMachineId());
    const String saltedDeviceId = deviceId + HELIO_SALT;
    const String saltedDeviceIdHash = SHA256(saltedDeviceId.toUTF8()).toHexString();

    for (int i = 0; i < NUM_CONNECT_ATTEMPTS; ++i)
    {
        URL url(HELIO_LOGIN_URL);
        url = url.withParameter(Serialization::Network::email, this->email);
        url = url.withParameter(Serialization::Network::passwordHash, this->passwordHash);
        url = url.withParameter(Serialization::Network::deviceId, deviceId);
        url = url.withParameter(Serialization::Network::clientCheck, saltedDeviceIdHash);

        {
            int statusCode = 0;
            StringPairArray responseHeaders;

            ScopedPointer<InputStream> downloadStream(
                url.createInputStream(true, nullptr, nullptr, HELIO_USERAGENT, 0, &responseHeaders, &statusCode));

            if (!downloadStream)
            {
                continue;
            }

            //const String rawResult = downloadStream->readEntireStreamAsString().trim();
            //Logger::writeToLog("Login: " + String(statusCode) + rawResult);

            if (statusCode != 200)
            {
                MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
                    {
                        LoginThread *self = static_cast<LoginThread *>(data);
                        self->listener->loginAuthorizationFailed();
                        return nullptr;
                    },
                    this);

                return;
            }

            MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
                {
                    LoginThread *self = static_cast<LoginThread *>(data);
                    self->listener->loginOk(self->email);
                    return nullptr;
                },
                this);

            return;
        }
    }

    MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
        {
            LoginThread *self = static_cast<LoginThread *>(data);
            self->listener->loginConnectionFailed();
            return nullptr;
        },
        this);
}

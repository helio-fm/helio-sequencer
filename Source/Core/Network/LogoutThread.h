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

class LogoutThread : private Thread
{
public:
    
    LogoutThread() :
        Thread("Logout"),
        listener(nullptr) {}

    ~LogoutThread() override
    {
        this->stopThread(100);
    }
    
    class Listener
    {
    public:
        virtual ~Listener() {}
    private:
        virtual void logoutOk() = 0;
        virtual void logoutFailed() = 0;
        virtual void logoutConnectionFailed() = 0;
        friend class LogoutThread;
    };
    
    void logout(LogoutThread::Listener *authListener)
    {
        jassert(!this->isThreadRunning());
        this->listener = authListener;
        this->startThread(3);
    }
    
private:

    void run() override
    {
        const String deviceId(Config::getMachineId());
        const String saltedDeviceId = deviceId + HELIO_SALT;
        const String saltedDeviceIdHash = SHA256(saltedDeviceId.toUTF8()).toHexString();

        for (int i = 0; i < NUM_CONNECT_ATTEMPTS; ++i)
        {
            URL url(HELIO_LOGOUT_URL);
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

                const String rawResult = downloadStream->readEntireStreamAsString().trim();
                const String result = DataEncoder::deobfuscateString(rawResult);

                // Logger::writeToLog("Logout, result: " + result);

                if (statusCode != 200)
                {
                    MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
                    {
                        LogoutThread *self = static_cast<LogoutThread *>(data);
                        self->listener->logoutFailed();
                        return nullptr;
                    },
                        this);

                    return;
                }

                MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
                {
                    LogoutThread *self = static_cast<LogoutThread *>(data);
                    self->listener->logoutOk();
                    return nullptr;
                },
                    this);

                return;
            }
        }

        MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
        {
            LogoutThread *self = static_cast<LogoutThread *>(data);
            self->listener->logoutConnectionFailed();
            return nullptr;
        },
            this);
    }
    
    LogoutThread::Listener *listener;
    friend class SessionManager;

};

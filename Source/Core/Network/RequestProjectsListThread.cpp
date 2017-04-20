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
#include "RequestProjectsListThread.h"

#include "AuthorizationManager.h"
#include "Config.h"

#include "DataEncoder.h"
#include "HelioServerDefines.h"

#include "Supervisor.h"
#include "SerializationKeys.h"

#define NUM_CONNECT_ATTEMPTS 4


RequestProjectsListThread::RequestProjectsListThread() :
    Thread("RequestProjectsList"),
    listener(nullptr)
{
}

RequestProjectsListThread::~RequestProjectsListThread()
{
    this->stopThread(100);
}

void RequestProjectsListThread::requestListAndEmail(RequestProjectsListThread::Listener *authListener)
{
    jassert(!this->isThreadRunning());
    this->listener = authListener;
    this->startThread(3);
}

void RequestProjectsListThread::run()
{
    {
        ScopedWriteLock lock(this->listLock);
        this->projectsList.clear();
    }

    const String deviceId(Config::getMachineId());
    const String saltedDeviceId = deviceId + HELIO_SALT;
    const String saltedDeviceIdHash = SHA256(saltedDeviceId.toUTF8()).toHexString();

    for (int attempt = 0; attempt < NUM_CONNECT_ATTEMPTS; ++attempt)
    {
        URL url(HELIO_PROJECTLIST_URL);
        url = url.withParameter(Serialization::Network::deviceId, deviceId);
        url = url.withParameter(Serialization::Network::clientCheck, saltedDeviceIdHash);

        {
			StringPairArray responseHeaders;
			int statusCode = 0;

			ScopedPointer<InputStream> downloadStream(
				url.createInputStream(true, nullptr, nullptr, HELIO_USERAGENT, 0, &responseHeaders, &statusCode));

			//Logger::writeToLog("statusCode: " + String(statusCode));

            if (!downloadStream || statusCode != 200)
            {
                continue;
            }

            const String &rawResult = downloadStream->readEntireStreamAsString();
			//Logger::writeToLog(rawResult);

            var json;
            Result result = JSON::parse(rawResult, json);

            if (! result.wasOk())
            {
				Logger::writeToLog("JSON parse error: " + result.getErrorMessage());
				continue;
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
                    jassert (!child.isVoid());

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
                            ScopedWriteLock lock(this->listLock);
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
					},
					this);

                return;
            }

            MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
				{
					RequestProjectsListThread *self = static_cast<RequestProjectsListThread *>(data);
					ScopedReadLock lock(self->listLock);
					self->listener->listRequestOk(self->userEmail, self->projectsList);
					return nullptr;
				},
				this);

            return;
        }
    }

    MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
		{
			RequestProjectsListThread *self = static_cast<RequestProjectsListThread *>(data);
			self->listener->listRequestConnectionFailed();
			return nullptr;
		},
		this);
}

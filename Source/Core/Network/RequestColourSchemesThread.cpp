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
#include "RequestColourSchemesThread.h"
#include "Config.h"

#include "DataEncoder.h"
#include "HelioServerDefines.h"
#include "Supervisor.h"
#include "SerializationKeys.h"

RequestColourSchemesThread::RequestColourSchemesThread() :
    Thread("RequestColourSchemes"),
    listener(nullptr)
{
}

RequestColourSchemesThread::~RequestColourSchemesThread()
{
    this->stopThread(100);
}

String RequestColourSchemesThread::getLastFetchedData() const
{
    ScopedReadLock lock(this->dataLock);
    return this->lastFetchedData;
}

void RequestColourSchemesThread::requestSchemes(RequestColourSchemesThread::Listener *authListener)
{
    jassert(!this->isThreadRunning());
    this->listener = authListener;
    this->startThread(2);
}


void RequestColourSchemesThread::run()
{
    //===------------------------------------------------------------------===//
    // Download
    //===------------------------------------------------------------------===//

    const String deviceId(Config::getMachineId());
    const String saltedDeviceId = deviceId + HELIO_SALT;
    const String saltedDeviceIdHash = SHA256(saltedDeviceId.toUTF8()).toHexString();

    URL fetchUrl(HELIO_COLOUR_SCHEMES_URL);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::deviceId, deviceId);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::clientCheck, saltedDeviceIdHash);

    {
		int statusCode = 0;
		StringPairArray responseHeaders;

		ScopedPointer<InputStream> downloadStream(
			fetchUrl.createInputStream(true, nullptr, nullptr, HELIO_USERAGENT, 0, &responseHeaders, &statusCode));

		if (downloadStream != nullptr && statusCode == 200)
		{
            {
                ScopedWriteLock lock(this->dataLock);
                this->lastFetchedData = downloadStream->readEntireStreamAsString();
            }
            
			MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
				{
					RequestColourSchemesThread *self = static_cast<RequestColourSchemesThread *>(data);
					self->listener->schemesRequestOk(self);
					return nullptr;
				},
				this);
			
			return;
        }
    }

	MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
		{
			RequestColourSchemesThread *self = static_cast<RequestColourSchemesThread *>(data);
			self->listener->schemesRequestFailed(self);
			return nullptr;
		},
		this);
}

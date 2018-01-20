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
#include "RequestArpeggiatorsThread.h"
#include "Config.h"

#include "DataEncoder.h"
#include "HelioServerDefines.h"
#include "SerializationKeys.h"
#include "ArpeggiatorsManager.h"

RequestArpeggiatorsThread::RequestArpeggiatorsThread() :
    Thread("RequestArpeggiators"),
    listener(nullptr) {}

RequestArpeggiatorsThread::~RequestArpeggiatorsThread()
{
    this->stopThread(100);
}

String RequestArpeggiatorsThread::getLastFetchedData() const
{
    const ScopedReadLock lock(this->dataLock);
    return this->lastFetchedData;
}

void RequestArpeggiatorsThread::requestArps(RequestArpeggiatorsThread::Listener *authListener)
{
    jassert(!this->isThreadRunning());
    this->arpsToUpdate = "";
    this->listener = authListener;
    this->startThread(3);
}

void RequestArpeggiatorsThread::updateArps(const String &newArps, RequestArpeggiatorsThread::Listener *authListener)
{
    jassert(!this->isThreadRunning());
    this->arpsToUpdate = newArps;
    this->listener = authListener;
    this->startThread(3);
}


void RequestArpeggiatorsThread::run()
{
    //===------------------------------------------------------------------===//
    // Download
    //===------------------------------------------------------------------===//

    const String deviceId(Config::getMachineId());
    const String saltedDeviceId = deviceId + HELIO_SALT;
    const String saltedDeviceIdHash = SHA256(saltedDeviceId.toUTF8()).toHexString();

    URL fetchUrl(HELIO_ARPS_URL);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::deviceId, deviceId);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::clientCheck, saltedDeviceIdHash);

    const bool pushMode = this->arpsToUpdate.isNotEmpty();
    
    if (pushMode)
    {
        fetchUrl = fetchUrl.withParameter(Serialization::Network::data, this->arpsToUpdate);
    }

    {
        int statusCode = 0;
        StringPairArray responseHeaders;

        ScopedPointer<InputStream> downloadStream(
            fetchUrl.createInputStream(true, nullptr, nullptr, HELIO_USERAGENT, 0, &responseHeaders, &statusCode));

        if (downloadStream != nullptr && statusCode == 200)
        {
            if (! pushMode)
            {
                const ScopedWriteLock lock(this->dataLock);
                this->lastFetchedData = downloadStream->readEntireStreamAsString();
            }

            MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
                {
                    RequestArpeggiatorsThread *self = static_cast<RequestArpeggiatorsThread *>(data);
                    self->listener->arpsRequestOk(self);
                    return nullptr;
                },
                this);

            return;
        }
    }

    MessageManager::getInstance()->callFunctionOnMessageThread([](void *data) -> void*
        {
            RequestArpeggiatorsThread *self = static_cast<RequestArpeggiatorsThread *>(data);
            self->listener->arpsRequestFailed(self);
            return nullptr;
        },
        this);
}

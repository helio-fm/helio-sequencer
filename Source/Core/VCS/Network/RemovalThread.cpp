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
#include "RemovalThread.h"
#include "VersionControl.h"
#include "Client.h"
#include "DataEncoder.h"
#include "HelioServerDefines.h"
#include "App.h"
#include "AuthorizationManager.h"
#include "Config.h"
#include "Supervisor.h"
#include "SerializationKeys.h"

using namespace VCS;

RemovalThread::RemovalThread(URL pushUrl,
                             String projectId,
                             MemoryBlock projectKey) :
    SyncThread(pushUrl, projectId, projectKey, nullptr)
{
}

void RemovalThread::run()
{
    const String saltedId = this->localId + HELIO_SALT;
    const String saltedIdHash = SHA256(saltedId.toUTF8()).toHexString();
    const String keyHash = SHA256(this->localKey.toString().toUTF8()).toHexString();
    
    //===------------------------------------------------------------------===//
    // Delete
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::sync);

    URL removeUrl(this->url);
    removeUrl = removeUrl.withParameter(Serialization::Network::remove, this->localId);
    removeUrl = removeUrl.withParameter(Serialization::Network::clientCheck, saltedIdHash);
    removeUrl = removeUrl.withParameter(Serialization::Network::key, keyHash);
    
    const bool loggedIn = (App::Helio()->getAuthManager()->getAuthorizationState() == AuthorizationManager::LoggedIn);
    
    if (loggedIn)
    {
        const String deviceId(Config::getMachineId());
        const String obfustatedKey = DataEncoder::obfuscateString(this->localKey.toBase64Encoding());
        removeUrl = removeUrl.withParameter(Serialization::Network::realKey, obfustatedKey);
        removeUrl = removeUrl.withParameter(Serialization::Network::deviceId, deviceId);
    }
    
    {
        int statusCode = 0;
        StringPairArray responseHeaders;

        ScopedPointer<InputStream> removalStream(
            removeUrl.createInputStream(true,
                syncProgressCallback,
                static_cast<void *>(this),
                HELIO_USERAGENT,
                0,
                &responseHeaders,
                &statusCode));

        if (removalStream == nullptr)
        {
            Supervisor::track(Serialization::Activities::networkConnectionError);
            this->setState(SyncThread::syncError);
            return;
        }
        
        const String rawResult = removalStream->readEntireStreamAsString().trim();
        const String result = DataEncoder::deobfuscateString(rawResult);
        
        Logger::writeToLog("Delete, raw result: " + rawResult);
        Logger::writeToLog("Delete, result: " + result);
        
        if (statusCode == 401)
        {
            this->setState(SyncThread::unauthorizedError);
            return;
        }
        if (statusCode == 403)
        {
            this->setState(SyncThread::forbiddenError);
            return;
        }
        else if (statusCode == 404)
        {
            this->setState(SyncThread::notFoundError);
            return;
        }
        else if (statusCode != 200)
        {
            this->setState(SyncThread::syncError);
            return;
        }
    }

    Supervisor::track(Serialization::Activities::vcsDelete);
    this->setState(SyncThread::allDone);

    // On success we ask the auth manager to update his projects list.
    App::Helio()->getAuthManager()->requestSessionData();
}

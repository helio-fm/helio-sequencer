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
#include "App.h"
#include "SessionManager.h"
#include "Config.h"
#include "SerializationKeys.h"

using namespace VCS;

RemovalThread::RemovalThread(URL pushUrl,
                             String projectId,
                             MemoryBlock projectKey) :
    SyncThread(pushUrl, projectId, projectKey, {}) {}

void RemovalThread::run()
{
    const String saltedId = this->localId + "salt";
    const String saltedIdHash = SHA256(saltedId.toUTF8()).toHexString();
    const String keyHash = SHA256(this->localKey.toString().toUTF8()).toHexString();
    
    //===------------------------------------------------------------------===//
    // Delete
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::sync);

    URL removeUrl(this->url);

    const bool loggedIn = (App::Helio()->getSessionManager()->getAuthorizationState() == SessionManager::LoggedIn);
    
    if (loggedIn)
    {
        const String deviceId(Config::getDeviceId());
        const String obfustatedKey = this->localKey.toBase64Encoding();

    }
    
    {
        int statusCode = 0;
        StringPairArray responseHeaders;

        ScopedPointer<InputStream> removalStream(
            removeUrl.createInputStream(true,
                syncProgressCallback,
                static_cast<void *>(this),
                "todo useragent",
                0,
                &responseHeaders,
                &statusCode));

        if (removalStream == nullptr)
        {
            this->setState(SyncThread::syncError);
            return;
        }
        
        const String rawResult = removalStream->readEntireStreamAsString().trim();
        const String result = rawResult;
        
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

    this->setState(SyncThread::allDone);
}

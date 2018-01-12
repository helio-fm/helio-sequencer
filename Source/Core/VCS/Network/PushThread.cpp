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
#include "PushThread.h"
#include "VersionControl.h"
#include "Client.h"
#include "DataEncoder.h"
#include "HelioServerDefines.h"
#include "App.h"
#include "AuthManager.h"
#include "Config.h"
#include "Supervisor.h"
#include "HttpConnection.h"
#include "SerializationKeys.h"

using namespace VCS;

PushThread::PushThread(URL pushUrl,
                       String projectId,
                       String projectTitle,
                       MemoryBlock projectKey,
                       ScopedPointer<XmlElement> pushContent) :
    SyncThread(pushUrl, projectId, projectKey, pushContent),
    title(std::move(projectTitle)) {}

void PushThread::run()
{
    const String saltedId = this->localId + HELIO_SALT;
    const String saltedIdHash = SHA256(saltedId.toUTF8()).toHexString();
    const String keyHash = SHA256(this->localKey.toString().toUTF8()).toHexString();
    
    //===------------------------------------------------------------------===//
    // Fetch remote history
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::fetchHistory);

    ScopedPointer<XmlElement> remoteXml;

    URL fetchUrl(this->url);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::fetch, this->localId);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::clientCheck, saltedIdHash);

    // TODO like this:

    //const HttpConnection connection(fetchUrl,
    //    [this](int bytesSent, int bytesTotal) { this->setProgress(bytesSent, bytesTotal); });

    //const auto response = connection.requestJson();

    //if (response.result.failed() || (response.statusCode != 200 && response.statusCode != 404))
    //{
    //    this->setState(SyncThread::fetchHistoryError);
    //    return;
    //}

    {
        int statusCode = 0;
        StringPairArray responseHeaders;

        ScopedPointer<InputStream> downloadStream(
            fetchUrl.createInputStream(true,
                syncProgressCallback,
                static_cast<void *>(this),
                HELIO_USERAGENT,
                0,
                &responseHeaders,
                &statusCode));

        // statusCode can be 404 when pushing new project
        if (downloadStream == nullptr ||
            (statusCode != 200 && statusCode != 404))
        {
            this->setState(SyncThread::fetchHistoryError);
            return;
        }

        MemoryBlock fetchData;
        downloadStream->readIntoMemoryBlock(fetchData);
        //Logger::writeToLog(fetchData.toString());

        remoteXml = DataEncoder::createDecryptedXml(fetchData, this->localKey);

        const bool fileExists = (fetchData.getSize() != 0) && (statusCode != 404);
        const bool fileCanBeDecrypted = (remoteXml != nullptr);
        if (fileExists && !fileCanBeDecrypted)
        {
            Logger::writeToLog("Wrong key!");
            Supervisor::track(Serialization::Activities::vcsPushError);
            this->setState(SyncThread::fetchHistoryError);
            return;
        }
    }

    Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 350);


    //===------------------------------------------------------------------===//
    // Do some checks
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::merge);

    VersionControl localVCS(nullptr);
    localVCS.deserialize(*this->localXml);

    VersionControl remoteVCS(nullptr);

    if (remoteXml != nullptr)
    {
        remoteVCS.deserialize(*remoteXml);
    }
    else
    {
        remoteVCS.reset();
    }

    Logger::writeToLog("Local version: " + String(localVCS.getVersion()));
    Logger::writeToLog("Remote version: " + String(remoteVCS.getVersion()));
    Logger::writeToLog("Local hash: " + localVCS.calculateHash().toHexString());
    Logger::writeToLog("Remote hash: " + remoteVCS.calculateHash().toHexString());

    if (localVCS.getVersion() == remoteVCS.getVersion() &&
        localVCS.calculateHash() == remoteVCS.calculateHash())
    {
        this->setState(SyncThread::upToDate);
        return;
    }

    if ((localVCS.getVersion() > remoteVCS.getVersion()) ||
        (localVCS.getVersion() == remoteVCS.getVersion() && localVCS.calculateHash() != remoteVCS.calculateHash()))
    {
        Logger::writeToLog("Remote history is ok.");
    }
    else
    {
        Supervisor::track(Serialization::Activities::vcsMergeError);
        this->setState(SyncThread::mergeError);
        return;
    }


    //===------------------------------------------------------------------===//
    // Merge two trees
    //===------------------------------------------------------------------===//

    remoteVCS.mergeWith(localVCS);
    remoteVCS.incrementVersion();

    TemporaryFile tempFile("vcs");

    {
        ScopedPointer<XmlElement> xmlToPush(remoteVCS.serialize());
        MemoryBlock encryptedRemoteXml(DataEncoder::encryptXml(*xmlToPush, this->localKey));
        tempFile.getFile().replaceWithData(encryptedRemoteXml.getData(), encryptedRemoteXml.getSize());
    }

    // debug
    //ScopedPointer<XmlElement> localXmlForDebug(localVCS.serialize());
    //File debugFile(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("debug.vcs"));
    //debugFile.replaceWithText(localXmlForDebug->createDocument("", false, true, "UTF-8", 512));


    //===------------------------------------------------------------------===//
    // Push merged tree to the server
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::sync);

    URL pushUrl(this->url);
    pushUrl = pushUrl.withFileToUpload(Serialization::Network::file, tempFile.getFile(), "application/octet-stream");
    pushUrl = pushUrl.withParameter(Serialization::Network::key, keyHash);

    const bool loggedIn = (App::Helio()->getAuthManager()->getAuthorizationState() == AuthManager::LoggedIn);

    if (loggedIn)
    {
        const String deviceId(Config::getMachineId());
        const String obfustatedKey = DataEncoder::obfuscateString(this->localKey.toBase64Encoding());
        //Logger::writeToLog(">>" + obfustatedKey + "<<");
        pushUrl = pushUrl.withParameter(Serialization::Network::realKey, obfustatedKey);
        pushUrl = pushUrl.withParameter(Serialization::Network::title, this->title);
        pushUrl = pushUrl.withParameter(Serialization::Network::deviceId, deviceId);
    }


    pushUrl = pushUrl.withParameter(Serialization::Network::push, this->localId);
    pushUrl = pushUrl.withParameter(Serialization::Network::clientCheck, saltedIdHash);

    {
        int statusCode = 0;
        StringPairArray responseHeaders;

        ScopedPointer<InputStream> pushStream(
            pushUrl.createInputStream(true,
                syncProgressCallback,
                static_cast<void *>(this),
                HELIO_USERAGENT,
                0,
                &responseHeaders,
                &statusCode));

        if (pushStream == nullptr)
        {
            this->setState(SyncThread::syncError);
            return;
        }

        const String rawResult = pushStream->readEntireStreamAsString().trim();
        const String result = DataEncoder::deobfuscateString(rawResult);

        Logger::writeToLog("Upload, raw result: " + rawResult);
        Logger::writeToLog("Upload, result: " + result);

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
        else if (statusCode != 200)
        {
            this->setState(SyncThread::syncError);
            return;
        }
    }

    Supervisor::track(Serialization::Activities::vcsPush);
    this->setState(SyncThread::allDone);

    // On success we ask the auth manager to update his projects list.
    App::Helio()->getAuthManager()->requestSessionData();
}

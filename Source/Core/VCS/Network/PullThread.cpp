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
#include "PullThread.h"
#include "VersionControl.h"
#include "Client.h"
#include "DataEncoder.h"
#include "HelioServerDefines.h"
#include "SerializationKeys.h"

using namespace VCS;

PullThread::PullThread(URL pushUrl,
                       String projectId,
                       MemoryBlock projectKey,
                       ScopedPointer<XmlElement> pushContent) :
    SyncThread(pushUrl, projectId, projectKey, pushContent),
    mergedVCS(nullptr)
{
}

XmlElement *PullThread::createMergedStateData()
{
    jassert(this->mergedVCS);
    return this->mergedVCS->serialize();
}

void PullThread::run()
{
    const String saltedId = this->localId + HELIO_SALT;
    const String saltedIdHash = SHA256(saltedId.toUTF8()).toHexString();

    //===------------------------------------------------------------------===//
    // Fetch remote history
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::fetchHistory);

    ScopedPointer<XmlElement> remoteXml;

    URL fetchUrl(this->url);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::fetch, this->localId);
    fetchUrl = fetchUrl.withParameter(Serialization::Network::clientCheck, saltedIdHash);

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

        if (downloadStream == nullptr || statusCode != 200)
        {
            //Logger::writeToLog("downloadStream " + String(statusCode));
            //Logger::writeToLog(downloadStream->readEntireStreamAsString());
            this->setState(SyncThread::fetchHistoryError);
            return;
        }

        MemoryBlock fetchData;
        downloadStream->readIntoMemoryBlock(fetchData);
        remoteXml = DataEncoder::createDecryptedXml(fetchData, this->localKey);

        if (remoteXml == nullptr)
        {
            //const String obfustatedKey = DataEncoder::obfuscateString(this->localKey.toBase64Encoding());
            //Logger::writeToLog("Possible, wrong key: " + obfustatedKey);
            this->setState(SyncThread::fetchHistoryError);
            return;
        }
    }

    Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 350);


    //===------------------------------------------------------------------===//
    // Do some checks
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::merge);

    this->mergedVCS = new VersionControl(nullptr);
    this->mergedVCS->deserialize(*this->localXml);

    VersionControl remoteVCS(nullptr);

    if (remoteXml != nullptr)
    {
        remoteVCS.deserialize(*remoteXml);
    }

    Logger::writeToLog("Local version: " + String(this->mergedVCS->getVersion()));
    Logger::writeToLog("Remote version: " + String(remoteVCS.getVersion()));
    Logger::writeToLog("Local hash: " + this->mergedVCS->calculateHash().toHexString());
    Logger::writeToLog("Remote hash: " + remoteVCS.calculateHash().toHexString());

    // итак, пулл разрешен только если серверная версия больше.
    // если версии равны и равны хэши - up to date
    // остальное - ошибка.

    if (this->mergedVCS->getVersion() == remoteVCS.getVersion() &&
            this->mergedVCS->calculateHash() == remoteVCS.calculateHash())
    {
        this->setState(SyncThread::upToDate);
        return;
    }

    if (remoteVCS.getVersion() > this->mergedVCS->getVersion())
    {
        Logger::writeToLog("Remote history is ok.");
    }
    else
    {
        this->setState(SyncThread::mergeError);
        return;
    }


    //===------------------------------------------------------------------===//
    // Merge two trees
    //===------------------------------------------------------------------===//

    this->mergedVCS->mergeWith(remoteVCS);

    // debug
    //ScopedPointer<XmlElement> localXmlForDebug(this->mergedVCS->serialize());
    //File debugFile(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getChildFile("debug.vcs"));
    //debugFile.replaceWithText(localXmlForDebug->createDocument("", false, true, "UTF-8", 512));

    Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 350);
    this->setState(SyncThread::allDone);
}

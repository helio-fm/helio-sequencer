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
#include "SerializationKeys.h"

using namespace VCS;

PullThread::PullThread(URL pushUrl,
                       String projectId,
                       MemoryBlock projectKey,
                       const ValueTree &pushContent) :
    SyncThread(pushUrl, projectId, projectKey, pushContent),
    mergedVCS(nullptr) {}

ValueTree PullThread::createMergedStateData()
{
    jassert(this->mergedVCS);
    return this->mergedVCS->serialize();
}

void PullThread::run()
{
    const String saltedId = this->localId + "salt";
    const String saltedIdHash = SHA256(saltedId.toUTF8()).toHexString();

    //===------------------------------------------------------------------===//
    // Fetch remote history
    //===------------------------------------------------------------------===//

    this->setState(SyncThread::fetchHistory);

    ValueTree remoteState;

    URL fetchUrl(this->url);

    {
        int statusCode = 0;
        StringPairArray responseHeaders;

        ScopedPointer<InputStream> downloadStream(
            fetchUrl.createInputStream(true,
                syncProgressCallback,
                static_cast<void *>(this),
                "useragent",
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
        remoteState = {}; // FIXME DocumentHelpers::createDecryptedXml(fetchData, this->localKey);

        if (!remoteState.isValid())
        {
            //const String obfustatedKey = DocumentReader::obfuscateString(this->localKey.toBase64Encoding());
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
    this->mergedVCS->deserialize(this->localState);

    VersionControl remoteVCS(nullptr);

    if (remoteState.isValid())
    {
        remoteVCS.deserialize(remoteState);
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

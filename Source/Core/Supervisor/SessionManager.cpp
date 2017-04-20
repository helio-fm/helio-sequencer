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
#include "Supervisor.h"
#include "SessionManager.h"
#include "DataEncoder.h"
#include "HelioServerDefines.h"
#include "SerializationKeys.h"
#include "Config.h"

SessionManager::SessionManager()
    : Thread("Overseer")
{
}

SessionManager::~SessionManager()
{
    this->stopSubmit();
}

void SessionManager::addOrUpdateSession(const Session *session)
{
    const ScopedWriteLock lock(this->sessionsLock);

    // чтобы не захламлять диск и память
    if (this->sessions.size() > 100)
    {
        this->sessions.removeRange(0, this->sessions.size() - 100);
    }

    ScopedPointer<XmlElement> newData(session->serialize());

    bool alreadyExists = false;

    for (auto current : this->sessions)
    {
        if (current->getId() == session->getId())
        {
            alreadyExists = true;
            current->deserialize(*newData);
        }
    }

    if (!alreadyExists)
    {
        auto newSession = new Session();
        newSession->deserialize(*newData);
        this->sessions.add(newSession);
    }
}

void SessionManager::submitSessions()
{
    this->startThread(3);
}

void SessionManager::stopSubmit()
{
    if (this->isThreadRunning())
    {
        this->signalThreadShouldExit();
        this->waitForThreadToExit(200);
    }
}

//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

void SessionManager::run()
{
    const String deviceId(Config::getMachineId());
    const String saltedDeviceId = deviceId + HELIO_SALT;
    const String saltedDeviceIdHash = SHA256(saltedDeviceId.toUTF8()).toHexString();
    
    const ScopedReadLock threadSessionsLock(this->sessionsLock);

    for (int i = 0; i < this->sessions.size();)
    {
        if (this->threadShouldExit()) { return; }

        const Session *session = this->sessions[i];

        ScopedPointer<XmlElement> serializedSession(session->serialize());
        const String &sessionDataString = serializedSession->createDocument("");
        const String &sessionDataCompressed = DataEncoder::obfuscateString(sessionDataString);

        if (this->threadShouldExit()) { return; }

        URL statURL(HELIO_SUPERVISOR_URL);
        statURL = statURL.withParameter(Serialization::Network::deviceId, deviceId);
        statURL = statURL.withParameter(Serialization::Network::clientCheck, saltedDeviceIdHash);
        statURL = statURL.withParameter(Serialization::Network::data, sessionDataCompressed);

        int statusCode = 0;
        StringPairArray responseHeaders;
        ScopedPointer<InputStream> io(
                                      statURL.createInputStream(true, nullptr, nullptr,
                                                                HELIO_USERAGENT,
                                                                0, &responseHeaders, &statusCode));

        if (io == nullptr)
        {
            Logger::writeToLog("Failed to access stat server.");
            return;
        }
        
        const String response = io->readEntireStreamAsString();
        Logger::writeToLog("Stat server response: " + String(statusCode) + " " + response);

        // Done or duplicate entry conflict
        if (statusCode == 200 || statusCode == 409)
        {
            // delete session
            Logger::writeToLog("Session sent, deleting.");
            const ScopedWriteLock lock(this->sessionsLock);
            this->sessions.remove(i, true);
        }
        else
        {
            i++;
        }
    }
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *SessionManager::serialize() const
{
    const ScopedReadLock lock(this->sessionsLock);

    auto xml = new XmlElement(Serialization::Supervisor::sessionManager);

    for (auto session : this->sessions)
    {
        xml->addChildElement(session->serialize());
    }

    return xml;
}

void SessionManager::deserialize(const XmlElement &xml)
{
    const ScopedWriteLock lock(this->sessionsLock);

    this->reset();

    const XmlElement *mainSlot = xml.hasTagName(Serialization::Supervisor::sessionManager) ?
                                 &xml : xml.getChildByName(Serialization::Supervisor::sessionManager);

    if (mainSlot == nullptr) { return; }

    forEachXmlChildElementWithTagName(*mainSlot, child, Serialization::Supervisor::session)
    {
        auto newSession = new Session();
        newSession->deserialize(*child);
        this->sessions.add(newSession);
    }
}

void SessionManager::reset()
{
    this->sessions.clear();
}

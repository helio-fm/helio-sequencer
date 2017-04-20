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
#include "Session.h"
#include "SessionManager.h"
#include "FileUtils.h"
#include "DataEncoder.h"
#include "App.h"
#include "Config.h"
#include "SerializationKeys.h"

void Supervisor::track(const String &key)
{
    App::Helio()->getSupervisor()->trackActivity(key);
}

Supervisor::Supervisor()
{
    this->currentSession = new Session();
    this->sessionManager = new SessionManager();

    this->loadSessions();
    this->sessionManager->submitSessions();
}

Supervisor::~Supervisor()
{
    this->sessionManager->stopSubmit();

    {
        ScopedWriteLock lock(this->sessionLock);
        this->currentSession->finish();
    }

    this->sessionManager->addOrUpdateSession(this->currentSession);
    this->saveSessions();
}

void Supervisor::trackActivity(const String &key)
{
    ScopedWriteLock lock(this->sessionLock);
    this->currentSession->onActivity(key);
}

void Supervisor::trackException(const std::exception *e, const String &sourceFilename, int lineNumber)
{
    {
        ScopedWriteLock lock(this->sessionLock);
        this->currentSession->onException(e, sourceFilename, lineNumber);
    }
}

void Supervisor::trackCrash()
{
    this->sessionManager->stopSubmit();
    
    {
        ScopedWriteLock lock(this->sessionLock);
        this->currentSession->onCrash();
        this->currentSession->finish();
    }
    
    this->sessionManager->addOrUpdateSession(this->currentSession);
    this->saveSessions();
}

void Supervisor::loadSessions()
{
    const String lastSavedName = Config::get(Serialization::Supervisor::sessionsData);
    const File lastSavedFile(lastSavedName);

    if (lastSavedFile.existsAsFile())
    {
        ScopedPointer<XmlElement> xml(DataEncoder::loadObfuscated(lastSavedFile));
        
        if (xml != nullptr)
        {
            this->deserialize(*xml);
        }
    }
}

void Supervisor::saveSessions()
{
    File autosave =
        FileUtils::getConfigSlot("sessions.helio");

    if (!autosave.getFullPathName().isEmpty())
    {
        ScopedPointer<XmlElement> xml(this->serialize());
        DataEncoder::saveObfuscated(autosave, xml);
        Config::set(Serialization::Supervisor::sessionsData, autosave.getFullPathName());
        Logger::writeToLog("Supervisor::saveSessions done");
    }

}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Supervisor::serialize() const
{
    return this->sessionManager->serialize();
}

void Supervisor::deserialize(const XmlElement &xml)
{
    this->sessionManager->deserialize(xml);
}

void Supervisor::reset()
{
    this->sessionManager->reset();
}

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
#include "Session.h"
#include "App.h"
#include "DataEncoder.h"
#include "Config.h"
#include "SerializationKeys.h"

Session::Session()
{
    this->reset();
}

Session::~Session()
{
}

void Session::onActivity(const String &key)
{
    const bool contains = this->activities.contains(key);

    if (contains)
    {
        this->activities.set(key, this->activities[key] + 1);
    }
    else
    {
        this->activities.set(key, 1);
    }
}

void Session::onException(const std::exception *e, const String &sourceFilename, int lineNumber)
{
    this->lastException = "Unhandled exception (" + sourceFilename + ", line " + String(lineNumber) + "):\n";

    if (e != nullptr)
    {
        this->lastException += String(e->what());
    }
}

void Session::onCrash()
{
    this->backtrace = SystemStats::getStackBacktrace();
}

void Session::finish()
{
    this->endTime = App::getCurrentTime();
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Session::serialize() const
{
    auto xml = new XmlElement(Serialization::Supervisor::session);

    xml->setAttribute(Serialization::Supervisor::sessionId, this->uuid);
    xml->setAttribute(Serialization::Supervisor::sessionHWID, this->machineID);
    xml->setAttribute(Serialization::Supervisor::sessionStartTime, this->startTime);
    xml->setAttribute(Serialization::Supervisor::sessionEndTime, this->endTime);
    xml->setAttribute(Serialization::Supervisor::sessionOS, this->platform);
    xml->setAttribute(Serialization::Supervisor::sessionLocale, this->locale);
    xml->setAttribute(Serialization::Supervisor::sessionAppVersion, this->appVersion);
    xml->setAttribute(Serialization::Supervisor::sessionBacktrace, this->backtrace);
    xml->setAttribute(Serialization::Supervisor::sessionLastException, this->lastException);

    HashMap<String, int>::Iterator it(this->activities);

    while (it.next())
    {
        auto activity = new XmlElement(Serialization::Supervisor::activity);
        activity->setAttribute(Serialization::Supervisor::activityName, it.getKey());
        activity->setAttribute(Serialization::Supervisor::activityCount, it.getValue());
        xml->addChildElement(activity);
    }

    return xml;
}

void Session::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *mainSlot = xml.hasTagName(Serialization::Supervisor::session) ?
        &xml : xml.getChildByName(Serialization::Supervisor::session);

    if (mainSlot == nullptr) { return; }

    this->uuid = mainSlot->getStringAttribute(Serialization::Supervisor::sessionId);
    this->machineID = mainSlot->getStringAttribute(Serialization::Supervisor::sessionHWID);
    this->startTime = mainSlot->getStringAttribute(Serialization::Supervisor::sessionStartTime);
    this->endTime = mainSlot->getStringAttribute(Serialization::Supervisor::sessionEndTime);
    this->platform = mainSlot->getStringAttribute(Serialization::Supervisor::sessionOS);
    this->locale = mainSlot->getStringAttribute(Serialization::Supervisor::sessionLocale);
    this->appVersion = mainSlot->getStringAttribute(Serialization::Supervisor::sessionAppVersion);
    this->backtrace = mainSlot->getStringAttribute(Serialization::Supervisor::sessionBacktrace);
    this->lastException = mainSlot->getStringAttribute(Serialization::Supervisor::sessionLastException);

    forEachXmlChildElementWithTagName(*mainSlot, child, Serialization::Supervisor::activity)
    {
        const String &key = child->getStringAttribute(Serialization::Supervisor::activityName);
        const int count = child->getIntAttribute(Serialization::Supervisor::activityCount);
        this->activities.set(key, count);
    }
}

void Session::reset()
{
    const Uuid newId;
    this->uuid = newId.toString();
    this->machineID = Config::getMachineId();
    this->startTime = App::getCurrentTime();
    this->endTime = this->startTime;
    this->platform = SystemStats::getOperatingSystemName();
    this->locale = SystemStats::getUserRegion();
    this->appVersion = App::getAppReadableVersion();
    this->backtrace = String::empty;
    this->lastException = String::empty;
}

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
#include "Config.h"
#include "App.h"
#include "DataEncoder.h"
#include "FileUtils.h"
#include "SerializationKeys.h"

String Config::getMachineId()
{
    const String systemStats =
        SystemStats::getLogonName() +
        SystemStats::getComputerName() +
        SystemStats::getOperatingSystemName() +
        SystemStats::getCpuVendor();

    return MD5::fromUTF32(systemStats).toHexString();
}

bool Config::hasNewMachineId()
{
    return App::Helio()->getConfig()->machineIdChanged();
}

void Config::set(const Identifier &keyName, const var &value)
{
    App::Helio()->getConfig()->setValue(keyName.toString(), value);
}

void Config::set(const Identifier &keyName, const XmlElement *xml)
{
    App::Helio()->getConfig()->setValue(keyName.toString(), xml);
}

String Config::get(StringRef keyName, const String &defaultReturnValue /*= String::empty*/)
{
    return App::Helio()->getConfig()->getValue(keyName, defaultReturnValue);
}

bool Config::contains(StringRef keyName)
{
    return App::Helio()->getConfig()->containsKey(keyName);
}

XmlElement *Config::getXml(StringRef keyName)
{
    return App::Helio()->getConfig()->getXmlValue(keyName);
}

void Config::save(const String &key, const Serializable *serializer)
{
    App::Helio()->getConfig()->saveConfig(key, serializer);
}

void Config::load(const String &key, Serializable *serializer)
{
    App::Helio()->getConfig()->loadConfig(key, serializer);
}

Config::Config(const int millisecondsBeforeSaving) :
    fileLock("Config Lock"),
    needsWriting(false),
    saveTimeout(millisecondsBeforeSaving)
{
    // Deal with legacy settings file
    auto legacySettingsFile(FileUtils::getConfigSlot("helio.settings"));
    auto newSettingsFile(FileUtils::getConfigSlot("settings.helio"));

    if (legacySettingsFile.existsAsFile())
    {
        legacySettingsFile.moveFileTo(newSettingsFile);
    }

    this->propertiesFile = newSettingsFile;
    this->reload();
}

Config::~Config()
{
    this->setValue(Serialization::Core::machineID, this->getMachineId());
    this->saveIfNeeded();
}

bool Config::machineIdChanged()
{
    const String storedID = this->getValue(Serialization::Core::machineID);
    const String currentID = this->getMachineId();
    Logger::writeToLog("Config::machineIdChanged " + storedID + " : " + currentID);
    return (storedID != currentID);
}

bool Config::saveIfNeeded()
{
    if (this->propertiesFile.getFullPathName().isEmpty())
    {
        return false;
    }

    Logger::writeToLog("Config::saveIfNeeded - " + this->propertiesFile.getFullPathName());

    XmlElement doc(Serialization::Core::globalConfig);

    for (int i = 0; i < getAllProperties().size(); ++i)
    {
        XmlElement *const e = doc.createNewChildElement(Serialization::Core::valueTag);
        e.setProperty(Serialization::Core::nameAttribute, getAllProperties().getAllKeys() [i]);

        // if the value seems to contain xml, store it as such..
        if (XmlElement *const childElement = XmlDocument::parse(getAllProperties().getAllValues()[i]))
        {
            e.appendChild(childElement);
        }
        else
        {
            e.setProperty(Serialization::Core::valueAttribute,
                            getAllProperties().getAllValues() [i]);
        }
    }


    InterProcessLock::ScopedLockType fLock(this->fileLock);

    if (!fLock.isLocked())
    {
        Logger::writeToLog("Config !fLock.isLocked()");
        return false;
    }

    if (DataEncoder::saveObfuscated(this->propertiesFile, &doc))
    {
        needsWriting = false;
        return true;
    }

    return false;
}

bool Config::reload()
{
    if (!this->propertiesFile.existsAsFile())
    {
        return false;
    }

    Logger::writeToLog("Config::reload - " + this->propertiesFile.getFullPathName());

    InterProcessLock::ScopedLockType fLock(this->fileLock);

    ScopedPointer<XmlElement> doc(DataEncoder::loadObfuscated(this->propertiesFile));

    if (doc != nullptr)
    {
        if (doc->hasTagName(Serialization::Core::globalConfig))
        {
            forEachValueTreeChildWithType(doc, e, Serialization::Core::valueTag)
            {
                const String name(e.getProperty(Serialization::Core::nameAttribute));

                if (name.isNotEmpty())
                {
                    getAllProperties().set(name,
                                           e.getChild(0) != nullptr
                                           ? e.getChild(0)->createDocument(String::empty, true)
                                           : e.getProperty(Serialization::Core::valueAttribute));
                }
            }

            return true;
        }
    }

    return false;
}


void Config::timerCallback()
{
    this->saveIfNeeded();
    this->stopTimer();
}

void Config::saveConfig(const String &key, const Serializable *serializer)
{
    const auto value(serializer->serialize());
    // TODO serialize somehow
    this->setValue(key, serialized);
}

void Config::loadConfig(const String &key, Serializable *serializer)
{
    ScopedPointer<XmlElement> xml(this->getXmlValue(key));

    if (xml)
    {
        serializer->deserialize(*xml);
    }
}

void Config::propertyChanged()
{
    this->needsWriting = true;

    if (this->saveTimeout > 0)
    {
        this->startTimer(this->saveTimeout);
    }
    else if (this->saveTimeout == 0)
    {
        this->saveIfNeeded();
    }
}

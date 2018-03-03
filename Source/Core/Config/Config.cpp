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
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "SerializationKeys.h"

String Config::getDeviceId()
{
    const String systemStats =
        SystemStats::getLogonName() +
        SystemStats::getComputerName() +
        SystemStats::getOperatingSystemName() +
        SystemStats::getCpuVendor();

    return MD5::fromUTF32(systemStats).toHexString();
}

void Config::set(const Identifier &key, const var &value)
{
    App::Config().setProperty(key, value);
}

String Config::get(const Identifier &key, const String &defaultReturnValue /*= String::empty*/)
{
    return App::Config().getProperty(key, defaultReturnValue);
}

bool Config::contains(const Identifier &key)
{
    return App::Config().containsPropertyOrChild(key);
}

void Config::save(const Identifier &key, const Serializable *serializer)
{
    App::Config().saveConfigFor(key, serializer);
}

void Config::load(const Identifier &key, Serializable *serializer)
{
    App::Config().loadConfigFor(key, serializer);
}

Config::Config(int timeoutToSaveMs) :
    fileLock("Config Lock"),
    needsSaving(false),
    saveTimeout(timeoutToSaveMs)
{
    this->config = ValueTree(Serialization::Core::globalConfig);
    this->propertiesFile = DocumentHelpers::getConfigSlot("settings.helio");
    this->reload();
}

Config::~Config()
{
    this->setProperty(Serialization::Core::machineID, this->getDeviceId());
    this->saveIfNeeded();
}

bool Config::saveIfNeeded()
{
    if (this->propertiesFile.getFullPathName().isEmpty())
    {
        return false;
    }

    Logger::writeToLog("Config::saveIfNeeded - " + this->propertiesFile.getFullPathName());

    InterProcessLock::ScopedLockType fLock(this->fileLock);
    if (!fLock.isLocked())
    {
        Logger::writeToLog("Config !fLock.isLocked()");
        return false;
    }

    if (DocumentHelpers::save<XmlSerializer>(this->propertiesFile, this->config))
    {
        needsSaving = false;
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

    const ValueTree doc(DocumentHelpers::load<XmlSerializer>(this->propertiesFile));

    if (doc.isValid() && doc.hasType(Serialization::Core::globalConfig))
    {
        this->config = doc;
        return true;
    }

    return false;
}

void Config::timerCallback()
{
    this->saveIfNeeded();
    this->stopTimer();
}

void Config::saveConfigFor(const Identifier &key, const Serializable *serializable)
{
    const ValueTree child(this->config.getChildWithName(key));
    this->config.removeChild(child, nullptr);
    this->config.appendChild(serializable->serialize(), nullptr);
    this->onConfigChanged();
}

void Config::loadConfigFor(const Identifier &key, Serializable *serializable)
{
    const auto tree(this->config.getChildWithName(key));
    if (tree.isValid())
    {
        serializable->deserialize(tree);
    }
}

void Config::setProperty(const Identifier &key, const var &value)
{
    this->config.setProperty(key, value, nullptr);
    this->onConfigChanged();
}

String Config::getProperty(const Identifier &key, const String &fallback) const noexcept
{
    return this->config.getProperty(key, fallback);
}

bool Config::containsPropertyOrChild(const Identifier &key) const noexcept
{
    return this->config.hasProperty(key) || this->config.getChildWithName(key).isValid();
}

void Config::onConfigChanged()
{
    this->needsSaving = true;

    if (this->saveTimeout > 0)
    {
        this->startTimer(this->saveTimeout);
    }
    else if (this->saveTimeout == 0)
    {
        this->saveIfNeeded();
    }
}

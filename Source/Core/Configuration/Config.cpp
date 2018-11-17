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
    static String kDeviceId;

    if (kDeviceId.isEmpty())
    {
        const auto &ids = SystemStats::getDeviceIdentifiers();
        if (!ids.isEmpty())
        {
            kDeviceId = String(CompileTimeHash(ids.joinIntoString({}).toUTF8()));
        }
        else
        {
            const String systemStats =
                SystemStats::getLogonName() +
                SystemStats::getComputerName() +
                SystemStats::getOperatingSystemName() +
                SystemStats::getCpuVendor();

            kDeviceId = String(CompileTimeHash(systemStats.toUTF8()));
        }
    }

    return kDeviceId;
}

void Config::set(const Identifier &key, const var &value, bool delayedSave)
{
    App::Config().setProperty(key, value, delayedSave);
}

String Config::get(const Identifier &key, const String &defaultReturnValue)
{
    return App::Config().getProperty(key, defaultReturnValue);
}

bool Config::contains(const Identifier &key)
{
    return App::Config().containsPropertyOrChild(key);
}

void Config::save(const Serializable *serializer, const Identifier &key)
{
    App::Config().saveConfigFor(key, serializer);
}

void Config::save(const Serializable &serializer, const Identifier &key)
{
    App::Config().saveConfigFor(key, &serializer);
}

void Config::load(Serializable *serializer, const Identifier &key)
{
    App::Config().loadConfigFor(key, serializer);
}

void Config::load(Serializable &serializer, const Identifier &key)
{
    App::Config().loadConfigFor(key, &serializer);
}

Config::Config(int timeoutToSaveMs) :
    fileLock("Config Lock"),
    needsSaving(false),
    saveTimeout(timeoutToSaveMs)
{
    this->propertiesFile = DocumentHelpers::getConfigSlot("settings.helio");
    this->reload();
}

Config::~Config()
{
    this->saveIfNeeded();
}

bool Config::saveIfNeeded()
{
    if (this->propertiesFile.getFullPathName().isEmpty())
    {
        return false;
    }

    InterProcessLock::ScopedLockType fLock(this->fileLock);
    if (!fLock.isLocked())
    {
        Logger::writeToLog("Config !fLock.isLocked()");
        return false;
    }

    // make sure only the used properties are saved
    ValueTree cleanedUpConfig(Serialization::Core::globalConfig);
    for (const auto &i : this->properties)
    {
        if (this->usedKeys.contains(i.first))
        {
            cleanedUpConfig.setProperty(i.first, i.second, nullptr);
        }
    }

    for (const auto &i : this->children)
    {
        if (this->usedKeys.contains(i.first))
        {
            cleanedUpConfig.appendChild(i.second, nullptr);
        }
    }

    if (DocumentHelpers::save<XmlSerializer>(this->propertiesFile, cleanedUpConfig))
    {
        needsSaving = false;
        Logger::writeToLog("Config saved: " + this->propertiesFile.getFullPathName());
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

    InterProcessLock::ScopedLockType fLock(this->fileLock);

    const ValueTree doc(DocumentHelpers::load<XmlSerializer>(this->propertiesFile));

    if (doc.isValid() && doc.hasType(Serialization::Core::globalConfig))
    {
        this->children.clear();
        this->properties.clear();

        for (int i = 0; i < doc.getNumProperties(); ++i)
        {
            const auto key(doc.getPropertyName(i));
            this->properties[key] = doc[key];
        }

        for (int i = 0; i < doc.getNumChildren(); ++i)
        {
            const auto child(doc.getChild(i));
            this->children[child.getType()] = child;
        }

        Logger::writeToLog("Config reloaded");
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
    this->usedKeys.emplace(key);

    ValueTree root(key);
    root.appendChild(serializable->serialize(), nullptr);

    this->children[key] = root;
    this->onConfigChanged();
}

void Config::loadConfigFor(const Identifier &key, Serializable *serializable)
{
    this->usedKeys.emplace(key);

    const auto found = this->children.find(key);
    if (found == this->children.end())
    {
        return;
    }

    const auto tree = found->second;
    if (tree.isValid() && tree.getChild(0).isValid())
    {
        serializable->deserialize(tree.getChild(0));
    }
}

void Config::setProperty(const Identifier &key, const var &value, bool delayedSave)
{
    this->usedKeys.emplace(key);
    this->properties[key] = value;
    if (delayedSave)
    {
        this->onConfigChanged();
    }
}

String Config::getProperty(const Identifier &key, const String &fallback) const noexcept
{
    this->usedKeys.emplace(key.toString());
    const auto found = this->properties.find(key);
    return (found == this->properties.end()) ? fallback : found->second.toString();
}

bool Config::containsPropertyOrChild(const Identifier &key) const noexcept
{
    return this->properties.contains(key) || this->children.contains(key);
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

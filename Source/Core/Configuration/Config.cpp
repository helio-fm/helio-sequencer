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
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "SerializationKeys.h"

Config::Config(int timeoutToSaveMs) :
    fileLock("Config Lock"),
    needsSaving(false),
    saveTimeout(timeoutToSaveMs),
    propertiesFile(DocumentHelpers::getConfigSlot("settings.helio"))
{
    this->translationsManager.reset(new TranslationsManager());
    this->arpeggiatorsManager.reset(new ArpeggiatorsManager());
    this->colourSchemesManager.reset(new ColourSchemesManager());
    this->hotkeySchemesManager.reset(new HotkeySchemesManager());
    this->scriptsManager.reset(new ScriptsManager());
    this->scalesManager.reset(new ScalesManager());
    this->chordsManager.reset(new ChordsManager());

    using namespace Serialization::Resources;
    this->resources[translations] = this->translationsManager.get();
    this->resources[arpeggiators] = this->arpeggiatorsManager.get();
    this->resources[colourSchemes] = this->colourSchemesManager.get();
    this->resources[hotkeySchemes] = this->hotkeySchemesManager.get();
    this->resources[scripts] = this->scriptsManager.get();
    this->resources[scales] = this->scalesManager.get();
    this->resources[chords] = this->chordsManager.get();
}

Config::~Config()
{
    this->saveIfNeeded();

    this->chordsManager = nullptr;
    this->scalesManager = nullptr;
    this->scriptsManager = nullptr;
    this->hotkeySchemesManager = nullptr;
    this->colourSchemesManager = nullptr;
    this->arpeggiatorsManager = nullptr;
    this->translationsManager = nullptr;
    this->resources.clear();
}

void Config::initResources()
{
    if (this->propertiesFile.existsAsFile())
    {
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

            DBG("Config reloaded");
        }
    }

    for (auto manager : this->resources)
    {
        manager.second->reloadResources();
    }
}

void Config::save(const Serializable *serializable, const Identifier &key)
{
    this->usedKeys.emplace(key);

    ValueTree root(key);
    root.appendChild(serializable->serialize(), nullptr);

    this->children[key] = root;
    this->onConfigChanged();
}

void Config::load(Serializable *serializable, const Identifier &key)
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

bool Config::containsProperty(const Identifier &key) const noexcept
{
    return this->properties.contains(key) || this->children.contains(key);
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
        DBG("Config !fLock.isLocked()");
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
        DBG("Config saved: " + this->propertiesFile.getFullPathName());
        return true;
    }

    return false;
}

void Config::timerCallback()
{
    this->saveIfNeeded();
    this->stopTimer();
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

ResourceManagerLookup &Config::getAllResources() noexcept
{
    return this->resources;
}

ChordsManager *Config::getChords() const noexcept
{
    return this->chordsManager.get();
}

ScalesManager *Config::getScales() const noexcept
{
    return this->scalesManager.get();
}

ScriptsManager *Config::getScripts() const noexcept
{
    return this->scriptsManager.get();
}

TranslationsManager *Config::getTranslations() const noexcept
{
    return this->translationsManager.get();
}

ArpeggiatorsManager *Config::getArpeggiators() const noexcept
{
    return this->arpeggiatorsManager.get();
}

ColourSchemesManager *Config::getColourSchemes() const noexcept
{
    return this->colourSchemesManager.get();
}

HotkeySchemesManager *Config::getHotkeySchemes() const noexcept
{
    return this->hotkeySchemesManager.get();
}

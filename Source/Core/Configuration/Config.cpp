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
    saveTimeout(timeoutToSaveMs),
    propertiesFile(DocumentHelpers::getConfigSlot("settings.helio"))
{
    this->translationsCollection = make<TranslationsCollection>();
    this->arpeggiatorsCollection = make<ArpeggiatorsCollection>();
    this->colourSchemesCollection = make<ColourSchemesCollection>();
    this->hotkeySchemesCollection = make<HotkeySchemesCollection>();
    this->temperamentsCollection = make<TemperamentsCollection>();
    this->keyboardMappingsCollection = make<KeyboardMappingsCollection>();
    this->scalesCollection = make<ScalesCollection>();
    this->chordsCollection = make<ChordsCollection>();

    using namespace Serialization::Resources;
    this->resources[translations] = this->translationsCollection.get();
    this->resources[arpeggiators] = this->arpeggiatorsCollection.get();
    this->resources[colourSchemes] = this->colourSchemesCollection.get();
    this->resources[hotkeySchemes] = this->hotkeySchemesCollection.get();
    this->resources[temperaments] = this->temperamentsCollection.get();
    this->resources[keyboardMappings] = this->keyboardMappingsCollection.get();
    this->resources[scales] = this->scalesCollection.get();
    this->resources[chords] = this->chordsCollection.get();

    this->uiFlags = make<UserInterfaceFlags>();
}

Config::~Config()
{
    this->saveIfNeeded();
}

void Config::initResources()
{
    if (this->propertiesFile.existsAsFile())
    {
        InterProcessLock::ScopedLockType fLock(this->fileLock);

        const auto doc = DocumentHelpers::load<XmlSerializer>(this->propertiesFile);
        if (doc.isValid() && doc.hasType(Serialization::Core::globalConfig))
        {
            this->children.clear();
            this->properties.clear();

            for (int i = 0; i < doc.getNumProperties(); ++i)
            {
                const auto key(doc.getPropertyName(i));
                this->properties[key] = doc.getProperty(key);
            }

            for (int i = 0; i < doc.getNumChildren(); ++i)
            {
                const auto child(doc.getChild(i));
                this->children[child.getType()] = child;
            }

            DBG("Config reloaded");
        }
    }

    for (auto &manager : this->resources)
    {
        manager.second->reloadResources();
    }

    this->load(this->uiFlags.get(), Serialization::Config::activeUiFlags);
}

void Config::save(const Serializable *serializable, const Identifier &key)
{
    SerializedData root(key);
    root.appendChild(serializable->serialize());

    this->children[key] = root;
    this->onConfigChanged();
}

void Config::load(Serializable *serializable, const Identifier &key)
{
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
    this->properties[key] = value;
    if (delayedSave)
    {
        this->onConfigChanged();
    }
}

String Config::getProperty(const Identifier &key, const String &fallback) const noexcept
{
    const auto found = this->properties.find(key);
    return (found == this->properties.end()) ? fallback : found->second.toString();
}

bool Config::containsProperty(const Identifier &key) const noexcept
{
    return this->properties.contains(key) || this->children.contains(key);
}

bool Config::saveIfNeeded()
{
    if (!this->needsSaving)
    {
        return false;
    }

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

    SerializedData configNode(Serialization::Core::globalConfig);
    for (const auto &i : this->properties)
    {
        configNode.setProperty(i.first, i.second);
    }

    for (const auto &i : this->children)
    {
        configNode.appendChild(i.second);
    }

    if (DocumentHelpers::save<XmlSerializer>(this->propertiesFile, configNode))
    {
        this->needsSaving = false;
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

ResourceCollectionsLookup &Config::getAllResources() noexcept
{
    return this->resources;
}

ChordsCollection *Config::getChords() const noexcept
{
    return this->chordsCollection.get();
}

ScalesCollection *Config::getScales() const noexcept
{
    return this->scalesCollection.get();
}

TranslationsCollection *Config::getTranslations() const noexcept
{
    return this->translationsCollection.get();
}

ArpeggiatorsCollection *Config::getArpeggiators() const noexcept
{
    return this->arpeggiatorsCollection.get();
}

ColourSchemesCollection *Config::getColourSchemes() const noexcept
{
    return this->colourSchemesCollection.get();
}

HotkeySchemesCollection *Config::getHotkeySchemes() const noexcept
{
    return this->hotkeySchemesCollection.get();
}

TemperamentsCollection *Config::getTemperaments() const noexcept
{
    return this->temperamentsCollection.get();
}

KeyboardMappingsCollection *Config::getKeyboardMappings() const noexcept
{
    return this->keyboardMappingsCollection.get();
}

UserInterfaceFlags *Config::getUiFlags() const noexcept
{
    return this->uiFlags.get();
}

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

#pragma once

class Serializable;

#include "TranslationsManager.h"
#include "ArpeggiatorsManager.h"
#include "ColourSchemesManager.h"
#include "HotkeySchemesManager.h"
#include "ScriptsManager.h"
#include "ScalesManager.h"
#include "ChordsManager.h"

class Config final : private Timer
{
public:

    explicit Config(int timeoutToSaveMs = 10000);
    ~Config() override;

    void initResources();

    void save(const Serializable *serializable, const Identifier &key);
    void load(Serializable *serializable, const Identifier &key);

    void setProperty(const Identifier &key, const var &value, bool delayedSave = true);
    String getProperty(const Identifier &key, const String &fallback = {}) const noexcept;
    bool containsProperty(const Identifier &key) const noexcept;

    ChordsManager *getChords() const noexcept;
    ScalesManager *getScales() const noexcept;
    ScriptsManager *getScripts() const noexcept;
    TranslationsManager *getTranslations() const noexcept;
    ArpeggiatorsManager *getArpeggiators() const noexcept;
    ColourSchemesManager *getColourSchemes() const noexcept;
    HotkeySchemesManager *getHotkeySchemes() const noexcept;

    ResourceManagerLookup &getAllResources() noexcept;

private:

    void onConfigChanged();
    bool saveIfNeeded();

    void timerCallback() override;

    InterProcessLock fileLock;
    File propertiesFile;
    
    FlatHashMap<Identifier, var, IdentifierHash> properties;
    FlatHashMap<Identifier, ValueTree, IdentifierHash> children;

    // As the app development moves forward, some properties
    // become deprecated, but they will still present in config file,
    // so we need to track the unused ones and never save them:
    mutable FlatHashSet<Identifier, IdentifierHash> usedKeys;

    UniquePointer<TranslationsManager> translationsManager;
    UniquePointer<ArpeggiatorsManager> arpeggiatorsManager;
    UniquePointer<ColourSchemesManager> colourSchemesManager;
    UniquePointer<HotkeySchemesManager> hotkeySchemesManager;
    UniquePointer<ScriptsManager> scriptsManager;
    UniquePointer<ScalesManager> scalesManager;
    UniquePointer<ChordsManager> chordsManager;

    ResourceManagerLookup resources;

    bool needsSaving;
    int saveTimeout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};

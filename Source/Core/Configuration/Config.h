/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class Serializable;

#include "TranslationsCollection.h"
#include "ArpeggiatorsCollection.h"
#include "ColourSchemesCollection.h"
#include "HotkeySchemesCollection.h"
#include "TemperamentsCollection.h"
#include "KeyboardMappingsCollection.h"
#include "ScalesCollection.h"
#include "ChordsCollection.h"
#include "MetersCollection.h"
#include "UserInterfaceFlags.h"

class Config final : private Timer
{
public:

    Config();
    ~Config() override;

    void initResources();

    void save(const Serializable *serializable, const Identifier &key);
    void load(Serializable *serializable, const Identifier &key);

    void setProperty(const Identifier &key, const var &value, bool delayedSave = true);
    var getProperty(const Identifier &key, const var &fallback = {}) const noexcept;
    bool containsProperty(const Identifier &key) const noexcept;

    // some shortcuts for setProperty/getProperty

    void setMaxSavedUndoActions(int value);
    int getMaxSavedUndoActions() const noexcept;

    void setWindowBounds(const Rectangle<int> &bounds);
    Optional<Rectangle<int>> getWindowBounds() const noexcept;
    void setWindowMaximised(bool isMaximised);
    bool isWindowMaximised() const noexcept;

    // resource collections

    ChordsCollection *getChords() const noexcept;
    ScalesCollection *getScales() const noexcept;
    MetersCollection *getMeters() const noexcept;
    TemperamentsCollection *getTemperaments() const noexcept;
    TranslationsCollection *getTranslations() const noexcept;
    ArpeggiatorsCollection *getArpeggiators() const noexcept;
    ColourSchemesCollection *getColourSchemes() const noexcept;
    HotkeySchemesCollection *getHotkeySchemes() const noexcept;
    KeyboardMappingsCollection *getKeyboardMappings() const noexcept;

    ResourceCollectionsLookup &getAllResources() noexcept;

    UserInterfaceFlags *getUiFlags() const noexcept;

private:

    void onConfigChanged();
    bool saveIfNeeded();

    void timerCallback() override;

    InterProcessLock fileLock;
    File propertiesFile;
    
    FlatHashMap<Identifier, var, IdentifierHash> properties;
    FlatHashMap<Identifier, SerializedData, IdentifierHash> children;

    UniquePointer<TranslationsCollection> translationsCollection;
    UniquePointer<ArpeggiatorsCollection> arpeggiatorsCollection;
    UniquePointer<ColourSchemesCollection> colourSchemesCollection;
    UniquePointer<HotkeySchemesCollection> hotkeySchemesCollection;
    UniquePointer<TemperamentsCollection> temperamentsCollection;
    UniquePointer<KeyboardMappingsCollection> keyboardMappingsCollection;
    UniquePointer<ScalesCollection> scalesCollection;
    UniquePointer<ChordsCollection> chordsCollection;
    UniquePointer<MetersCollection> metersCollection;

    ResourceCollectionsLookup resources;

    UniquePointer<UserInterfaceFlags> uiFlags;

    bool needsSaving = false;

#if PLATFORM_DESKTOP
    const int saveTimeout = 30000;
#elif PLATFORM_MOBILE
    const int saveTimeout = 0;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};

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

#include "Translation.h"
#include "ConfigurationResourceCollection.h"

class TranslationsCollection final : public ConfigurationResourceCollection
{
public:

    TranslationsCollection();
    ~TranslationsCollection() override;

    inline const Array<Translation::Ptr> getAll() const
    {
        return this->getAllResources<Translation>();
    }

    const Translation::Ptr getCurrent() const noexcept;

    void loadLocaleWithId(const String &localeId);

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//
    
    String translate(I18n::Key singular);
    String translate(const String &text);
    String translate(const String &baseLiteral, int64 targetNumber);
    
private:

    void deserializeResources(const SerializedData &tree, Resources &outResources) override;
    void reset() override;

    UniquePointer<JavascriptEngine> engine;
    String equationResult;

    SpinLock currentTranslationLock;
    Translation::Ptr currentTranslation;

    String getSelectedLocaleId() const;
    friend struct PluralEquationWrapper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TranslationsCollection)
};

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

#include "Translation.h"
#include "ResourceManager.h"

class TranslationsManager final : public ResourceManager
{
public:

    static TranslationsManager &getInstance()
    {
        static TranslationsManager Instance;
        return Instance;
    }

    void initialise() override;
    void shutdown() override;

    inline const Array<Translation::Ptr> getAvailableLocales() const
    {
        return this->getResources<Translation::Ptr>();
    }

    const Translation::Ptr getCurrentLocale() const noexcept;

    void loadLocaleWithName(const String &localeName);

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//
    
    String translate(const String &text);
    String translate(const String &text, const String &resultIfNotFound);
    String translate(const String &baseLiteral, int64 targetNumber);
    
private:
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:
        
    TranslationsManager();

    ScopedPointer<JavascriptEngine> engine;
    String equationResult;

    SpinLock currentTranslationLock;
    Translation::Ptr currentTranslation;
    
    String getSelectedLocaleId() const;

    friend struct PluralEquationWrapper;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TranslationsManager)

};

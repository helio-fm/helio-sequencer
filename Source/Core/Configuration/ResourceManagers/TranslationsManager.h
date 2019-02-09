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

    TranslationsManager();
    ~TranslationsManager() override;

    BaseResource::Ptr createResource() const override
    {
        return { new Translation() };
    }

    inline const Array<Translation::Ptr> getAll() const
    {
        return this->getAllResources<Translation>();
    }

    const Translation::Ptr getCurrent() const noexcept;

    void loadLocaleWithId(const String &localeId);

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//
    
    String translate(const String &text);
    String translate(const String &baseLiteral, int64 targetNumber);
    
private:

    void deserializeResources(const ValueTree &tree, Resources &outResources) override;
    void reset() override;

    ScopedPointer<JavascriptEngine> engine;
    String equationResult;

    SpinLock currentTranslationLock;
    Translation::Ptr currentTranslation;
    Translation::Ptr fallbackTranslation;

    String getSelectedLocaleId() const;
    friend struct PluralEquationWrapper;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TranslationsManager)
};

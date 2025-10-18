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

#include "Common.h"
#include "TranslationsCollection.h"
#include "SerializationKeys.h"
#include "Config.h"

TranslationsCollection::TranslationsCollection() :
    ConfigurationResourceCollection(Serialization::Resources::translations) {}

//===----------------------------------------------------------------------===//
// Translations
//===----------------------------------------------------------------------===//

const Translation::Ptr TranslationsCollection::getCurrent() const noexcept
{
    return this->currentTranslation;
}

void TranslationsCollection::loadLocaleWithId(const String &localeId)
{
    if (this->currentTranslation->id == localeId)
    {
        DBG(localeId + "translation is already loaded, skipping");
        return;
    }

    App::Config().setProperty(Serialization::Config::currentLocale, localeId);
    this->reloadResources();

    if (const auto translation = this->getResourceById<Translation>(localeId))
    {
        this->currentTranslation = translation;
        this->sendChangeMessage();
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

String TranslationsCollection::translate(const String &text)
{
    const auto translation = translate(constexprHash(text.getCharPointer()));
    return translation.isNotEmpty() ? translation : text;
}

String TranslationsCollection::translate(I18n::Key key)
{
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);

    const auto foundCurrentSingular = this->currentTranslation->singulars.find(key);
    if (foundCurrentSingular != this->currentTranslation->singulars.end())
    {
        return foundCurrentSingular->second;
    }

    return {};
}

// in previous versions, translation updates were downloaded in the runtime,
// and each translation had a plurals formula as text, evaluated by JUCE's JS engine;
// nowadays, the app is offline and all translations are built-in,
// and using the JS engine seems like an overkill, let's just hard-code it all
static int getPluralForm(const String &locale, int64 x) noexcept
{
    if (locale == "en" || locale == "de" || locale == "el" || locale == "es" ||
        locale == "it" || locale == "ko" || locale == "af" || locale == "nl")
    {
        return x == 1 ? 1 : 2;
    }
    else if (locale == "zh" || locale == "ja")
    {
        return 1;
    }
    else if (locale == "fr" || locale == "pt" || locale == "tr")
    {
        return (x == 0 || x == 1) ? 1 : 2;
    }
    else if (locale == "ru" || locale == "uk")
    {
        return x % 10 == 1 && x % 100 != 11 ? 1 :
            x % 10 >= 2 && x % 10 <= 4 && (x % 100 < 10 || x % 100 >= 20) ? 2 : 3;
    }
    else if (locale == "pl")
    {
        return x == 1 ? 1 :
            x % 10 >= 2 && x % 10 <= 4 && (x % 100 < 10 || x % 100 >= 20) ? 2 : 3;
    }

    return 1;
}

String TranslationsCollection::translate(const String &baseLiteral, int64 targetNumber)
{
    if (baseLiteral.isEmpty())
    {
        return {};
    }

    using namespace Serialization;
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);

    const auto literalKey = constexprHash(baseLiteral.getCharPointer());
    const auto foundPlural = this->currentTranslation->plurals.find(literalKey);
    if (foundPlural == this->currentTranslation->plurals.end())
    {
        return baseLiteral.replace(Translations::metaSymbol, String(targetNumber));
    }

    const String pluralForm(getPluralForm(this->currentTranslation->id, targetNumber));
    const auto foundTranslation = foundPlural->second->find(pluralForm);
    if (foundTranslation != foundPlural->second->end())
    {
        return foundTranslation->second.replace(Translations::metaSymbol, String(targetNumber));
    }

    return baseLiteral.replace(Translations::metaSymbol, String(targetNumber));
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

const static String fallbackTranslationId = "en";

void TranslationsCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::translations) ?
        tree : tree.getChildWithName(Serialization::Resources::translations);

    if (!root.isValid()) { return; }

    const String selectedLocaleId = this->getSelectedLocaleId();

    Translation::Ptr fallbackTranslation;

    forEachChildWithType(root, translationRoot, Serialization::Translations::locale)
    {
        // if the existing translation for locale id is found,
        // just extend it, otherwise some new keys may be missing:
        const auto translationId =
            translationRoot.getProperty(Serialization::Translations::localeId).toString().toLowerCase();
        const auto existingTranslation = this->getResourceById(translationId);

        Translation::Ptr translation(existingTranslation != nullptr ?
            static_cast<Translation *>(existingTranslation.get()) : new Translation());

        //DBG(translationId + "/" + translation->getResourceId());
        translation->deserialize(translationRoot);

        outResources[translation->getResourceId()] = translation;

        if (translation->id == selectedLocaleId)
        {
            this->currentTranslation = translation;
        }
        else if (translation->id == fallbackTranslationId)
        {
            fallbackTranslation = translation;
        }
    }

    if (this->currentTranslation == nullptr)
    {
        this->currentTranslation = fallbackTranslation;
    }

    if (this->currentTranslation->isEmpty()) // only found the description?
    {
        int dataSize;
        const String builtInTranslationName = this->currentTranslation->getId() + "_json";
        if (const auto *data = BinaryData::getNamedResource(builtInTranslationName.toUTF8(), dataSize))
        {
            const String translationDataString(String::fromUTF8(data, dataSize));
            if (translationDataString.isNotEmpty())
            {
                const auto translationData(DocumentHelpers::load(translationDataString));
                jassert(translationData.isValid());
                this->currentTranslation->deserialize(translationData);
            }
        }
    }

    jassert(this->currentTranslation != nullptr && !this->currentTranslation->isEmpty());
}

void TranslationsCollection::reset()
{
    ConfigurationResourceCollection::reset();
    this->currentTranslation = nullptr;
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

String TranslationsCollection::getSelectedLocaleId() const
{
    if (App::Config().containsProperty(Serialization::Config::currentLocale))
    {
        return App::Config().getProperty(Serialization::Config::currentLocale, fallbackTranslationId);
    }
    
    const String systemLocale =
        SystemStats::getUserLanguage().toLowerCase().substring(0, 2);
    
    if (this->userResources.contains(systemLocale) ||
        this->baseResources.contains(systemLocale))
    {
        return systemLocale;
    }
    
    return fallbackTranslationId;
}

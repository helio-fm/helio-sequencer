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
#include "TranslationsManager.h"
#include "SerializationKeys.h"
#include "Config.h"
#include "BinaryData.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "JsonSerializer.h"
#include "BinarySerializer.h"

struct PluralEquationWrapper final : public DynamicObject
{
    explicit PluralEquationWrapper(TranslationsManager &owner) : translator(owner)
    {
        this->setMethod(Serialization::Translations::wrapperMethodName, PluralEquationWrapper::detect);
    }
    
    static Identifier getClassName()
    {
        return Serialization::Translations::wrapperClassName;
    }
    
    static var detect(const var::NativeFunctionArgs &args)
    {
        if (args.numArguments > 0)
        {
            if (auto *thisObject = dynamic_cast<PluralEquationWrapper *>(args.thisObject.getObject()))
            {
                thisObject->translator.equationResult = args.arguments[0].toString();
            }
        }
        
        return var::undefined();
    }
    
    TranslationsManager &translator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluralEquationWrapper)
};

TranslationsManager::TranslationsManager() :
    ResourceManager(Serialization::Resources::translations)
{
    this->engine = new JavascriptEngine();
    this->engine->maximumExecutionTime = RelativeTime::milliseconds(200);

    PluralEquationWrapper::Ptr pluralEquationWrapper(new PluralEquationWrapper(*this));
    this->engine->registerNativeObject(Serialization::Translations::wrapperClassName, pluralEquationWrapper.get());
}

TranslationsManager::~TranslationsManager()
{
    this->engine = nullptr;
}

//===----------------------------------------------------------------------===//
// Translations
//===----------------------------------------------------------------------===//

const Translation::Ptr TranslationsManager::getCurrent() const noexcept
{
    return this->currentTranslation;
}

void TranslationsManager::loadLocaleWithId(const String &localeId)
{
    if (this->currentTranslation->id == localeId)
    {
        DBG(localeId + "translation is already loaded, skipping");
        return;
    }

    if (const auto translation = this->getResourceById<Translation>(localeId))
    {
        this->currentTranslation = translation;
        App::Config().setProperty(Serialization::Config::currentLocale, localeId);
        this->sendChangeMessage();
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

String TranslationsManager::translate(const String &text)
{
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);

    const auto foundCurrentSingular = this->currentTranslation->singulars.find(text);
    if (foundCurrentSingular != this->currentTranslation->singulars.end())
    {
        return foundCurrentSingular->second;
    }

    const auto foundCurrentPlural = this->currentTranslation->plurals.find(text);
    if (foundCurrentPlural != this->currentTranslation->plurals.end())
    {
        const auto *plurals = foundCurrentPlural->second.get();
        if (plurals->size() > 0)
        {
            return plurals->begin()->second;
        }
    }

    const auto foundFallbackSingular = this->fallbackTranslation->singulars.find(text);
    if (foundFallbackSingular != this->fallbackTranslation->singulars.end())
    {
        return foundFallbackSingular->second;
    }

    return text;
}

String TranslationsManager::translate(const String &baseLiteral, int64 targetNumber)
{
    using namespace Serialization;
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);

    const auto foundPlural = this->currentTranslation->plurals.find(baseLiteral);
    if (foundPlural == this->currentTranslation->plurals.end())
    {
        return baseLiteral.replace(Translations::metaSymbol, String(targetNumber));
    }

    const String expessionToEvaluate =
        this->currentTranslation->pluralEquation.replace(Translations::metaSymbol,
            String(targetNumber > 0 ? targetNumber : -targetNumber));

    const Result result = this->engine->execute(expessionToEvaluate);
    if (!result.failed())
    {
        const String pluralForm = this->equationResult;
        const auto foundTranslation = foundPlural->second->find(pluralForm);
        if (foundTranslation != foundPlural->second->end())
        {
            return foundTranslation->second.replace(Translations::metaSymbol, String(targetNumber));
        }
    }

    return baseLiteral.replace(Translations::metaSymbol, String(targetNumber));
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

const static String fallbackTranslationId = "en";

void TranslationsManager::deserializeResources(const ValueTree &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::translations) ?
        tree : tree.getChildWithName(Serialization::Resources::translations);

    if (!root.isValid()) { return; }

    const String selectedLocaleId = this->getSelectedLocaleId();

    // First, fill up the available translations
    forEachValueTreeChildWithType(root, translationRoot, Serialization::Translations::locale)
    {
        Translation::Ptr translation(new Translation());
        translation->deserialize(translationRoot);
        outResources[translation->getResourceId()] = translation;

        if (translation->id == selectedLocaleId)
        {
            this->currentTranslation = translation;
        }

        if (translation->id == fallbackTranslationId)
        {
            this->fallbackTranslation = translation;
        }
    }

    if (this->currentTranslation == nullptr)
    {
        this->currentTranslation = this->fallbackTranslation;
    }

    jassert(this->currentTranslation != nullptr);
    jassert(this->fallbackTranslation != nullptr);
}

void TranslationsManager::reset()
{
    ResourceManager::reset();
    this->currentTranslation = nullptr;
    this->fallbackTranslation = nullptr;
    this->equationResult.clear();
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

String TranslationsManager::getSelectedLocaleId() const
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

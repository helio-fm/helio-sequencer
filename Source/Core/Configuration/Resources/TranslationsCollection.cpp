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
#include "TranslationsCollection.h"
#include "SerializationKeys.h"
#include "Config.h"

struct PluralEquationWrapper final : public DynamicObject
{
    explicit PluralEquationWrapper(TranslationsCollection &owner) : translator(owner)
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
    
    TranslationsCollection &translator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluralEquationWrapper)
};

TranslationsCollection::TranslationsCollection() :
    ConfigurationResourceCollection(Serialization::Resources::translations)
{
    this->engine = make<JavascriptEngine>();
    this->engine->maximumExecutionTime = RelativeTime::milliseconds(200);

    PluralEquationWrapper::Ptr pluralEquationWrapper(new PluralEquationWrapper(*this));
    this->engine->registerNativeObject(Serialization::Translations::wrapperClassName, pluralEquationWrapper.get());
}

TranslationsCollection::~TranslationsCollection()
{
    this->engine = nullptr;
}

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

    const auto foundFallbackSingular = this->fallbackTranslation->singulars.find(key);
    if (foundFallbackSingular != this->fallbackTranslation->singulars.end())
    {
        return foundFallbackSingular->second;
    }

    return {};
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

    const String expressionToEvaluate =
        this->currentTranslation->pluralEquation.replace(Translations::metaSymbol,
            String(targetNumber > 0 ? targetNumber : -targetNumber));

    const Result result = this->engine->execute(expressionToEvaluate);
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

void TranslationsCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::translations) ?
        tree : tree.getChildWithName(Serialization::Resources::translations);

    if (!root.isValid()) { return; }

    const String selectedLocaleId = this->getSelectedLocaleId();

    forEachChildWithType(root, translationRoot, Serialization::Translations::locale)
    {
        // if the existing translation for locale id is found,
        // just extend it, otherwise some new keys may be missing:
        const auto translationId = translationRoot.getProperty(Serialization::Translations::localeId).toString().toLowerCase();
        const auto existingTranslation = outResources.find(translationId);

        Translation::Ptr translation(existingTranslation != outResources.end() ?
            static_cast<Translation *>(existingTranslation->second.get()) : new Translation());

        //DBG(translationId + "/" + translation->getResourceId());
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

void TranslationsCollection::reset()
{
    ConfigurationResourceCollection::reset();
    this->currentTranslation = nullptr;
    this->fallbackTranslation = nullptr;
    this->equationResult.clear();
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

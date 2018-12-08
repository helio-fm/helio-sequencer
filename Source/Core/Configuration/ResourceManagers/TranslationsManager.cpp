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
#include "App.h"
#include "Config.h"
#include "BinaryData.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "JsonSerializer.h"
#include "BinarySerializer.h"

struct PluralEquationWrapper: public DynamicObject
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
    ResourceManager(Serialization::Resources::translations) {}

void TranslationsManager::initialise()
{
    this->engine = new JavascriptEngine();
    this->engine->maximumExecutionTime = RelativeTime::milliseconds(200);
    
    PluralEquationWrapper::Ptr pluralEquationWrapper(new PluralEquationWrapper(*this));
    this->engine->registerNativeObject(Serialization::Translations::wrapperClassName, pluralEquationWrapper.get());
    
    this->reloadResources();
}

void TranslationsManager::shutdown()
{
    this->reset();
    this->engine = nullptr;
}

//===----------------------------------------------------------------------===//
// Translations
//===----------------------------------------------------------------------===//

const Translation::Ptr TranslationsManager::getCurrentLocale() const noexcept
{
    return this->currentTranslation;
}

void TranslationsManager::loadLocaleWithName(const String &localeName)
{
    if (this->currentTranslation->name == localeName)
    {
        DBG(localeName + "translation is already loaded, skipping");
        return;
    }

    Resources::Iterator i(this->resources);
    while (i.next())
    {
        const Translation::Ptr translation = static_cast<Translation *>(i.getValue().get());
        if (translation->name == localeName)
        {
            this->currentTranslation = translation;
            const auto localeId = i.getKey();
            Config::set(Serialization::Config::currentLocale, localeId);
            this->sendChangeMessage();
            return;
        }
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

String TranslationsManager::translate(const String &text)
{
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);

    if (this->currentTranslation->singulars.contains(text))
    {
        return this->currentTranslation->singulars[text];
    }

    if (this->currentTranslation->plurals.contains(text))
    {
        const auto *plurals = this->currentTranslation->plurals[text].get();
        if (plurals->size() > 0)
        {
            return plurals->begin()->second;
        }
    }

    if (this->fallbackTranslation->singulars.contains(text))
    {
        return this->fallbackTranslation->singulars[text];
    }

    return text;
}

String TranslationsManager::translate(const String &baseLiteral, int64 targetNumber)
{
    using namespace Serialization;
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);

    if (!this->currentTranslation->plurals.contains(baseLiteral))
    {
        return baseLiteral.replace(Translations::metaSymbol, String(targetNumber));
    }

    const int64 absTargetNumber = (targetNumber > 0 ? targetNumber : -targetNumber);

    const String expessionToEvaluate =
        Translations::wrapperClassName + "." +
        Translations::wrapperMethodName + "(" +
        this->currentTranslation->pluralEquation.replace(Translations::metaSymbol, String(absTargetNumber)) + ")";

    const Result result = this->engine->execute(expessionToEvaluate);
    if (!result.failed())
    {
        const String pluralForm = this->equationResult;
        auto *targetPlurals = this->currentTranslation->plurals[baseLiteral].get();
        const auto translation = targetPlurals->find(pluralForm);
        if (translation != targetPlurals->end())
        {
            return translation->second.replace(Translations::metaSymbol, String(targetNumber));
        }
    }

    return baseLiteral.replace(Translations::metaSymbol, String(targetNumber));
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

const static String fallbackTranslationId = "en";

ValueTree TranslationsManager::serialize() const
{
    ValueTree emptyXml(Serialization::Translations::translations);
    // TODO
    return emptyXml;
}

void TranslationsManager::deserialize(const ValueTree &tree)
{
    using namespace Serialization;

    const auto root = tree.hasType(Translations::translations) ?
        tree : tree.getChildWithName(Translations::translations);
    
    if (!root.isValid()) { return; }

    const String selectedLocaleId = this->getSelectedLocaleId();

    // First, fill up the available translations
    forEachValueTreeChildWithType(root, translationRoot, Translations::locale)
    {
        Translation::Ptr translation(new Translation());
        translation->deserialize(translationRoot);
        this->resources.set(translation->getResourceId(), translation);
        
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
   
    if (Config::contains(Serialization::Config::currentLocale))
    {
        return Config::get(Serialization::Config::currentLocale, fallbackTranslationId);
    }
    
    const String systemLocale =
        SystemStats::getUserLanguage().toLowerCase().substring(0, 2);
    
    if (this->resources.contains(systemLocale))
    {
        return systemLocale;
    }
    
    return fallbackTranslationId;
}

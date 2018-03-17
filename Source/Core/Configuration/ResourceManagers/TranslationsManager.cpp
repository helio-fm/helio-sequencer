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
            if (PluralEquationWrapper *thisObject =
                dynamic_cast<PluralEquationWrapper *>(args.thisObject.getObject()))
            {
                //Logger::writeToLog("PluralEquationWrapper::detect " + args.arguments[0].toString());
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

void TranslationsManager::initialise(const String &commandLine)
{
    this->engine = new JavascriptEngine();
    this->engine->maximumExecutionTime = RelativeTime::milliseconds(200);
    
    PluralEquationWrapper::Ptr pluralEquationWrapper(new PluralEquationWrapper(*this));
    this->engine->registerNativeObject(Serialization::Translations::wrapperClassName, pluralEquationWrapper);
    
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

const Array<Translation::Ptr> TranslationsManager::getAvailableLocales() const
{
    const Translation comparator;
    Array<Translation::Ptr> result;
    HashMap<String, Translation::Ptr>::Iterator i(this->availableTranslations);
    
    while (i.next())
    {
        result.addSorted(comparator, i.getValue());
    }
    
    return result;
}

const Translation::Ptr TranslationsManager::getCurrentLocale() const noexcept
{
    return this->currentTranslation;
}

void TranslationsManager::loadLocaleWithName(const String &localeName)
{
    if (this->currentTranslation->name == localeName)
    {
        Logger::writeToLog(localeName + "translation is already loaded, skipping");
        return;
    }

    HashMap<String, Translation::Ptr>::Iterator i(this->availableTranslations);
    
    while (i.next())
    {
        if (i.getValue()->name == localeName)
        {
            this->currentTranslation = i.getValue();
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
    return this->currentTranslation->singulars.getValue(text, text);
}

String TranslationsManager::translate(const String &text, const String &resultIfNotFound)
{
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);
    return this->currentTranslation->singulars.getValue(text, resultIfNotFound);
}

String TranslationsManager::translate(const String &baseLiteral, int64 targetNumber)
{
    const SpinLock::ScopedLockType sl(this->currentTranslationLock);

    //const double startTime = Time::getMillisecondCounterHiRes();
    const int64 absTargetNumber = (targetNumber > 0 ? targetNumber : -targetNumber);

    const String expessionToEvaluate =
        Serialization::Translations::wrapperClassName +
        "." +
        Serialization::Translations::wrapperMethodName +
        "(" +
        this->currentTranslation->pluralEquation.replace(Serialization::Translations::metaSymbol, String(absTargetNumber)) +
        ")";

    //Logger::writeToLog("expessionToEvaluate: " + expessionToEvaluate);

    Result result = this->engine->execute(expessionToEvaluate);
    //const double elapsedMs = Time::getMillisecondCounterHiRes() - startTime;

    if (!result.failed())
    {
        StringPairArray targetPlurals = this->currentTranslation->plurals[baseLiteral];
        const String pluralForm = this->equationResult;
        //Logger::writeToLog("(Execution time: " + String (elapsedMs, 2) + " milliseconds): " + pluralForm);
        return targetPlurals[pluralForm].replace(Serialization::Translations::metaSymbol, String(targetNumber));
    }

    return baseLiteral.replace(Serialization::Translations::metaSymbol, String(targetNumber));
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree TranslationsManager::serialize() const
{
    ValueTree emptyXml(Serialization::Translations::translations);
    // TODO
    return emptyXml;
}

void TranslationsManager::deserialize(const ValueTree &tree)
{
    this->reset();
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
        this->availableTranslations.set(translation->name, translation);
        if (translation->id == selectedLocaleId)
        {
            this->currentTranslation = translation;
        }
    }
}

void TranslationsManager::reset()
{
    this->availableTranslations.clear();
    this->equationResult.clear();
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

String TranslationsManager::getSelectedLocaleId() const
{
    const String lastFallbackLocale = "en";
    
    if (Config::contains(Serialization::Config::currentLocale))
    {
        return Config::get(Serialization::Config::currentLocale, lastFallbackLocale);
    }
    
    const String systemLocale =
        SystemStats::getUserLanguage().toLowerCase().substring(0, 2);
    
    if (this->availableTranslations.contains(systemLocale))
    {
        return systemLocale;
    }
    
    return lastFallbackLocale;
}

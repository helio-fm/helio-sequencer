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
#include "TranslationManager.h"
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
    explicit PluralEquationWrapper(TranslationManager &owner) : translator(owner)
    {
        this->setMethod(Serialization::Locales::wrapperMethodName, PluralEquationWrapper::detect);
    }
    
    static Identifier getClassName()
    {
        return Serialization::Locales::wrapperClassName;
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
    
    TranslationManager &translator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluralEquationWrapper)
};

void TranslationManager::initialise(const String &commandLine)
{
    this->reset();

    this->engine = new JavascriptEngine();
    this->engine->maximumExecutionTime = RelativeTime::milliseconds(100);
    
    PluralEquationWrapper::Ptr pluralEquationWrapper(new PluralEquationWrapper(*this));
    this->engine->registerNativeObject(Serialization::Locales::wrapperClassName, pluralEquationWrapper);
    
    this->reloadLocales();
}

void TranslationManager::shutdown()
{
    this->reset();
    this->engine = nullptr;
}


//===----------------------------------------------------------------------===//
// Translations
//===----------------------------------------------------------------------===//

StringPairArray TranslationManager::getMappings() const
{
    return this->singulars;
}

String TranslationManager::findPluralFor(const String &baseLiteral, int64 targetNumber)
{
    //const double startTime = Time::getMillisecondCounterHiRes();
    const int64 absTargetNumber = (targetNumber > 0 ? targetNumber : -targetNumber);
    
    const String expessionToEvaluate =
        Serialization::Locales::wrapperClassName +
        "." +
        Serialization::Locales::wrapperMethodName +
        "(" +
        this->pluralEquation.replace(Serialization::Locales::metaSymbol, String(absTargetNumber)) +
        ")";
    
    //Logger::writeToLog("expessionToEvaluate: " + expessionToEvaluate);

    Result result = this->engine->execute(expessionToEvaluate);
    //const double elapsedMs = Time::getMillisecondCounterHiRes() - startTime;
    
    if (! result.failed())
    {
        StringPairArray targetPlurals = this->plurals[baseLiteral];
        const String pluralForm = this->equationResult;
        //Logger::writeToLog("(Execution time: " + String (elapsedMs, 2) + " milliseconds): " + pluralForm);
        return targetPlurals[pluralForm].replace(Serialization::Locales::metaSymbol, String(targetNumber));
    }

    return baseLiteral.replace(Serialization::Locales::metaSymbol, String(targetNumber));
}


//===----------------------------------------------------------------------===//
// Locales
//===----------------------------------------------------------------------===//

Array<TranslationManager::Locale> TranslationManager::getAvailableLocales() const
{
    const Locale comparator;
    Array<Locale> result;
    HashMap<String, Locale>::Iterator i(this->availableTranslations);
    
    while (i.next())
    {
        result.addSorted(comparator, i.getValue());
    }
    
    return result;
}

void TranslationManager::loadLocaleWithName(const String &localeName)
{
    HashMap<String, Locale>::Iterator i(this->availableTranslations);
    
    while (i.next())
    {
        if (i.getValue().localeName == localeName)
        {
            // No check if locale is already selected, we need to reload it anyway 
            // (updated translations file may be waiting to be read)
            this->loadLocaleWithId(i.getKey());
            return;
        }
    }
}

void TranslationManager::loadLocaleWithId(const String &localeId)
{
    if (! this->availableTranslations.contains(localeId))
    {
        return;
    }
    
    if (this->availableTranslations.contains(localeId))
    {
        Config::set(Serialization::Config::currentLocale, localeId);
        this->reloadLocales();
        this->sendChangeMessage();
    }
}

String TranslationManager::getCurrentLocaleName() const
{
    const String localeId(this->getSelectedLocaleId());
    return this->availableTranslations[localeId].localeName;
}

String TranslationManager::getCurrentLocaleId() const
{
    return this->getSelectedLocaleId();
}


//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

String TranslationManager::translate(const String &text)
{
    const SpinLock::ScopedLockType sl(this->mappingsLock);
    return this->getMappings().getValue(text, text);
}

String TranslationManager::translate(const String &text, const String &resultIfNotFound)
{
    const SpinLock::ScopedLockType sl(this->mappingsLock);
    return this->getMappings().getValue(text, resultIfNotFound);
}

String TranslationManager::translate(const String &baseLiteral, int64 targetNumber)
{
    const SpinLock::ScopedLockType sl(this->mappingsLock);
    return this->findPluralFor(baseLiteral, targetNumber);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree TranslationManager::serialize() const
{
    ValueTree emptyXml(Serialization::Locales::translations);
    return emptyXml;
}

void TranslationManager::deserialize(const ValueTree &tree)
{
    using namespace Serialization;

    this->reset();
    
    const auto root = tree.hasType(Locales::translations) ?
        tree : tree.getChildWithName(Locales::translations);
    
    if (!root.isValid()) { return; }
    
    // First, fill up the available translations
    forEachValueTreeChildWithType(root, locale, Locales::locale)
    {
        const String localeId = locale.getProperty(Locales::id).toString().toLowerCase();
        const String localeName = locale.getProperty(Locales::name);
        const String localeAuthor = locale.getProperty(Locales::author);
        
        Locale newLocale;
        newLocale.localeName = localeName;
        newLocale.localeId = localeId;
        newLocale.localeAuthor = localeAuthor;
        this->availableTranslations.set(localeId, newLocale);
    }
    
    // Detect the right translation and load
    const String selectedLocaleId = this->getSelectedLocaleId();
    
    forEachValueTreeChildWithType(root, locale, Locales::locale)
    {
        const String localeId =
        locale.getProperty(Locales::id).toString().toLowerCase();

        if (localeId == selectedLocaleId)
        {
            this->pluralEquation = locale.getProperty(Locales::pluralEquation);

            forEachValueTreeChildWithType(locale, pluralLiteral, Locales::pluralLiteral)
            {
                const String baseLiteral = pluralLiteral.getProperty(Locales::name);

                StringPairArray formsAndTranslations;
                
                forEachValueTreeChildWithType(pluralLiteral, pluralTranslation, Locales::translation)
                {
                    const String translatedLiteral = pluralTranslation.getProperty(Locales::name);
                    const String pluralForm = pluralTranslation.getProperty(Locales::pluralForm);
                    formsAndTranslations.set(pluralForm, translatedLiteral);
                }
                
                this->plurals.set(baseLiteral, formsAndTranslations);
            }

            forEachValueTreeChildWithType(locale, literal, Locales::literal)
            {
                const String literalName = literal.getProperty(Locales::name);
                const String translatedLiteral = literal.getProperty(Locales::translation);
                this->singulars.set(literalName, translatedLiteral);
            }
        }
    }
}

void TranslationManager::reset()
{
    this->availableTranslations.clear();
    this->pluralEquation.clear();
    this->equationResult.clear();
    this->singulars.clear();
    this->plurals.clear();
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void TranslationManager::reloadLocales()
{
    const File downloadedTranslations(this->getDownloadedTranslationsFile());
    if (downloadedTranslations.existsAsFile())
    {
        const auto tree(DocumentHelpers::load(downloadedTranslations));
        if (tree.isValid())
        {
            Logger::writeToLog("Found downloaded translations file, loading..");
            this->deserialize(tree);
            this->sendChangeMessage();
            return;
        }
    }

    const auto defaultTranslations = String(CharPointer_UTF8(BinaryData::Translations_json));
    const auto state = DocumentHelpers::load(defaultTranslations);
    if (state.isValid())
    {
        this->deserialize(state);
    }

}

void TranslationManager::updateLocales(const ValueTree &locales)
{
    Logger::writeToLog("Updating downloaded translations file..");

    XmlSerializer serializer; // debug
    //BinarySerializer serializer;
    serializer.saveToFile(this->getDownloadedTranslationsFile(), locales);

    this->deserialize(locales);

    // Don't send redundant change messages here,
    // as translations are going to be updated at the very start of the app:
    //this->sendChangeMessage();
}

String TranslationManager::getSelectedLocaleId() const
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

//===----------------------------------------------------------------------===//
// Static
//===----------------------------------------------------------------------===//

File TranslationManager::getDownloadedTranslationsFile()
{
    static String downloadedTranslationsFileName = "translations.helio";
    return DocumentHelpers::getConfigSlot(downloadedTranslationsFileName);
}

File TranslationManager::getDebugTranslationsFile()
{
    static String debugTranslationsFileName = "TranslationsDebug.xml";
    return File::getSpecialLocation(File::currentApplicationFile).getSiblingFile(debugTranslationsFileName);
}

int TranslationManager::Locale::compareElements(const Locale &first, const Locale &second)
{
    return first.localeName.compare(second.localeName);
}

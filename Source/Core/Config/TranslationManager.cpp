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
#include "RequestResourceThread.h"
#include "App.h"
#include "SessionManager.h"
#include "Config.h"
#include "BinaryData.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "JsonSerializer.h"

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
    
    // Run update thread after 1 sec
    const int requestTranslationsDelayMs = 1000;
    this->startTimer(requestTranslationsDelayMs);
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
    
    this->setSelectedLocaleId(localeId);
    this->reloadLocales();
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
    
    // Now detect the right one and load
    const String selectedLocaleId = this->getSelectedLocaleId();
    
    forEachValueTreeChildWithType(root, locale, Locales::locale)
    {
        const String localeId =
        locale.getProperty(Locales::id).toString().toLowerCase();
        
        if (localeId == selectedLocaleId)
        {
            forEachValueTreeChildWithType(locale, pluralForms, Locales::pluralForms)
            {
                this->pluralEquation = pluralForms.getProperty(Locales::equation);
            }

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

String TranslationManager::getLocalizationFileContents() const
{
    return String(CharPointer_UTF8(BinaryData::DefaultTranslations_xml));
}

void TranslationManager::loadFromXml(const String &xmlData)
{
    XmlSerializer serializer;
    ValueTree state;
    serializer.loadFromString(xmlData, state);
    if (state.isValid())
    {
        this->deserialize(state);
    }
}

void TranslationManager::reloadLocales()
{
    const File downloadedTranslations(this->getDownloadedTranslationsFile());
    if (downloadedTranslations.existsAsFile())
    {
        const auto tree(DocumentHelpers::load<JsonSerializer>(downloadedTranslations));
        if (tree.isValid())
        {
            Logger::writeToLog("Found downloaded translations file, loading..");
            this->deserialize(tree);
            this->sendChangeMessage();
        }
    }
    else
    {
        this->loadFromXml(this->getLocalizationFileContents());
    }
}

String TranslationManager::getSelectedLocaleId() const
{
    const String lastFallbackLocale = "en";
    
    if (Config::contains(Serialization::Locales::currentLocale))
    {
        return Config::get(Serialization::Locales::currentLocale, lastFallbackLocale);
    }
    
    const String systemLocale =
    SystemStats::getUserLanguage().toLowerCase().substring(0, 2);
    
    if (this->availableTranslations.contains(systemLocale))
    {
        return systemLocale;
    }
    
    return lastFallbackLocale;
}

void TranslationManager::setSelectedLocaleId(const String &localeId)
{
    if (this->availableTranslations.contains(localeId))
    {
        Config::set(Serialization::Locales::currentLocale, localeId);
        this->sendChangeMessage();
    }
}


//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void TranslationManager::timerCallback()
{
    this->stopTimer();
    const auto requestTranslationsThread =
        App::Backend().getRequestThread<RequestResourceThread>();
    requestTranslationsThread->requestResource(this, "translations");
}

//===----------------------------------------------------------------------===//
// RequestTranslationsThread::Listener
//===----------------------------------------------------------------------===//

void TranslationManager::requestResourceOk(const ValueTree &resource)
{
    Logger::writeToLog("TranslationManager::requestResourceOk");
    //ScopedPointer<XmlElement> xml(XmlDocument::parse(translations));
    //if (xml)
    //{
    //    const bool seemsToBeValid = xml->hasTagName(Serialization::Locales::translations);
    //    if (seemsToBeValid)
    //    {
    //        Logger::writeToLog("TranslationManager :: downloaded translations file seems to be valid");
    //        DocumentReader::saveObfuscated(this->getDownloadedTranslationsFile(), xml);

    //        Logger::writeToLog("TranslationManager :: applying new translations");
    //        this->deserialize(*xml);
    //        // Don't send redunant change messages,
    //        // as translations are going to be updated at the very start of the app:
    //        //this->sendChangeMessage();
    //    }
    //}
}

void TranslationManager::requestResourceFailed(const Array<String> &errors)
{
    Logger::writeToLog("TranslationManager::requestResourceFailed " + errors.getFirst());
}

void TranslationManager::requestResourceConnectionFailed()
{
    Logger::writeToLog("TranslationManager::requestResourceConnectionFailed");
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

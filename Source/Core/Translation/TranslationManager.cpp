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
#include "DataEncoder.h"
#include "BinaryData.h"
#include "FileUtils.h"

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


static String unescapeString(const String &s)
{
    return s.replace("\\\"", "\"")
            .replace("\\\'", "\'")
            .replace("\\t", "\t")
            .replace("\\r", "\r")
            .replace("\\n", "\n");
}


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
        
        for (int i = 0; i < targetPlurals.size(); ++i)
        {
            return targetPlurals[pluralForm].replace(Serialization::Locales::metaSymbol, String(targetNumber));
        }
    }

    return baseLiteral.replace(Serialization::Locales::metaSymbol, String(targetNumber));
}


//===----------------------------------------------------------------------===//
// Locales
//===----------------------------------------------------------------------===//

Array<TranslationManager::Locale> TranslationManager::getAvailableLocales() const
{
    Array<Locale> result;
    HashMap<String, Locale>::Iterator i(this->availableTranslations);
    
    while (i.next())
    {
        result.add(i.getValue());
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
// Static
//===----------------------------------------------------------------------===//

SpinLock currentMappingsLock;

String TranslationManager::translate(const String &text)
{
    const SpinLock::ScopedLockType sl(currentMappingsLock);
    return TranslationManager::getInstance().getMappings().getValue(text, text);
}

String TranslationManager::translate(const String &text, const String &resultIfNotFound)
{
    const SpinLock::ScopedLockType sl(currentMappingsLock);
    return TranslationManager::getInstance().getMappings().getValue(text, resultIfNotFound);
}

String TranslationManager::translate(const String &baseLiteral, int64 targetNumber)
{
    const SpinLock::ScopedLockType sl(currentMappingsLock);
    return TranslationManager::getInstance().findPluralFor(baseLiteral, targetNumber);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *TranslationManager::serialize() const
{
    auto emptyXml = new XmlElement(Serialization::Locales::translations);
    return emptyXml;
}

void TranslationManager::deserialize(const XmlElement &xml)
{
    this->reset();
    
    const XmlElement *root = xml.hasTagName(Serialization::Locales::translations) ?
        &xml : xml.getChildByName(Serialization::Locales::translations);
    
    if (root == nullptr) { return; }
    
    // First, fill up the available translations
    forEachXmlChildElementWithTagName(*root, locale, Serialization::Locales::locale)
    {
        const String localeId = locale->getStringAttribute(Serialization::Locales::id).toLowerCase();
        const String localeName = locale->getStringAttribute(Serialization::Locales::name);
        const String localeAuthor = locale->getStringAttribute(Serialization::Locales::author);
        
        Locale newLocale;
        newLocale.localeName = localeName;
        newLocale.localeId = localeId;
        newLocale.localeAuthor = localeAuthor;
        this->availableTranslations.set(localeId, newLocale);
    }
    
    // Now detect the right one and load
    const String selectedLocaleId = this->getSelectedLocaleId();
    
    forEachXmlChildElementWithTagName(*root, locale, Serialization::Locales::locale)
    {
        const String localeId =
        locale->getStringAttribute(Serialization::Locales::id).toLowerCase();
        
        if (localeId == selectedLocaleId)
        {
            forEachXmlChildElementWithTagName(*locale, pluralForms, Serialization::Locales::pluralForms)
            {
                this->pluralEquation = pluralForms->getStringAttribute(Serialization::Locales::equation);
            }

            forEachXmlChildElementWithTagName(*locale, pluralLiteral, Serialization::Locales::pluralLiteral)
            {
                const String baseLiteral = pluralLiteral->getStringAttribute(Serialization::Locales::name);

                StringPairArray formsAndTranslations;
                
                forEachXmlChildElementWithTagName(*pluralLiteral, pluralTranslation, Serialization::Locales::translation)
                {
                    const String translatedLiteral = pluralTranslation->getStringAttribute(Serialization::Locales::name);
                    const String pluralForm = pluralTranslation->getStringAttribute(Serialization::Locales::pluralForm);
                    formsAndTranslations.set(pluralForm, translatedLiteral);
                }
                
                this->plurals.set(baseLiteral, formsAndTranslations);
            }

            forEachXmlChildElementWithTagName(*locale, literal, Serialization::Locales::literal)
            {
                const String literalName = literal->getStringAttribute(Serialization::Locales::name);
                const String translatedLiteral = literal->getStringAttribute(Serialization::Locales::translation);
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
    ScopedPointer<XmlElement> xml(XmlDocument::parse(xmlData));
    
    if (xml)
    {
        this->deserialize(*xml);
    }
}

void TranslationManager::reloadLocales()
{
    const File downloadedTranslations(this->getDownloadedTranslationsFile());
    
    if (downloadedTranslations.existsAsFile())
    {
        ScopedPointer<XmlElement> xml(DataEncoder::loadObfuscated(downloadedTranslations));
        if (xml)
        {
            Logger::writeToLog("Found downloaded translations file, loading..");
            this->deserialize(*xml);
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
    this->requestTranslationsThread = new RequestTranslationsThread();
    this->requestTranslationsThread->requestTranslations(this);
}


//===----------------------------------------------------------------------===//
// RequestTranslationsThread::Listener
//===----------------------------------------------------------------------===//

void TranslationManager::translationsRequestOk()
{
    Logger::writeToLog("TranslationManager::translationsRequestOk");
    const String translations(DataEncoder::deobfuscateString(this->requestTranslationsThread->getLatestResponse()));
    ScopedPointer<XmlElement> xml(XmlDocument::parse(translations));

    if (xml)
    {
        const bool seemsToBeValid = xml->hasTagName(Serialization::Locales::translations);
        if (seemsToBeValid)
        {
            Logger::writeToLog("TranslationManager :: downloaded translations file seems to be valid");
            DataEncoder::saveObfuscated(this->getDownloadedTranslationsFile(), xml);
        }
    }
}

void TranslationManager::translationsRequestFailed()
{
    Logger::writeToLog("TranslationManager::translationsRequestConnectionFailed");
}


//===----------------------------------------------------------------------===//
// Static
//===----------------------------------------------------------------------===//

File TranslationManager::getDownloadedTranslationsFile()
{
    static String downloadedTranslationsFileName = "translations.helio";
    return FileUtils::getConfigSlot(downloadedTranslationsFileName);
}

File TranslationManager::getDebugTranslationsFile()
{
    static String debugTranslationsFileName = "TranslationsDebug.xml";
    return File::getSpecialLocation(File::currentApplicationFile).getSiblingFile(debugTranslationsFileName);
}


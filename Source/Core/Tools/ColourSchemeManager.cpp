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
#include "ColourSchemeManager.h"
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "BinaryData.h"

#include "App.h"
#include "Config.h"
#include "DataEncoder.h"


void ColourSchemeManager::initialise(const String &commandLine)
{
    this->reloadSchemes();
    const int requestDelayMs = 7000;
    this->startTimer(requestDelayMs);
}

void ColourSchemeManager::shutdown()
{
    this->reset();
}

Array<ColourScheme> ColourSchemeManager::getSchemes() const
{
    return this->schemes;
}

ColourScheme ColourSchemeManager::getCurrentScheme() const
{
    const String schemeData =
        Config::get(Serialization::UI::Colours::appliedScheme);
 
    if (schemeData.isEmpty())
    {
        return this->schemes[0]; // Will return ColourScheme() if array is empty
    }

    ColourScheme cs(schemeData);
    return cs;
}

void ColourSchemeManager::setCurrentScheme(const ColourScheme &scheme)
{
    ScopedPointer<XmlElement> xml(scheme.serialize());
    const String xmlString(xml->createDocument("", false, true, "UTF-8", 512));
    Config::set(Serialization::UI::Colours::appliedScheme, xmlString);
}

bool ColourSchemeManager::isPullPending() const
{
    if (this->requestThread == nullptr)
    {
        return false;
    }
    
    return this->requestThread->isThreadRunning();
}

void ColourSchemeManager::pull()
{
    if (this->isPullPending())
    {
        return;
    }
    
    this->requestThread = new RequestColourSchemesThread();
    this->requestThread->requestSchemes(this);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *ColourSchemeManager::serialize() const
{
    auto xml = new XmlElement(Serialization::UI::Colours::schemes);
    
    for (int i = 0; i < this->schemes.size(); ++i)
    {
        xml->addChildElement(this->schemes.getUnchecked(i).serialize());
    }
    
    return xml;
}

void ColourSchemeManager::deserialize(const XmlElement &xml)
{
    this->reset();
    
    const XmlElement *root = xml.hasTagName(Serialization::UI::Colours::schemes) ?
        &xml : xml.getChildByName(Serialization::UI::Colours::schemes);
    
    if (root == nullptr) { return; }
    
    forEachXmlChildElementWithTagName(*root, schemeXml, Serialization::UI::Colours::scheme)
    {
        ColourScheme cs;
        cs.deserialize(*schemeXml);
        this->schemes.add(cs);
    }
    
    this->sendChangeMessage();
}

void ColourSchemeManager::reset()
{
    this->schemes.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void ColourSchemeManager::reloadSchemes()
{
    const String configSchemes(this->getConfigSchemes());

    if (configSchemes.isNotEmpty())
    {
        Logger::writeToLog("Found config schemes, loading..");
        ScopedPointer<XmlElement> xml(XmlDocument::parse(configSchemes));

        if (xml != nullptr)
        {
            this->deserialize(*xml);
        }
    }
    else
    {
        // built-in schemes
        const String defaultSchemes = String(CharPointer_UTF8(BinaryData::ColourSchemes_xml));
        ScopedPointer<XmlElement> xml(XmlDocument::parse(defaultSchemes));

        if (xml != nullptr)
        {
            this->deserialize(*xml);
        }
    }
}

void ColourSchemeManager::saveConfigSchemes()
{
    ScopedPointer<XmlElement> xml(this->serialize());
    const String xmlString(xml->createDocument("", false, true, "UTF-8", 512));
    Config::set(Serialization::UI::Colours::schemes, xmlString);
}

String ColourSchemeManager::getConfigSchemes()
{
    return Config::get(Serialization::UI::Colours::schemes);
}


//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void ColourSchemeManager::timerCallback()
{
    this->stopTimer();
    this->pull();
}


//===----------------------------------------------------------------------===//
// RequestTranslationsThread::Listener
//===----------------------------------------------------------------------===//

void ColourSchemeManager::schemesRequestOk(RequestColourSchemesThread *thread)
{
    Logger::writeToLog("ColourSchemeManager::schemesRequestOk");
    
    if (thread == this->requestThread)
    {
        ScopedPointer<XmlElement> xml(XmlDocument::parse(thread->getLastFetchedData()));
        
        if (xml != nullptr)
        {
            this->deserialize(*xml);
            this->saveConfigSchemes();
            this->sendChangeMessage();
        }
    }
}

void ColourSchemeManager::schemesRequestFailed(RequestColourSchemesThread *thread)
{
    Logger::writeToLog("ColourSchemeManager::schemesRequestConnectionFailed");
}

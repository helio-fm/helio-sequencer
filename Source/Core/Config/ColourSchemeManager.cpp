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

void ColourSchemeManager::pull()
{
    // TODO
    this->requestThread = new RequestResourceThread();
    this->requestThread->requestResource(this, "ColourScheme");
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ColourSchemeManager::serialize() const
{
    ValueTree tree(Serialization::UI::Colours::schemes);
    
    for (int i = 0; i < this->schemes.size(); ++i)
    {
        tree.appendChild(this->schemes.getUnchecked(i).serialize());
    }
    
    return tree;
}

void ColourSchemeManager::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto root = tree.hasType(Serialization::UI::Colours::schemes) ?
        tree : tree.getChildWithName(Serialization::UI::Colours::schemes);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, schemeNode, Serialization::UI::Colours::scheme)
    {
        ColourScheme cs;
        cs.deserialize(schemeNode);
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

void ColourSchemeManager::requestResourceOk(const ValueTree &resource)
{
    Logger::writeToLog("ColourSchemeManager::requestResourceOk");
    //this->deserialize(resource);
    this->saveConfigSchemes();
    this->sendChangeMessage();
}

void ColourSchemeManager::requestResourceFailed(const Array<String> &errors)
{
    Logger::writeToLog("ColourSchemeManager::requestResourceFailed: " + errors.getFirst());
}

void ColourSchemeManager::requestResourceConnectionFailed()
{
    Logger::writeToLog("ColourSchemeManager::requestResourceConnectionFailed");
}

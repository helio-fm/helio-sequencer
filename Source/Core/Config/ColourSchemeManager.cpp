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
#include "DocumentHelpers.h"
#include "App.h"
#include "Config.h"

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
    if (Config::contains(Serialization::UI::Colours::appliedScheme))
    {
        ColourScheme cs;
        Config::load(Serialization::UI::Colours::appliedScheme, &cs);
        return cs;
    }

    return this->schemes[0]; // Will return ColourScheme() if array is empty
}

void ColourSchemeManager::setCurrentScheme(const ColourScheme &scheme)
{
    Config::save(Serialization::UI::Colours::appliedScheme, &scheme);
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
        tree.appendChild(this->schemes.getUnchecked(i).serialize(), nullptr);
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
    if (Config::contains(Serialization::UI::Colours::schemes))
    {
        Config::load(Serialization::UI::Colours::schemes, this);
    }
    else
    {
        // built-in schemes
        const auto defaultSchemes = String(CharPointer_UTF8(BinaryData::Colours_json));
        const auto schemesState = DocumentHelpers::load(defaultSchemes);
        if (schemesState.isValid())
        {
            this->deserialize(schemesState);
        }
    }
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
    Config::save(Serialization::UI::Colours::schemes, this);
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

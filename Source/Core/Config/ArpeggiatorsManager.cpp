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
#include "ArpeggiatorsManager.h"
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "BinaryData.h"
#include "XmlSerializer.h"
#include "App.h"
#include "Config.h"

void ArpeggiatorsManager::initialise(const String &commandLine)
{
    this->reset();
    this->reloadArps();
    const int requestArpsDelayMs = 2000;
    
    this->startTimer(requestArpsDelayMs);
    //Logger::writeToLog(DocumentReader::obfuscate("http://helioworkstation.com/vcs/arps.php"));
}

void ArpeggiatorsManager::shutdown()
{
    this->reset();
}

Array<Arpeggiator> ArpeggiatorsManager::getArps() const
{
    return this->arps;
}

bool ArpeggiatorsManager::replaceArpWithId(const String &id, const Arpeggiator &arp)
{
    for (int i = 0; i < this->arps.size(); ++i)
    {
        if (this->arps.getUnchecked(i).getId() == id)
        {
            this->arps.setUnchecked(i, arp);
            Config::save(Serialization::Arps::arpeggiators, this);
            this->sendChangeMessage();
            return true;
        }
    }
    
    return false;
}

void ArpeggiatorsManager::addArp(const Arpeggiator &arp)
{
    if (! this->replaceArpWithId(arp.getId(), arp))
    {
        this->arps.add(arp);
        Config::save(Serialization::Arps::arpeggiators, this);
        this->sendChangeMessage();
    }
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ArpeggiatorsManager::serialize() const
{
    ValueTree tree(Serialization::Arps::arpeggiators);
    
    for (int i = 0; i < this->arps.size(); ++i)
    {
        tree.appendChild(this->arps.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void ArpeggiatorsManager::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto root = tree.hasType(Serialization::Arps::arpeggiators) ?
        tree : tree.getChildWithName(Serialization::Arps::arpeggiators);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, arpNode, Serialization::Arps::arpeggiator)
    {
        Arpeggiator arp;
        arp.deserialize(arpNode);
        this->arps.add(arp);
    }
    
    this->sendChangeMessage();
}

void ArpeggiatorsManager::reset()
{
    this->arps.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void ArpeggiatorsManager::reloadArps()
{
    const File debugArps(this->getDebugArpsFile());
    if (debugArps.existsAsFile())
    {
        Logger::writeToLog("Found debug arps file, loading..");
        XmlSerializer serializer;
        ValueTree arpsState;
        serializer.loadFromFile(debugArps, arpsState);
        if (arpsState.isValid())
        {
            this->deserialize(arpsState);
        }
    }
    else if (Config::contains(Serialization::Arps::arpeggiators))
    {
        Config::load(Serialization::Arps::arpeggiators, this);
    }
    else
    {
        // built-in arps
        const String defaultArps = String(CharPointer_UTF8(BinaryData::DefaultArps_xml));
        XmlSerializer serializer;
        ValueTree arpsState;
        serializer.loadFromString(defaultArps, arpsState);
        if (arpsState.isValid())
        {
            this->deserialize(arpsState);
        }
    }
}


//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

void ArpeggiatorsManager::timerCallback()
{
    this->stopTimer();
    // TODO update arps from the server
}


//===----------------------------------------------------------------------===//
// Static
//===----------------------------------------------------------------------===//

File ArpeggiatorsManager::getDebugArpsFile()
{
    static String debugArpsFileName = "ArpsDebug.xml";
    return File::getSpecialLocation(File::currentApplicationFile).getSiblingFile(debugArpsFileName);
}


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
#include "DocumentHelpers.h"
#include "App.h"
#include "Config.h"

ArpeggiatorsManager::ArpeggiatorsManager() :
    ResourceManager(Serialization::Resources::arpeggiators) {}

void ArpeggiatorsManager::initialise(const String &commandLine)
{
    this->reloadResources();
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
            // TODO sage Config::save(this);
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
        // TODO sage Config::save(this);
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

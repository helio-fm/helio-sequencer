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
#include "ScalesManager.h"
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "App.h"
#include "Config.h"


ScalesManager::ScalesManager() :
    ResourceManager(Serialization::Resources::scales) {}

void ScalesManager::initialise(const String &commandLine)
{
    this->reloadResources();
}

void ScalesManager::shutdown()
{
    this->reset();
}

const Array<Scale> &ScalesManager::getScales() const
{
    return this->scales;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ScalesManager::serialize() const
{
    ValueTree tree(Serialization::Midi::scales);
    
    for (int i = 0; i < this->scales.size(); ++i)
    {
        tree.appendChild(this->scales.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void ScalesManager::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto root = tree.hasType(Serialization::Midi::scales) ?
        tree : tree.getChildWithName(Serialization::Midi::scales);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, schemeNode, Serialization::Midi::scale)
    {
        Scale s;
        s.deserialize(schemeNode);
        this->scales.add(s);
    }
    
    this->sendChangeMessage();
}

void ScalesManager::reset()
{
    this->scales.clear();
    this->sendChangeMessage();
}

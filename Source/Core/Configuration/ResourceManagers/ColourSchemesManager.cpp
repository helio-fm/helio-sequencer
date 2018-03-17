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
#include "ColourSchemesManager.h"
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "App.h"
#include "Config.h"


ColourSchemesManager::ColourSchemesManager() :
    ResourceManager(Serialization::Resources::colourSchemes) {}

void ColourSchemesManager::initialise(const String &commandLine)
{
    this->reloadResources();
}

void ColourSchemesManager::shutdown()
{
    this->reset();
}

Array<ColourScheme> ColourSchemesManager::getSchemes() const
{
    return this->schemes;
}

ColourScheme ColourSchemesManager::getCurrentScheme() const
{
    if (Config::contains(Serialization::Config::activeColourScheme))
    {
        ColourScheme cs;
        Config::load(cs, Serialization::Config::activeColourScheme);
        return cs;
    }

    return this->schemes[0]; // Will return ColourScheme() if array is empty
}

void ColourSchemesManager::setCurrentScheme(const ColourScheme &scheme)
{
    Config::save(scheme, Serialization::Config::activeColourScheme);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ColourSchemesManager::serialize() const
{
    ValueTree tree(Serialization::UI::Colours::schemes);
    
    for (int i = 0; i < this->schemes.size(); ++i)
    {
        tree.appendChild(this->schemes.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void ColourSchemesManager::deserialize(const ValueTree &tree)
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

void ColourSchemesManager::reset()
{
    this->schemes.clear();
    this->sendChangeMessage();
}

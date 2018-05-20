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

ColourScheme::Ptr ColourSchemesManager::getCurrentScheme() const
{
    if (Config::contains(Serialization::Config::activeColourScheme))
    {
        ColourScheme::Ptr cs(new ColourScheme());
        Config::load(cs, Serialization::Config::activeColourScheme);
        return cs;
    }

    if (this->resources.size() > 0)
    {
        Resources::Iterator i(this->resources);
        i.next();
        return { static_cast<ColourScheme *>(i.getValue().get()) };
    }

    return { new ColourScheme() };
}

void ColourSchemesManager::setCurrentScheme(const ColourScheme::Ptr scheme)
{
    Config::save(scheme, Serialization::Config::activeColourScheme);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ColourSchemesManager::serialize() const
{
    ValueTree tree(Serialization::UI::Colours::schemes);
    
    Resources::Iterator i(this->resources);
    while (i.next())
    {
        tree.appendChild(i.getValue()->serialize(), nullptr);
    }
    
    return tree;
}

void ColourSchemesManager::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::UI::Colours::schemes) ?
        tree : tree.getChildWithName(Serialization::UI::Colours::schemes);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, schemeNode, Serialization::UI::Colours::scheme)
    {
        ColourScheme::Ptr scheme(new ColourScheme());
        scheme->deserialize(schemeNode);
        this->resources.set(scheme->getResourceId(), scheme);
    }
}

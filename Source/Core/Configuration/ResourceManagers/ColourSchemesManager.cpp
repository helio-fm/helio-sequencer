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
#include "Config.h"

ColourSchemesManager::ColourSchemesManager() :
    ResourceManager(Serialization::Resources::colourSchemes) {}

ColourScheme::Ptr ColourSchemesManager::getCurrent() const
{
    if (App::Config().containsProperty(Serialization::Config::activeColourScheme))
    {
        ColourScheme::Ptr cs(new ColourScheme());
        App::Config().load(cs.get(), Serialization::Config::activeColourScheme);
        return cs;
    }

    if (const auto firstScheme = this->getAll().getFirst())
    {
        return firstScheme;
    }

    jassertfalse;
    return { new ColourScheme() };
}

void ColourSchemesManager::setCurrent(const ColourScheme::Ptr scheme)
{
    App::Config().save(scheme.get(), Serialization::Config::activeColourScheme);
}

void ColourSchemesManager::deserializeResources(const ValueTree &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::colourSchemes) ?
        tree : tree.getChildWithName(Serialization::Resources::colourSchemes);

    if (!root.isValid()) { return; }

    forEachValueTreeChildWithType(root, schemeNode, Serialization::UI::Colours::scheme)
    {
        ColourScheme::Ptr scheme(new ColourScheme());
        scheme->deserialize(schemeNode);
        outResources[scheme->getResourceId()] = scheme;
    }
}

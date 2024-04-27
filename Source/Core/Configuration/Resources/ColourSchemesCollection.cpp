/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "ColourSchemesCollection.h"
#include "SerializationKeys.h"
#include "Config.h"

ColourSchemesCollection::ColourSchemesCollection() :
    ConfigurationResourceCollection(Serialization::Resources::colourSchemes) {}

ColourScheme::Ptr ColourSchemesCollection::getCurrent() const
{
    if (App::Config().containsProperty(Serialization::Config::activeColourScheme))
    {
        ColourScheme::Ptr cs(new ColourScheme());
        App::Config().load(cs.get(), Serialization::Config::activeColourScheme);

        // try to load its latest version from the configs if it's still present
        const auto latestVersion = this->getResourceById<ColourScheme>(cs->getResourceId());
        if (latestVersion != nullptr)
        {
            return latestVersion;
        }

        return cs;
    }

    // likely the config file is missing here, meaning the app runs for the first time:
    if (const auto defaultScheme = this->getResourceById<ColourScheme>("Helio Theme v2"))
    {
        return defaultScheme;
    }

    jassertfalse;

    if (const auto firstScheme = this->getAll().getFirst())
    {
        return firstScheme;
    }

    return { new ColourScheme() };
}

void ColourSchemesCollection::setCurrent(const ColourScheme::Ptr scheme)
{
    App::Config().save(scheme.get(), Serialization::Config::activeColourScheme);
}

void ColourSchemesCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::colourSchemes) ?
        tree : tree.getChildWithName(Serialization::Resources::colourSchemes);

    if (!root.isValid()) { return; }

    forEachChildWithType(root, schemeNode, Serialization::UI::Colours::scheme)
    {
        ColourScheme::Ptr scheme(new ColourScheme());
        scheme->deserialize(schemeNode);
        outResources[scheme->getResourceId()] = scheme;
    }
}

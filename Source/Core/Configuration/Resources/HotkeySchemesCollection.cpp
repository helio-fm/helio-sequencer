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
#include "HotkeySchemesCollection.h"
#include "SerializationKeys.h"
#include "Config.h"

HotkeySchemesCollection::HotkeySchemesCollection() :
    ConfigurationResourceCollection(Serialization::Resources::hotkeySchemes) {}

HotkeyScheme::Ptr HotkeySchemesCollection::findActiveScheme() const
{
    if (App::Config().containsProperty(Serialization::Config::activeHotkeyScheme))
    {
        HotkeyScheme::Ptr hs(new HotkeyScheme());
        App::Config().load(hs.get(), Serialization::Config::activeHotkeyScheme);
        return hs;
    }

    if (const auto firstScheme = this->getAll().getFirst())
    {
        return firstScheme;
    }

    jassertfalse;
    return { new HotkeyScheme() };
}

const HotkeyScheme::Ptr HotkeySchemesCollection::getCurrent() const noexcept
{
    jassert(this->activeScheme != nullptr);
    return this->activeScheme;
}

void HotkeySchemesCollection::setCurrent(const HotkeyScheme::Ptr scheme)
{
    jassert(scheme != nullptr);
    this->activeScheme = scheme;
    App::Config().save(this->activeScheme.get(), Serialization::Config::activeHotkeyScheme);
}

void HotkeySchemesCollection::deserializeResources(const SerializedData &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::hotkeySchemes) ?
        tree : tree.getChildWithName(Serialization::Resources::hotkeySchemes);

    if (!root.isValid()) { return; }

    forEachChildWithType(root, schemeNode, Serialization::UI::Hotkeys::scheme)
    {
        // if the existing scheme is found, just extend it:
        const auto schemeName = schemeNode.getProperty(Serialization::UI::Hotkeys::schemeName).toString();
        const auto existingScheme = this->getResourceById(schemeName);

        HotkeyScheme::Ptr scheme(existingScheme != nullptr ?
            static_cast<HotkeyScheme *>(existingScheme.get()) : new HotkeyScheme());
        scheme->deserialize(schemeNode);

        outResources[scheme->getResourceId()] = scheme;
    }

    this->activeScheme = this->findActiveScheme();
    jassert(this->activeScheme != nullptr);
}

void HotkeySchemesCollection::reset()
{
    ConfigurationResourceCollection::reset();
    this->activeScheme = nullptr;
}

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
#include "HotkeySchemesManager.h"
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "App.h"
#include "Config.h"

HotkeySchemesManager::HotkeySchemesManager() :
    ResourceManager(Serialization::Resources::hotkeySchemes) {}

HotkeyScheme::Ptr HotkeySchemesManager::findActiveScheme() const
{
    if (Config::contains(Serialization::Config::activeHotkeyScheme))
    {
        Config::load(this->activeScheme.get(), Serialization::Config::activeHotkeyScheme);
    }

    if (const auto firstScheme = this->getSchemes().getFirst())
    {
        return firstScheme;
    }

    jassertfalse;
    return { new HotkeyScheme() };
}

const HotkeyScheme::Ptr HotkeySchemesManager::getCurrentScheme() const noexcept
{
    jassert(this->activeScheme != nullptr);
    return this->activeScheme;
}

void HotkeySchemesManager::setCurrentScheme(const HotkeyScheme::Ptr scheme)
{
    jassert(scheme != nullptr);
    this->activeScheme = scheme;
    Config::save(this->activeScheme.get(), Serialization::Config::activeHotkeyScheme);
}

void HotkeySchemesManager::deserializeResources(const ValueTree &tree, Resources &outResources)
{
    const auto root = tree.hasType(Serialization::Resources::hotkeySchemes) ?
        tree : tree.getChildWithName(Serialization::Resources::hotkeySchemes);

    if (!root.isValid()) { return; }

    forEachValueTreeChildWithType(root, schemeNode, Serialization::UI::Hotkeys::scheme)
    {
        HotkeyScheme::Ptr hs(new HotkeyScheme());
        hs->deserialize(schemeNode);
        outResources[hs->getResourceId()] = hs;
    }

    this->activeScheme = this->findActiveScheme();
    jassert(this->activeScheme != nullptr);
}

void HotkeySchemesManager::reset()
{
    ResourceManager::reset();
    this->activeScheme = nullptr;
}

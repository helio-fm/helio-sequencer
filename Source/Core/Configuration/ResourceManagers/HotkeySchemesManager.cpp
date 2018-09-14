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

    if (this->resources.size() > 0)
    {
        Resources::Iterator i(this->resources);
        i.next();
        return  { static_cast<HotkeyScheme *>(i.getValue().get()) };
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

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree HotkeySchemesManager::serialize() const
{
    ValueTree tree(Serialization::UI::Hotkeys::schemes);
    
    Resources::Iterator i(this->resources);
    while (i.next())
    {
        tree.appendChild(i.getValue()->serialize(), nullptr);
    }
    
    return tree;
}

void HotkeySchemesManager::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::UI::Hotkeys::schemes) ?
        tree : tree.getChildWithName(Serialization::UI::Hotkeys::schemes);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, schemeNode, Serialization::UI::Hotkeys::scheme)
    {
        HotkeyScheme::Ptr hs(new HotkeyScheme());
        hs->deserialize(schemeNode);
        this->resources.set(hs->getResourceId(), hs);
    }

    this->activeScheme = this->findActiveScheme();
    jassert(this->activeScheme != nullptr);
}

void HotkeySchemesManager::reset()
{
    ResourceManager::reset();
    this->activeScheme = nullptr;
}

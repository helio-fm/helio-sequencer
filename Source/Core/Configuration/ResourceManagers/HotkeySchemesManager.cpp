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

void HotkeySchemesManager::initialise(const String &commandLine)
{
    this->reloadResources();

    if (Config::contains(Serialization::Config::activeHotkeyScheme))
    {
        Config::load(this->activeScheme, Serialization::Config::activeHotkeyScheme);
    }
    else
    {
        this->activeScheme = this->schemes[0];
    }
}

void HotkeySchemesManager::shutdown()
{
    this->reset();
}

const Array<HotkeyScheme> &HotkeySchemesManager::getSchemes() const noexcept
{
    return this->schemes;
}

const HotkeyScheme &HotkeySchemesManager::getCurrentScheme() const noexcept
{
    return this->activeScheme;
}

void HotkeySchemesManager::setCurrentScheme(const HotkeyScheme &scheme)
{
    this->activeScheme = scheme;
    Config::save(this->activeScheme, Serialization::Config::activeHotkeyScheme);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree HotkeySchemesManager::serialize() const
{
    ValueTree tree(Serialization::UI::Hotkeys::schemes);
    
    for (int i = 0; i < this->schemes.size(); ++i)
    {
        tree.appendChild(this->schemes.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void HotkeySchemesManager::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto root = tree.hasType(Serialization::UI::Hotkeys::schemes) ?
        tree : tree.getChildWithName(Serialization::UI::Hotkeys::schemes);
    
    if (!root.isValid()) { return; }
    
    forEachValueTreeChildWithType(root, schemeNode, Serialization::UI::Hotkeys::scheme)
    {
        HotkeyScheme cs;
        cs.deserialize(schemeNode);
        this->schemes.add(cs);
    }
    
    this->sendChangeMessage();
}

void HotkeySchemesManager::reset()
{
    this->schemes.clear();
    this->activeScheme.reset();
    this->sendChangeMessage();
}

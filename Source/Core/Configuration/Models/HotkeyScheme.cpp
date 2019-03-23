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
#include "HotkeyScheme.h"
#include "CommandIDs.h"
#include "SerializationKeys.h"
#include "ResourceCache.h"

HotkeyScheme::HotkeyScheme(const HotkeyScheme &other)
{
    operator= (other);
}

String HotkeyScheme::findHotkeyDescription(int commandId) const noexcept
{
    for (const auto &key : this->keyPresses)
    {
        if (key.commandId == commandId)
        {
            return key.keyPress.getTextDescriptionWithIcons();
        }
    }

    return {};
}

bool HotkeyScheme::dispatchKeyPress(KeyPress keyPress,
    WeakReference<Component> keyPressReceiver,
    WeakReference<Component> messageReceiver)
{
    for (const auto &key : this->keyPresses)
    {
        if (keyPress == key.keyPress)
        {
            if (this->sendHotkeyCommand(key, keyPressReceiver, messageReceiver))
            {
                return true;
            }
        }
    }

    return false;
}

bool HotkeyScheme::dispatchKeyStateChange(bool isKeyDown,
    WeakReference<Component> keyPressReceiver,
    WeakReference<Component> messageReceiver)
{
    // TODO test multiple key-downs:

    if (isKeyDown)
    {
        for (const auto &key : this->keyDowns)
        {
            if (key.keyPress.isCurrentlyDown() &&
                this->sendHotkeyCommand(key, keyPressReceiver, messageReceiver))
            {
                this->holdKeys.addIfNotAlreadyThere(key.keyPress);
                return true;
            }
        }
    }
    else
    {
        for (const auto &key : this->keyUps)
        {
            if (!key.keyPress.isCurrentlyDown() && 
                this->holdKeys.contains(key.keyPress) &&
                this->sendHotkeyCommand(key, keyPressReceiver, messageReceiver))
            {
                this->holdKeys.removeAllInstancesOf(key.keyPress);
                return true;
            }
        }
    }

    return false;
}

static Component *findMessageReceiver(Component *root, const String &id)
{
    if (root->getComponentID() == id)
    {
        return root;
    }

    for (auto *child : root->getChildren())
    {
        if (Component *found = findMessageReceiver(child, id))
        {
            return found;
        }
    }

    return nullptr;
}

bool HotkeyScheme::sendHotkeyCommand(Hotkey key,
    WeakReference<Component> keyPressReceiver,
    WeakReference<Component> messageReceiver)
{
    if (messageReceiver != this->lastReceiver)
    {
        this->lastReceiver = messageReceiver;
        this->receiverChildren.clear();
    }

    WeakReference<Component> receiver = this->receiverChildren[key.componentId];
    if (receiver == nullptr)
    {
        if (keyPressReceiver != nullptr &&
            keyPressReceiver->getComponentID() == key.componentId)
        {
            receiver = keyPressReceiver;
        }
        else if (messageReceiver != nullptr)
        {
            receiver = findMessageReceiver(messageReceiver, key.componentId);
        }

        this->receiverChildren.set(key.componentId, receiver);
    }

    if (receiver != nullptr)
    {
        if (receiver->isEnabled() && receiver->isShowing())
        {
            receiver->postCommandMessage(key.commandId);
            return true;
        }
    }

    return false;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree HotkeyScheme::serialize() const
{
    ValueTree tree(Serialization::UI::Hotkeys::scheme);
    tree.setProperty(Serialization::UI::Hotkeys::schemeName, this->name, nullptr);
    // Not implemented (cannot convert command id's to string messages back)
    return tree;
}

static inline HotkeyScheme::Hotkey createHotkey(const ValueTree &e)
{
    HotkeyScheme::Hotkey key;
    auto keyPressDesc = e.getProperty(Serialization::UI::Hotkeys::hotkeyDescription);
    auto receiver = e.getProperty(Serialization::UI::Hotkeys::hotkeyReceiver);
    auto command = e.getProperty(Serialization::UI::Hotkeys::hotkeyCommand);
    key.keyPress = KeyPress::createFromDescription(keyPressDesc);
    key.commandId = CommandIDs::getIdForName(command);
    key.componentId = receiver;
    return key;
}

void HotkeyScheme::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root =
        tree.hasType(Serialization::UI::Hotkeys::scheme) ?
        tree : tree.getChildWithName(Serialization::UI::Hotkeys::scheme);

    if (!root.isValid())
    {
        return;
    }

    this->name = root.getProperty(Serialization::UI::Hotkeys::schemeName);

    forEachValueTreeChildWithType(root, e, Serialization::UI::Hotkeys::keyPress)
    {
        this->keyPresses.add(createHotkey(e));
    }

    forEachValueTreeChildWithType(root, e, Serialization::UI::Hotkeys::keyDown)
    {
        this->keyDowns.add(createHotkey(e));
    }

    forEachValueTreeChildWithType(root, e, Serialization::UI::Hotkeys::keyUp)
    {
        this->keyUps.add(createHotkey(e));
    }
}

void HotkeyScheme::reset()
{
    this->name.clear();
    this->keyPresses.clearQuick();
    this->keyDowns.clearQuick();
    this->keyUps.clearQuick();
    this->receiverChildren.clear();
    this->holdKeys.clearQuick();
    this->lastReceiver = nullptr;
}

HotkeyScheme &HotkeyScheme::operator=(const HotkeyScheme &other)
{
    this->name = other.name;
    this->keyPresses.clearQuick();
    this->keyDowns.clearQuick();
    this->keyUps.clearQuick();
    this->keyPresses.addArray(other.keyPresses);
    this->keyDowns.addArray(other.keyDowns);
    this->keyUps.addArray(other.keyUps);
    return *this;
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String HotkeyScheme::getResourceId() const noexcept
{
    return this->name;
}

Identifier HotkeyScheme::getResourceType() const noexcept
{
    return Serialization::Resources::hotkeySchemes;
}

bool operator==(const HotkeyScheme &l, const HotkeyScheme &r)
{
    return &l == &r || l.name == r.name;
}

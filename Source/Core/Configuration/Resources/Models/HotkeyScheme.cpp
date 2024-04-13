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
#include "HotkeyScheme.h"
#include "SerializationKeys.h"

bool operator==(const HotkeyScheme::Hotkey &lhs, const HotkeyScheme::Hotkey &rhs)
{
    return lhs.keyPress == rhs.keyPress &&
        lhs.componentId == rhs.componentId;
}

HotkeyScheme::HotkeyScheme(const HotkeyScheme &other)
{
    operator= (other);
}

String HotkeyScheme::findHotkeyDescription(int commandId) const noexcept
{
    for (const auto &key : this->keyPresses)
    {
        if (key.second == commandId)
        {
            return key.first.keyPress.getTextDescriptionWithIcons();
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
        if (key.first.keyPress == keyPress)
        {
            if (this->sendHotkeyCommand(key.first.componentId, key.second, keyPressReceiver, messageReceiver))
            {
                this->lastKeyPress = keyPress;
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
            if (key.first.keyPress.isCurrentlyDown() &&
                this->sendHotkeyCommand(key.first.componentId, key.second, keyPressReceiver, messageReceiver))
            {
                this->holdKeys.insert(key.first.keyPress);
                return true;
            }
        }
    }
    else
    {
        for (const auto &key : this->keyUps)
        {
            if (!key.first.keyPress.isCurrentlyDown() && 
                this->holdKeys.contains(key.first.keyPress) &&
                this->sendHotkeyCommand(key.first.componentId, key.second, keyPressReceiver, messageReceiver))
            {
                this->holdKeys.erase(key.first.keyPress);
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
        if (auto *found = findMessageReceiver(child, id))
        {
            return found;
        }
    }

    return nullptr;
}

bool HotkeyScheme::sendHotkeyCommand(const String &componentId,
    CommandIDs::Id commandId,
    WeakReference<Component> keyPressReceiver,
    WeakReference<Component> messageReceiver)
{
    if (messageReceiver != this->lastReceiver)
    {
        this->lastReceiver = messageReceiver;
        this->receiverChildren.clear();
    }

    auto receiver = this->receiverChildren[componentId];
    if (receiver == nullptr)
    {
        if (keyPressReceiver != nullptr &&
            keyPressReceiver->getComponentID() == componentId)
        {
            // main layout itself
            receiver = keyPressReceiver;
        }
        else if (messageReceiver != nullptr)
        {
            // child of the showing page
            receiver = findMessageReceiver(messageReceiver, componentId);
        }

        this->receiverChildren[componentId] = receiver;
    }

    if (receiver != nullptr)
    {
        if (receiver->isEnabled() && receiver->isShowing())
        {
            receiver->postCommandMessage(commandId);
            return true;
        }
    }

    return false;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData HotkeyScheme::serialize() const
{
    SerializedData tree(Serialization::UI::Hotkeys::scheme);
    tree.setProperty(Serialization::UI::Hotkeys::schemeName, this->name);
    jassert(false); // not implemented
    return tree;
}

static inline HotkeyScheme::Hotkey deserializeHotkey(const SerializedData &e, const String &receiver = "")
{
    HotkeyScheme::Hotkey result;
    
    const auto keyPressDesc =
        e.getProperty(Serialization::UI::Hotkeys::hotkeyDescription);

    result.keyPress = KeyPress::createFromDescription(keyPressDesc);
    result.componentId = receiver.isNotEmpty() ? receiver :
        e.getProperty(Serialization::UI::Hotkeys::hotkeyReceiver).toString();

    return result;
}

static inline CommandIDs::Id deserializeCommand(const SerializedData &e)
{
    const auto command = e.getProperty(Serialization::UI::Hotkeys::hotkeyCommand);
    return CommandIDs::getIdForName(command);
}

void HotkeyScheme::deserialize(const SerializedData &data)
{
    // don't reset so that user's scheme appends
    // the built-in one instead of replacing it
    // this->reset();

    using namespace Serialization::UI;

    const auto root =
        data.hasType(Hotkeys::scheme) ?
        data : data.getChildWithName(Hotkeys::scheme);

    if (!root.isValid())
    {
        return;
    }

    this->name = root.getProperty(Hotkeys::schemeName);

    // plain format:

    forEachChildWithType(root, e, Hotkeys::keyPress)
    {
        this->keyPresses[deserializeHotkey(e)] = deserializeCommand(e);
    }

    forEachChildWithType(root, e, Hotkeys::keyDown)
    {
        this->keyDowns[deserializeHotkey(e)] = deserializeCommand(e);
    }

    forEachChildWithType(root, e, Hotkeys::keyUp)
    {
        this->keyUps[deserializeHotkey(e)] = deserializeCommand(e);
    }

    // grouped format:

    forEachChildWithType(root, group, Hotkeys::group)
    {
        const auto receiver = group.getProperty(Hotkeys::hotkeyReceiver);

        forEachChildWithType(group, e, Hotkeys::keyPress)
        {
            this->keyPresses[deserializeHotkey(e, receiver)] = deserializeCommand(e);
        }

        forEachChildWithType(group, e, Hotkeys::keyDown)
        {
            this->keyDowns[deserializeHotkey(e, receiver)] = deserializeCommand(e);
        }

        forEachChildWithType(group, e, Hotkeys::keyUp)
        {
            this->keyUps[deserializeHotkey(e, receiver)] = deserializeCommand(e);
        }
    }
}

void HotkeyScheme::reset()
{
    this->name.clear();
    this->keyPresses.clear();
    this->keyDowns.clear();
    this->keyUps.clear();
    this->receiverChildren.clear();
    this->holdKeys.clear();
    this->lastReceiver = nullptr;
}

HotkeyScheme &HotkeyScheme::operator=(const HotkeyScheme &other)
{
    this->name = other.name;
    this->keyPresses.clear();
    this->keyDowns.clear();
    this->keyUps.clear();
    this->keyPresses.insert(other.keyPresses.begin(), other.keyPresses.end());
    this->keyDowns.insert(other.keyDowns.begin(), other.keyDowns.end());
    this->keyUps.insert(other.keyUps.begin(), other.keyUps.end());
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

KeyPress HotkeyScheme::getLastKeyPress() const noexcept
{
    return this->lastKeyPress;
}

const HotkeyScheme::HotkeyMap &HotkeyScheme::getKeyPresses() const noexcept
{
    return this->keyPresses;
}

bool operator==(const HotkeyScheme &l, const HotkeyScheme &r)
{
    return &l == &r || l.name == r.name;
}

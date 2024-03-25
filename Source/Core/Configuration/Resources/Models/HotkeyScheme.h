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

#pragma once

#include "ConfigurationResource.h"
#include "CommandIDs.h"

class HotkeyScheme final : public ConfigurationResource
{
public:

    HotkeyScheme() = default;
    HotkeyScheme(const HotkeyScheme &other);

    using Ptr = ReferenceCountedObjectPtr<HotkeyScheme>;

    struct Hotkey final
    {
        KeyPress keyPress;
        String componentId;
        friend bool operator==(const Hotkey &lhs, const Hotkey &rhs);
    };

    struct HotkeyHash
    {
        inline HashCode operator()(const Hotkey &key) const noexcept
        {
            return static_cast<HashCode>(key.keyPress.getKeyCode() + key.componentId.hashCode());
        }
    };

    String findHotkeyDescription(int commandId) const noexcept;

    // To be used by command palette:
    using HotkeyMap = FlatHashMap<Hotkey, CommandIDs::Id, HotkeyHash>;
    const HotkeyMap &getKeyPresses() const noexcept;
    KeyPress getLastKeyPress() const noexcept;

    bool dispatchKeyPress(KeyPress keyPress,
        WeakReference<Component> keyPressReceiver,
        WeakReference<Component> messageReceiverParent);

    bool dispatchKeyStateChange(bool isKeyDown,
        WeakReference<Component> keyPressReceiver,
        WeakReference<Component> messageReceiverParent);

    HotkeyScheme &operator=(const HotkeyScheme &other);
    friend bool operator==(const HotkeyScheme &lhs, const HotkeyScheme &rhs);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // BaseResource
    //===------------------------------------------------------------------===//

    String getResourceId() const noexcept override;
    Identifier getResourceType() const noexcept override;

private:

    String name;
    KeyPress lastKeyPress;

    HotkeyMap keyPresses;
    HotkeyMap keyDowns;
    HotkeyMap keyUps;

    struct KeyPressHash
    {
        inline HashCode operator()(const KeyPress &key) const noexcept
        {
            return static_cast<HashCode>(key.getKeyCode());
        }
    };

    FlatHashSet<KeyPress, KeyPressHash> holdKeys;

    WeakReference<Component> lastReceiver;
    FlatHashMap<String, WeakReference<Component>, StringHash> receiverChildren;

    bool sendHotkeyCommand(const String &componentId, CommandIDs::Id commandId,
        WeakReference<Component> root, WeakReference<Component> target);

    JUCE_LEAK_DETECTOR(HotkeyScheme)
};

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

#pragma once

#include "BaseResource.h"

class HotkeyScheme final : public BaseResource
{
public:

    HotkeyScheme();
    HotkeyScheme(const HotkeyScheme &other);

    typedef ReferenceCountedObjectPtr<HotkeyScheme> Ptr;

    class Hotkey final
    {
    public:
        KeyPress keyPress;
        String componentId;
        int commandId;
    };

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

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // BaseResource
    //===------------------------------------------------------------------===//

    String getResourceId() const override;

private:

    String name;
    Array<Hotkey> keyPresses;
    Array<Hotkey> keyDowns;
    Array<Hotkey> keyUps;

    WeakReference<Component> lastReceiver;
    HashMap<String, WeakReference<Component>> receiverChildren;

    bool sendHotkeyCommand(Hotkey key,
        WeakReference<Component> root,
        WeakReference<Component> target);

    JUCE_LEAK_DETECTOR(HotkeyScheme)
};

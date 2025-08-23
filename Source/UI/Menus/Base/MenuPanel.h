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

#include "MenuItemComponent.h"
#include "ComponentFader.h"

struct MenuPanelUtils final
{
    static void disableKeyboardFocusForAllChildren(Component *panel)
    {
        for (auto *child : panel->getChildren())
        {
            child->setFocusContainerType(Component::FocusContainerType::none);
            child->setWantsKeyboardFocus(false);
            child->setMouseClickGrabsKeyboardFocus(false);
        }
    }
};

class MenuPanel : public Component, private ListBoxModel
{
public:

    MenuPanel();

    enum AnimationType
    {
        None,
        Fading,
        SlideLeft,
        SlideRight,
        SlideUp,
        SlideDown
    };

    using Menu = ReferenceCountedArray<MenuItem>;

    inline int getMenuSize() const noexcept
    {
        return this->menu.size();
    }

    inline int getMenuHeight() const noexcept
    {
        return this->getMenuSize() * Globals::UI::menuPanelRowHeight;
    }

    void updateContent(const Menu &commands,
        AnimationType animationType = SlideDown,
        bool adjustsWidth = true,
        Component *customFooter = nullptr);
    
    void applyFilter(const String &text);

    void resized() override;

    // when overriding this, put MenuPanel::handleCommandMessage(commandId)
    // in the default switch case to handle menu navigation commands:
    void handleCommandMessage(int commandId) override;

    int indexOfItemNamed(const String &name);
    const MenuItem::Ptr getMenuItem(int index);

    void scrollToItem(int index);

private:

    friend class MenuItemComponent;

    void dismiss()
    {
        // there's a chance some component will be calling dismiss() on mouseDown,
        // let's give them time to process mouse messages before dismissing:
        MessageManager::callAsync([self = WeakReference<Component>(this)]()
        {
            if (self == nullptr) { return; }
            // assumes being owned by a modal component:
            if (auto *parent = self->getParentComponent())
            {
                parent->exitModalState(0);
            }
        });
    }

private:

    ComponentFader animator;

    bool shouldResizeToFitContent = false;
    AnimationType lastAnimationType = AnimationType::None;

    Menu menu;
    Menu filteredMenu;
    inline Menu &getMenuOrFiltered();

    bool moveCursor(int delta);
    void doActionAtCursor();
    Optional<int> cursorPosition;
    FlatHashMap<String, int, StringHash> recentCursorPositions;
    void postCommandMessageToParent(int commandId);

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, Graphics &g,
        int width, int height, bool rowIsSelected) noexcept override {}
    Component *refreshComponentForRow(int rowNumber, bool isRowSelected,
        Component *existingComponentToUpdate) override;

    UniquePointer<ListBox> listBox;
    UniquePointer<Component> customFooter;

    Rectangle<int> getMenuBounds() const;
    Rectangle<int> getFooterBounds() const;
    int getFooterHeight() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuPanel)
};

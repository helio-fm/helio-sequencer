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

#include "MenuItemComponent.h"

class MenuPanelUtils final
{
public:

    static void disableKeyboardFocusForAllChildren(Component *panel)
    {
        for (auto *child : panel->getChildren())
        {
            child->setFocusContainer(false);
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

    void updateContent(const Menu &commands,
        AnimationType animationType = SlideDown,
        bool adjustsWidth = true,
        Component *customFooter = nullptr);
    
    void applyFilter(const String &text);

    void resized() override;
    void handleCommandMessage(int commandId) override;

private:

    friend class MenuItemComponent;

    virtual void dismiss() const
    {
        // assumes being owned by a modal component:
        if (auto *parent = this->getParentComponent())
        {
            parent->exitModalState(0);
        }
    }

private:

    ComponentAnimator animator;

    bool shouldResizeToFitContent = false;
    AnimationType lastAnimationType = AnimationType::None;

    Menu menu;
    Menu filteredMenu;

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

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

#if HELIO_DESKTOP
#   define COMMAND_PANEL_BUTTON_HEIGHT (32)
#elif HELIO_MOBILE
#   define COMMAND_PANEL_BUTTON_HEIGHT (40)
#endif

class MenuPanel : public Component, private ListBoxModel
{
public:

    MenuPanel();

    enum AnimationType
    {
        None            = 0x000,
        Fading          = 0x010,
        SlideLeft       = 0x020,
        SlideRight      = 0x030,
        SlideUp         = 0x040,
        SlideDown       = 0x050,
    };

    using Menu = ReferenceCountedArray<MenuItem>;

    void updateContent(const Menu &commands,
        AnimationType animationType = SlideDown,
        bool adjustsWidth = true,
        Component *customFooter = nullptr);

    static StringPairArray getColoursList();
    
    void resized() override;
    void handleCommandMessage (int commandId) override;

protected:

    friend class MenuItemComponent;

    virtual void dismiss() const
    {
        if (auto parent = this->getParentComponent())
        {
            parent->exitModalState(0);
        }
    }

private:

    ComponentAnimator animator;

    bool shouldResizeToFitContent;
    AnimationType lastAnimationType;
    Menu commandDescriptions;

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

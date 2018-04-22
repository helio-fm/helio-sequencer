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

    typedef ReferenceCountedArray<MenuItem> Menu;

    void updateContent(const Menu &commands,
        AnimationType animationType = SlideDown,
        bool adjustsWidth = true);

    static StringPairArray getColoursList();
    
    void resized() override;
    void handleCommandMessage (int commandId) override;

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

    ScopedPointer<ListBox> listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuPanel)
};

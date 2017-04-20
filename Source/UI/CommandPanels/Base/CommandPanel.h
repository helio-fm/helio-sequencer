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

//[Headers]
#include "CommandItemComponent.h"

#if HELIO_DESKTOP
#   define COMMAND_PANEL_BUTTON_HEIGHT (32)
#elif HELIO_MOBILE
#   define COMMAND_PANEL_BUTTON_HEIGHT (40)
#endif

//[/Headers]

#include "../../Themes/PanelBackgroundC.h"

class CommandPanel  : public Component,
                      private ListBoxModel
{
public:

    CommandPanel ();

    ~CommandPanel();

    //[UserMethods]
    enum AnimationType
    {
        None            = 0x000,
        Fading          = 0x010,
        SlideLeft       = 0x020,
        SlideRight      = 0x030,
        SlideUp         = 0x040,
        SlideDown       = 0x050,
    };

    void updateContent(ReferenceCountedArray<CommandItem> commands, AnimationType animationType = SlideRight);

    static StringPairArray getColoursList();

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    ComponentAnimator animator;

    ReferenceCountedArray<CommandItem> commandDescriptions;


    //===----------------------------------------------------------------------===//
    // ListBoxModel
    //

    int getNumRows() override;

    void paintListBoxItem(int rowNumber,
                                  Graphics &g,
                                  int width, int height,
                                  bool rowIsSelected) override;

    Component *refreshComponentForRow(int rowNumber, bool isRowSelected,
                                              Component *existingComponentToUpdate) override;

    void listWasScrolled() override;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> component;
    ScopedPointer<ListBox> listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CommandPanel)
};

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
#include "ColourScheme.h"
#include "MobileComboBox.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontalFading.h"
#include "../../Themes/SeparatorHorizontalFading.h"

class UserInterfaceSettings final : public Component,
                                    public ListBoxModel,
                                    private ChangeListener,
                                    public Button::Listener
{
public:

    UserInterfaceSettings();
    ~UserInterfaceSettings();

    //[UserMethods]

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    Component *refreshComponentForRow(int, bool, Component*) override;
    int getNumRows() override;
    void paintListBoxItem(int, Graphics&, int, int, bool) override {}
    void listBoxItemClicked(int, const MouseEvent &) override {}

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void visibilityChanged() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;
    void updateButtons();

    ColourScheme::Ptr currentScheme;
    Array<Font> systemFonts;

    //[/UserVariables]

    ScopedPointer<MobileComboBox::Primer> fontComboPrimer;
    ScopedPointer<ListBox> themesList;
    ScopedPointer<ToggleButton> openGLRendererButton;
    ScopedPointer<ToggleButton> defaultRendererButton;
    ScopedPointer<SeparatorHorizontalFading> separator2;
    ScopedPointer<TextEditor> fontEditor;
    ScopedPointer<SeparatorHorizontalFading> separator3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UserInterfaceSettings)
};

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
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Themes/ShadowRightwards.h"

class AutomationsCommandPanel  : public Component,
                                 private ListBoxModel
{
public:

    AutomationsCommandPanel ();

    ~AutomationsCommandPanel();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;
    void childrenChanged() override;
    void mouseMove (const MouseEvent& e) override;

    // Binary resources:
    static const char* gray1x1_png;
    static const int gray1x1_pngSize;

private:

    //[UserVariables]

    ReferenceCountedArray<CommandItem> commandDescriptions;

    void recreateCommandDescriptions();


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


    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> component;
    ScopedPointer<ShadowRightwards> rightwardsShadow;
    ScopedPointer<ListBox> listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationsCommandPanel)
};

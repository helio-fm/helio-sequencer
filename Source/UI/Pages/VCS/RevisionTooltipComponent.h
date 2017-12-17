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
#include "VersionControl.h"
#include "Revision.h"
//[/Headers]

#include "../../Themes/PanelBackgroundC.h"
#include "../../Themes/FramePanel.h"
#include "../../Themes/ShadowDownwards.h"

class RevisionTooltipComponent  : public Component,
                                  public ListBoxModel,
                                  public Button::Listener
{
public:

    RevisionTooltipComponent (VersionControl &owner, const ValueTree revision);

    ~RevisionTooltipComponent();

    //[UserMethods]

    //===----------------------------------------------------------------------===//
    // ListBoxModel
    //

    Component *refreshComponentForRow(int rowNumber,
            bool isRowSelected, Component *existingComponentToUpdate) override;

    void listBoxItemClicked(int row, const MouseEvent &e) override;

    void listBoxItemDoubleClicked(int row, const MouseEvent &e) override;

    int getNumRows() override;

    void paintListBoxItem(int rowNumber, Graphics &g,
                                  int width, int height, bool rowIsSelected) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    void hide();

    VersionControl &vcs;

    const ValueTree revision;

    ValueTree revisionItemsOnly;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> background;
    ScopedPointer<FramePanel> panel;
    ScopedPointer<ListBox> changesList;
    ScopedPointer<TextButton> checkoutRevisionButton;
    ScopedPointer<ShadowDownwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RevisionTooltipComponent)
};

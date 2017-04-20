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
#include "FadingDialog.h"

class DocumentOwner;
class ProjectTreeItem;
class ProgressIndicator;
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/PanelA.h"
#include "../Themes/ShadowDownwards.h"

class UpdateDialog  : public FadingDialog,
                      public ButtonListener,
                      public LabelListener
{
public:

    UpdateDialog ();

    ~UpdateDialog();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void labelTextChanged (Label* labelThatHasChanged) override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]
    //[/UserVariables]

    ScopedPointer<PanelC> background;
    ScopedPointer<PanelA> panel;
    ScopedPointer<TextButton> updateButton;
    ScopedPointer<Label> titleLabel;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<Label> descriptionLabel1;
    ScopedPointer<Label> descriptionLabel2;
    ScopedPointer<Label> installedVersionLabel;
    ScopedPointer<Label> availableVersionLabel;
    ScopedPointer<TextButton> forceUpdateButton;
    ScopedPointer<ShadowDownwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdateDialog)
};

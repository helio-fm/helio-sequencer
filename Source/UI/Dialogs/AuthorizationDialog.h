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
#include "IconComponent.h"
#include "FadingDialog.h"

class DocumentOwner;
class LabelWithPassword;
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class AuthorizationDialog  : public FadingDialog,
                             private ChangeListener,
                             public Button::Listener,
                             public Label::Listener
{
public:

    AuthorizationDialog ();

    ~AuthorizationDialog();

    //[UserMethods]

    void showImport(DocumentOwner *owner);
    void login();

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
    void editorShown(Label *targetLabel, TextEditor &editor) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    bool validateTextFields() const;
    //[/UserVariables]

    ScopedPointer<PanelC> background;
    ScopedPointer<TextButton> loginButton;
    ScopedPointer<Label> emailEditor;
    ScopedPointer<Label> emailLabel;
    ScopedPointer<Label> passwordLabel;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<LabelWithPassword> passwordEditor;
    ScopedPointer<SeparatorHorizontal> separatorH;
    ScopedPointer<SeparatorVertical> separatorV;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuthorizationDialog)
};

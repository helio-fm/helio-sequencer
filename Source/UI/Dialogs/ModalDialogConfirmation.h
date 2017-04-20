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
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/PanelA.h"
#include "../Themes/ShadowDownwards.h"

class ModalDialogConfirmation  : public FadingDialog,
                                 public ButtonListener
{
public:

    ModalDialogConfirmation (Component &owner, const String &message, const String &okText, const String &cancelText, int okCode, int cancelCode);

    ~ModalDialogConfirmation();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    Component &ownerComponent;

    int okCommand;
    int cancelCommand;

    void cancel()
    {
        this->ownerComponent.postCommandMessage(this->cancelCommand);
        this->disappear();
    }

    void okay()
    {
        this->ownerComponent.postCommandMessage(this->okCommand);
        this->disappear();
    }

    void disappear()
    { delete this; }

    //[/UserVariables]

    ScopedPointer<PanelC> background;
    ScopedPointer<PanelA> panel;
    ScopedPointer<Label> messageLabel;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<ShadowDownwards> component2;
    ScopedPointer<TextButton> okButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalDialogConfirmation)
};

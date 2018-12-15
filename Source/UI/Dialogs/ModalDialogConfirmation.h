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

using SimpleDialogCallback = Function<void()>;
//[/Headers]

#include "../Themes/DialogPanel.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class ModalDialogConfirmation final : public FadingDialog,
                                      public Button::Listener
{
public:

    ModalDialogConfirmation(const String &message, const String &okText, const String &cancelText);
    ~ModalDialogConfirmation();

    //[UserMethods]
    SimpleDialogCallback onOk;
    SimpleDialogCallback onCancel;

    struct Presets final
    {
        static ScopedPointer<ModalDialogConfirmation> deleteProject();
        static ScopedPointer<ModalDialogConfirmation> forceCheckout();
        static ScopedPointer<ModalDialogConfirmation> resetChanges();
        static ScopedPointer<ModalDialogConfirmation> confirmOpenGL();
    };
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
    void cancel();
    void okay();
    //[/UserVariables]

    ScopedPointer<DialogPanel> background;
    ScopedPointer<Label> messageLabel;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<TextButton> okButton;
    ScopedPointer<SeparatorHorizontal> separatorH;
    ScopedPointer<SeparatorVertical> separatorV;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalDialogConfirmation)
};

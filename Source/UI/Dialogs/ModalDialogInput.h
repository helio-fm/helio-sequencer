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

using InputDialogCallback = Function<void(const String &result)>;
//[/Headers]

#include "../Themes/DialogPanel.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class ModalDialogInput final : public FadingDialog,
                               public TextEditor::Listener,
                               private Timer,
                               public Button::Listener
{
public:

    ModalDialogInput(const String &text, const String &message, const String &okText, const String &cancelText);
    ~ModalDialogInput();

    //[UserMethods]
    InputDialogCallback onOk;
    InputDialogCallback onCancel;

    struct Presets final
    {
        static ScopedPointer<ModalDialogInput> renameAnnotation(const String &name);
        static ScopedPointer<ModalDialogInput> renameInstrument(const String &name);
        static ScopedPointer<ModalDialogInput> changeTimeSignature(const String &name);
        static ScopedPointer<ModalDialogInput> renameTrack(const String &name);
        static ScopedPointer<ModalDialogInput> newTrack();
        static ScopedPointer<ModalDialogInput> newArpeggiator();
        static ScopedPointer<ModalDialogInput> deleteProjectConfirmation();
        static ScopedPointer<ModalDialogInput> commit(const String &name);
    };
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]
    String input;

    void cancel();
    void okay();

    void updateOkButtonState();
    void timerCallback() override;

    void textEditorTextChanged(TextEditor &editor) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;
    //[/UserVariables]

    ScopedPointer<DialogPanel> background;
    ScopedPointer<Label> messageLabel;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<TextButton> okButton;
    ScopedPointer<TextEditor> textEditor;
    ScopedPointer<SeparatorHorizontal> separatorH;
    ScopedPointer<SeparatorVertical> separatorV;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalDialogInput)
};

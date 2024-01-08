/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "DialogBase.h"

using InputDialogCallback = Function<void(const String &result)>;

class ModalDialogInput final : public DialogBase, public TextEditor::Listener
{
public:

    ModalDialogInput(const String &text, const String &message,
        const String &okText, const String &cancelText,
        const String &inputToRequire = {});

    ~ModalDialogInput();

    InputDialogCallback onOk;
    InputDialogCallback onCancel;

    struct Presets final
    {
        static UniquePointer<ModalDialogInput> renameAnnotation(const String &name);
        static UniquePointer<ModalDialogInput> renameInstrument(const String &name);
        static UniquePointer<ModalDialogInput> changeTimeSignature(const String &name);
        static UniquePointer<ModalDialogInput> renameTrack(const String &name);
        static UniquePointer<ModalDialogInput> newTrack();
        static UniquePointer<ModalDialogInput> newArpeggiator(const String &name);
        static UniquePointer<ModalDialogInput> deleteProjectConfirmation(const String &projectNameCheck);
        static UniquePointer<ModalDialogInput> commit(const String &name);
        static UniquePointer<ModalDialogInput> savePreset();
    };

    void resized() override;
    void parentHierarchyChanged() override;
    void visibilityChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;

private:

    String input;

    const String inputToRequire;

    void cancel();
    void okay();

    void updateOkButtonState();

    void textEditorTextChanged(TextEditor &editor) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    Component *getPrimaryFocusTarget() override;

    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> cancelButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalDialogInput)
};

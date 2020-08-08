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

#include "DialogBase.h"
#include "TimeSignatureEvent.h"
#include "MobileComboBox.h"

class TimeSignaturesSequence;

class TimeSignatureDialog final : public DialogBase, public TextEditor::Listener
{
public:

    TimeSignatureDialog(Component &owner,
        TimeSignaturesSequence *timeSequence, const TimeSignatureEvent &editedEvent,
        bool shouldAddNewEvent, float targetBeat);

    ~TimeSignatureDialog();

    static UniquePointer<Component> editingDialog(Component &owner, const TimeSignatureEvent &event);
    static UniquePointer<Component> addingDialog(Component &owner, TimeSignaturesSequence *annotationsLayer, float targetBeat);

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;
    void inputAttemptWhenModal() override;

private:

    TimeSignatureEvent originalEvent;
    TimeSignaturesSequence *const originalSequence;
    Component &ownerComponent;

    const StringPairArray defailtMeters;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    inline void cancelAndDisappear();
    inline void updateOkButtonState();

    const bool addsNewEvent = false;
    bool hasMadeChanges = false;

    void sendEventChange(const TimeSignatureEvent &newEvent);
    void removeEvent();

    UniquePointer<MobileComboBox::Primer> comboPrimer;
    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> removeEventButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeSignatureDialog)
};

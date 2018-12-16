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
#include "TimeSignatureEvent.h"
#include "MobileComboBox.h"

class TimeSignaturesSequence;
//[/Headers]

#include "../Themes/DialogPanel.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class TimeSignatureDialog final : public FadingDialog,
                                  public TextEditor::Listener,
                                  private Timer,
                                  public Button::Listener
{
public:

    TimeSignatureDialog(Component &owner, TimeSignaturesSequence *timeSequence, const TimeSignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);
    ~TimeSignatureDialog();

    //[UserMethods]
    static TimeSignatureDialog *createEditingDialog(Component &owner, const TimeSignatureEvent &event);
    static TimeSignatureDialog *createAddingDialog(Component &owner, TimeSignaturesSequence *annotationsLayer, float targetBeat);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked(Button* buttonThatWasClicked) override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    TimeSignatureEvent originalEvent;
    TimeSignaturesSequence *originalSequence;
    Component &ownerComponent;

    const StringPairArray defailtMeters;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    void timerCallback() override;

    inline void cancelAndDisappear();
    inline void updateOkButtonState();

    bool addsNewEvent;
    bool hasMadeChanges;
    void sendEventChange(const TimeSignatureEvent &newEvent);
    void removeEvent();
    void cancelChangesIfAny();

    //[/UserVariables]

    UniquePointer<DialogPanel> background;
    UniquePointer<MobileComboBox::Primer> comboPrimer;
    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> removeEventButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<SeparatorHorizontal> separatorH;
    UniquePointer<SeparatorVertical> separatorV;
    UniquePointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeSignatureDialog)
};

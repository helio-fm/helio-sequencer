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
#include "DialogComboBox.h"

class TimeSignaturesSequence;
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class TimeSignatureDialog  : public FadingDialog,
                             public TextEditorListener,
                             private Timer,
                             public Button::Listener
{
public:

    TimeSignatureDialog (Component &owner, TimeSignaturesSequence *timeSequence, const TimeSignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);

    ~TimeSignatureDialog();

    //[UserMethods]
    static TimeSignatureDialog *createEditingDialog(Component &owner, const TimeSignatureEvent &event);
    static TimeSignatureDialog *createAddingDialog(Component &owner, TimeSignaturesSequence *annotationsLayer, float targetBeat);
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

    TimeSignatureEvent originalEvent;
    TimeSignaturesSequence *originalSequence;
    Component &ownerComponent;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    void timerCallback() override;

    inline void cancelAndDisappear();
    inline void disappear();
    inline void updateOkButtonState();

    bool addsNewEvent;
    bool hasMadeChanges;
    void sendEventChange(const TimeSignatureEvent &newEvent);
    void removeEvent();
    void cancelChangesIfAny();

    //[/UserVariables]

    ScopedPointer<PanelC> background;
    ScopedPointer<DialogComboBox::Primer> comboPrimer;
    ScopedPointer<Label> messageLabel;
    ScopedPointer<TextButton> removeEventButton;
    ScopedPointer<TextButton> okButton;
    ScopedPointer<SeparatorHorizontal> separatorH;
    ScopedPointer<SeparatorVertical> separatorV;
    ScopedPointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeSignatureDialog)
};

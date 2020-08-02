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
#include "DialogBase.h"
#include "AnnotationEvent.h"
#include "ColourButton.h"
#include "ColourSwatches.h"
#include "MobileComboBox.h"

class AnnotationsSequence;
//[/Headers]


class AnnotationDialog final : public DialogBase,
                               public TextEditor::Listener,
                               public ColourButton::Listener,
                               private Timer,
                               public Button::Listener
{
public:

    AnnotationDialog(Component &owner, AnnotationsSequence *sequence, const AnnotationEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);
    ~AnnotationDialog();

    //[UserMethods]
    static UniquePointer<Component> editingDialog(Component &owner, const AnnotationEvent &event);
    static UniquePointer<Component> addingDialog(Component &owner, AnnotationsSequence *annotationsLayer, float targetBeat);

    void onColourButtonClicked(ColourButton *button) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked(Button *buttonThatWasClicked) override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    AnnotationEvent originalEvent;
    AnnotationsSequence *const originalSequence;
    Component &ownerComponent;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    void timerCallback() override;

    inline void cancelAndDisappear();
    inline void updateOkButtonState();

    bool addsNewEvent;
    bool hasMadeChanges;
    void sendEventChange(const AnnotationEvent &newEvent);
    void removeEvent();

    //[/UserVariables]

    UniquePointer<MobileComboBox::Primer> comboPrimer;
    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> removeEventButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<ColourSwatches> colourSwatches;
    UniquePointer<TextEditor> textEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnnotationDialog)
};



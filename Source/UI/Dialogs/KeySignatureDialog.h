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
#include "KeySignatureEvent.h"
#include "ScaleEditor.h"
#include "KeySelector.h"
#include "MobileComboBox.h"

class Transport;
class ProjectNode;
class KeySignaturesSequence;
//[/Headers]

#include "../Common/PlayButton.h"

class KeySignatureDialog final : public DialogBase,
                                 public TextEditor::Listener,
                                 public ScaleEditor::Listener,
                                 public KeySelector::Listener,
                                 private Timer,
                                 public Button::Listener
{
public:

    KeySignatureDialog(ProjectNode &project, KeySignaturesSequence *keySequence, const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);
    ~KeySignatureDialog();

    //[UserMethods]
    static UniquePointer<Component> editingDialog(ProjectNode &project,
        const KeySignatureEvent &event);

    static UniquePointer<Component> addingDialog(ProjectNode &project,
        KeySignaturesSequence *annotationsLayer, float targetBeat);
    //[/UserMethods]

    void resized() override;
    void buttonClicked(Button *buttonThatWasClicked) override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    void onKeyChanged(int key) override;
    void onScaleChanged(const Scale::Ptr scale) override;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    void timerCallback() override;

    ProjectNode &project;
    Transport &transport;
    KeySignatureEvent originalEvent;
    KeySignaturesSequence *const originalSequence;

    Array<Scale::Ptr> scales;

    inline void cancelAndDisappear();
    inline void updateOkButtonState();

    bool addsNewEvent = false;
    bool hasMadeChanges = false;
    void sendEventChange(const KeySignatureEvent &newEvent);
    void removeEvent();

    int key = 0;
    Scale::Ptr scale;

    //[/UserVariables]

    UniquePointer<MobileComboBox::Primer> comboPrimer;
    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> removeEventButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<KeySelector> keySelector;
    UniquePointer<ScaleEditor> scaleEditor;
    UniquePointer<PlayButton> playButton;
    UniquePointer<TextEditor> scaleNameEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureDialog)
};



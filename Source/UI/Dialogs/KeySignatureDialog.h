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
#include "KeySignatureEvent.h"
#include "ScaleEditor.h"
#include "KeySelector.h"
#include "MobileComboBox.h"

class Transport;
class KeySignaturesSequence;
//[/Headers]

#include "../Themes/DialogPanel.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"
#include "../Common/PlayButton.h"

class KeySignatureDialog final : public FadingDialog,
                                 public TextEditor::Listener,
                                 public ScaleEditor::Listener,
                                 public KeySelector::Listener,
                                 private Timer,
                                 public Button::Listener
{
public:

    KeySignatureDialog(Component &owner, Transport &transport, KeySignaturesSequence *keySequence, const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);
    ~KeySignatureDialog();

    //[UserMethods]
    static KeySignatureDialog *createEditingDialog(Component &owner,
        Transport &transport, const KeySignatureEvent &event);
    static KeySignatureDialog *createAddingDialog(Component &owner,
        Transport &transport,  KeySignaturesSequence *annotationsLayer,
        float targetBeat);
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

    void onKeyChanged(int key) override;
    void onScaleChanged(const Scale::Ptr scale) override;

    void textEditorTextChanged(TextEditor&) override;
    void textEditorReturnKeyPressed(TextEditor&) override;
    void textEditorEscapeKeyPressed(TextEditor&) override;
    void textEditorFocusLost(TextEditor&) override;

    void timerCallback() override;

    Transport &transport;
    KeySignatureEvent originalEvent;
    KeySignaturesSequence *originalSequence;
    Component &ownerComponent;

    const Array<Scale::Ptr> defaultScales;

    inline void cancelAndDisappear();
    inline void updateOkButtonState();

    bool addsNewEvent;
    bool hasMadeChanges;
    void sendEventChange(const KeySignatureEvent &newEvent);
    void removeEvent();
    bool cancelChangesIfAny();

    int key;
    Scale::Ptr scale;

    //[/UserVariables]

    UniquePointer<DialogPanel> background;
    UniquePointer<MobileComboBox::Primer> comboPrimer;
    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> removeEventButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<SeparatorHorizontal> separatorH;
    UniquePointer<SeparatorVertical> separatorV;
    UniquePointer<KeySelector> keySelector;
    UniquePointer<ScaleEditor> scaleEditor;
    UniquePointer<PlayButton> playButton;
    UniquePointer<TextEditor> scaleNameEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureDialog)
};

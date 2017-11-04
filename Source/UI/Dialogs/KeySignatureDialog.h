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

class KeySignaturesSequence;
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class KeySignatureDialog  : public FadingDialog,
                            public TextEditorListener,
                            public Button::Listener,
                            public ComboBox::Listener
{
public:

    KeySignatureDialog (Component &owner, KeySignaturesSequence *signaturesLayer, const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);

    ~KeySignatureDialog();

    //[UserMethods]
    static KeySignatureDialog *createEditingDialog(Component &owner, const KeySignatureEvent &event);
    static KeySignatureDialog *createAddingDialog(Component &owner, KeySignaturesSequence *annotationsLayer, float targetBeat);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void visibilityChanged() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    KeySignatureEvent targetEvent;
    KeySignaturesSequence *targetLayer;
    Component &ownerComponent;

    inline void cancelAndDisappear();
    inline void disappear();
    inline void updateOkButtonState();

    bool addsNewEvent;
    bool hasMadeChanges;
    void sendEventChange(KeySignatureEvent newEvent);
    void removeEvent();
    void cancelChangesIfAny();

    //[/UserVariables]

    ScopedPointer<PanelC> background;
    ScopedPointer<Label> messageLabel;
    ScopedPointer<TextButton> removeEventButton;
    ScopedPointer<TextButton> okButton;
    ScopedPointer<ComboBox> textEditor;
    ScopedPointer<SeparatorHorizontal> separatorH;
    ScopedPointer<SeparatorVertical> separatorV;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureDialog)
};

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

class Transport;
class KeySignaturesSequence;
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"
#include "../Common/PlayButton.h"

class KeySignatureDialog  : public FadingDialog,
                            public TextEditorListener,
                            public ScaleEditor::Listener,
                            public KeySelector::Listener,
                            public Button::Listener,
                            public ComboBox::Listener
{
public:

    KeySignatureDialog (Component &owner, Transport &transport, KeySignaturesSequence *keySequence, const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);

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

    void onKeyChanged(int key) override;
    void onScaleChanged(Scale scale) override;

    Transport &transport;
    KeySignatureEvent originalEvent;
    KeySignaturesSequence *originalSequence;
    Component &ownerComponent;

    inline void cancelAndDisappear();
    inline void disappear();
    inline void updateOkButtonState();

    bool addsNewEvent;
    bool hasMadeChanges;
    void sendEventChange(KeySignatureEvent newEvent);
    void removeEvent();
    bool cancelChangesIfAny();

    int key;
    Scale scale;

    //[/UserVariables]

    ScopedPointer<PanelC> background;
    ScopedPointer<Label> messageLabel;
    ScopedPointer<TextButton> removeEventButton;
    ScopedPointer<TextButton> okButton;
    ScopedPointer<ComboBox> scaleNameEditor;
    ScopedPointer<SeparatorHorizontal> separatorH;
    ScopedPointer<SeparatorVertical> separatorV;
    ScopedPointer<KeySelector> keySelector;
    ScopedPointer<ScaleEditor> scaleEditor;
    ScopedPointer<PlayButton> playButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureDialog)
};

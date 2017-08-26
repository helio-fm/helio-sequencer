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

class TimeSignaturesSequence;
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorVertical.h"

class TimeSignatureDialog  : public FadingDialog,
                             public TextEditorListener,
                             public ButtonListener,
                             public ComboBoxListener
{
public:

    TimeSignatureDialog (Component &owner, TimeSignaturesSequence *signaturesLayer, const TimeSignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat);

    ~TimeSignatureDialog();

    //[UserMethods]
    static TimeSignatureDialog *createEditingDialog(Component &owner, const TimeSignatureEvent &event);
    static TimeSignatureDialog *createAddingDialog(Component &owner, TimeSignaturesSequence *annotationsLayer, float targetBeat);
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

    TimeSignatureEvent targetEvent;
    TimeSignaturesSequence *targetLayer;
    Component &ownerComponent;

    inline void cancelAndDisappear();
    inline void disappear();
    inline void updateOkButtonState();

    bool addsNewEvent;
    bool hasMadeChanges;
    void sendEventChange(TimeSignatureEvent newEvent);
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeSignatureDialog)
};

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
#include "AnnotationEvent.h"
//[/Headers]

#include "../Themes/PanelC.h"
#include "../Themes/PanelA.h"
#include "../Themes/ShadowDownwards.h"
#include "../Common/ColourSwatches.h"

class AnnotationDialog  : public FadingDialog,
                          public TextEditorListener,
                          public ColourButtonListener,
                          private Timer,
                          public ButtonListener,
                          public ComboBoxListener
{
public:

    AnnotationDialog (Component &owner, const AnnotationEvent &event, const String &message, const String &okText, const String &cancelText, int okCode, int cancelCode);

    ~AnnotationDialog();

    //[UserMethods]
	void onColourButtonClicked(ColourButton *button) override;
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

	AnnotationEvent targetEvent;

	Component &ownerComponent;

    int okCommand;
    int cancelCommand;

    void cancel()
    {
        this->ownerComponent.postCommandMessage(this->cancelCommand);
        this->disappear();
    }

    void okay()
    {
        if (textEditor->getText().isEmpty()) { return; }

        this->ownerComponent.postCommandMessage(this->okCommand);
        this->disappear();
    }

    void disappear()
    { delete this; }

    void updateOkButtonState();
    void timerCallback() override;


	bool hasMadeChanges;
	void sendEventChange(AnnotationEvent newEvent);
	void cancelChangesIfAny();

    //[/UserVariables]

    ScopedPointer<PanelC> background;
    ScopedPointer<PanelA> panel;
    ScopedPointer<Label> messageLabel;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<ShadowDownwards> shadow;
    ScopedPointer<TextButton> okButton;
    ScopedPointer<ComboBox> textEditor;
    ScopedPointer<ColourSwatches> colourSwatches;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnnotationDialog)
};

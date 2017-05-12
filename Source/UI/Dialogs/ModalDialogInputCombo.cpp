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

//[Headers]
#include "Common.h"
//[/Headers]

#include "ModalDialogInputCombo.h"

//[MiscUserDefs]
static const int asyncCancelCommandId = 123123;

#if HELIO_DESKTOP
#    define INPUT_DIALOG_FONT_SIZE (21)
#elif HELIO_MOBILE
#    define INPUT_DIALOG_FONT_SIZE (28)
#endif
//[/MiscUserDefs]

ModalDialogInputCombo::ModalDialogInputCombo(Component &owner, String &result, const String &message, const String &okText, const String &cancelText, int okCode, int cancelCode)
    : ownerComponent(owner),
      targetString(result),
      okCommand(okCode),
      cancelCommand(cancelCode)
{
    addAndMakeVisible (background = new PanelC());
    addAndMakeVisible (panel = new PanelA());
    addAndMakeVisible (messageLabel = new Label (String(),
                                                 TRANS("...")));
    messageLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    messageLabel->setJustificationType (Justification::centred);
    messageLabel->setEditable (false, false, false);
    messageLabel->setColour (Label::textColourId, Colours::white);
    messageLabel->setColour (TextEditor::textColourId, Colours::black);
    messageLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (cancelButton = new TextButton (String()));
    cancelButton->setButtonText (TRANS("..."));
    cancelButton->setConnectedEdges (Button::ConnectedOnTop);
    cancelButton->addListener (this);

    addAndMakeVisible (shadow = new ShadowDownwards());
    addAndMakeVisible (okButton = new TextButton (String()));
    okButton->setButtonText (TRANS("..."));
    okButton->setConnectedEdges (Button::ConnectedOnTop);
    okButton->addListener (this);

    addAndMakeVisible (textEditor = new ComboBox (String()));
    textEditor->setEditableText (true);
    textEditor->setJustificationType (Justification::centredLeft);
    textEditor->setTextWhenNothingSelected (String());
    textEditor->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    textEditor->addListener (this);


    //[UserPreSize]
    this->messageLabel->setText(message, dontSendNotification);
    this->okButton->setButtonText(okText);
    this->cancelButton->setButtonText(cancelText);

    //this->textEditor->setFont(Font(INPUT_DIALOG_FONT_SIZE));
    this->textEditor->setText(this->targetString, dontSendNotification);
    this->textEditor->addListener(this);
    //[/UserPreSize]

    setSize (450, 175);

    //[Constructor]
    this->rebound();
    this->setWantsKeyboardFocus(true);
    this->setInterceptsMouseClicks(true, true);
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->textEditor->grabKeyboardFocus();
    //this->textEditor->setTextToShowWhenEmpty(message, Colours::black.withAlpha(0.5f));
    this->updateOkButtonState();

    this->startTimer(100);
    //[/Constructor]
}

ModalDialogInputCombo::~ModalDialogInputCombo()
{
    //[Destructor_pre]
    this->stopTimer();

    textEditor->removeListener(this);

    FadingDialog::fadeOut();
    //[/Destructor_pre]

    background = nullptr;
    panel = nullptr;
    messageLabel = nullptr;
    cancelButton = nullptr;
    shadow = nullptr;
    okButton = nullptr;
    textEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ModalDialogInputCombo::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x59000000));
    g.fillRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 10.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ModalDialogInputCombo::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds ((getWidth() / 2) - ((getWidth() - 10) / 2), 5, getWidth() - 10, getHeight() - 10);
    panel->setBounds ((getWidth() / 2) - ((getWidth() - 30) / 2), 15, getWidth() - 30, 100);
    messageLabel->setBounds ((getWidth() / 2) - ((getWidth() - 60) / 2), 5 + 16, getWidth() - 60, 36);
    cancelButton->setBounds ((getWidth() / 2) + -5 - 150, 15 + 100, 150, 42);
    shadow->setBounds ((getWidth() / 2) - (310 / 2), 15 + 100 - 3, 310, 24);
    okButton->setBounds ((getWidth() / 2) + 5, 15 + 100, 150, 42);
    textEditor->setBounds ((getWidth() / 2) - ((getWidth() - 60) / 2), 62, getWidth() - 60, 36);
    //[UserResized] Add your own custom resize handling here..
    this->textEditor->grabKeyboardFocus();
    //[/UserResized]
}

void ModalDialogInputCombo::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == cancelButton)
    {
        //[UserButtonCode_cancelButton] -- add your button handler code here..
        this->cancel();
        //[/UserButtonCode_cancelButton]
    }
    else if (buttonThatWasClicked == okButton)
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        this->okay();
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void ModalDialogInputCombo::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == textEditor)
    {
        //[UserComboBoxCode_textEditor] -- add your combo box handling code here..
        //[/UserComboBoxCode_textEditor]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void ModalDialogInputCombo::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    this->textEditor->grabKeyboardFocus();
    //[/UserCode_visibilityChanged]
}

void ModalDialogInputCombo::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentHierarchyChanged]
}

void ModalDialogInputCombo::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentSizeChanged]
}

void ModalDialogInputCombo::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == asyncCancelCommandId)
    {
        this->cancel();
    }
    //[/UserCode_handleCommandMessage]
}

bool ModalDialogInputCombo::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancel();
        return true;
    }
    else if (key.isKeyCode(KeyPress::returnKey) ||
             key.isKeyCode(KeyPress::tabKey))
    {
        this->okay();
        return true;
    }

    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}

void ModalDialogInputCombo::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(asyncCancelCommandId);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void ModalDialogInputCombo::updateOkButtonState()
{
    const bool textIsEmpty = this->targetString.isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void ModalDialogInputCombo::timerCallback()
{
    if (! this->textEditor->hasKeyboardFocus(true))
    {
        this->textEditor->grabKeyboardFocus();
        this->stopTimer();
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ModalDialogInputCombo" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public TextEditorListener, private Timer"
                 constructorParams="Component &amp;owner, String &amp;result, const String &amp;message, const String &amp;okText, const String &amp;cancelText, int okCode, int cancelCode"
                 variableInitialisers="ownerComponent(owner),&#10;targetString(result),&#10;okCommand(okCode),&#10;cancelCommand(cancelCode)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="450" initialHeight="175">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10" fill="solid: 59000000" hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 5 10M 10M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/PanelC.cpp" constructorParams=""/>
  <JUCERCOMP name="" id="fee11f38ba63ec9" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="0Cc 15 30M 100" sourceFile="../Themes/PanelA.cpp"
             constructorParams=""/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 16 60M 36" posRelativeY="e96b77baef792d3a"
         textCol="ffffffff" edTextCol="ff000000" edBkgCol="0" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21" kerning="0" bold="0"
         italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="-5Cr 0R 150 42" posRelativeY="fee11f38ba63ec9"
              buttonText="..." connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="ab3649d51aa02a67" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0Cc 3R 310 24" posRelativeY="fee11f38ba63ec9"
             sourceFile="../Themes/ShadowDownwards.cpp" constructorParams=""/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="5C 0R 150 42" posRelativeY="fee11f38ba63ec9"
              buttonText="..." connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="" id="1923d71c308d2169" memberName="textEditor" virtualName=""
            explicitFocusOrder="0" pos="0Cc 62 60M 36" editable="1" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

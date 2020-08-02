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

#include "ModalDialogConfirmation.h"

//[MiscUserDefs]
#include "CommandIDs.h"
//[/MiscUserDefs]

ModalDialogConfirmation::ModalDialogConfirmation(const String &message, const String &okText, const String &cancelText)
{
    this->messageLabel.reset(new Label(String(),
                                              String()));
    this->addAndMakeVisible(messageLabel.get());
    this->messageLabel->setFont(Font (21.00f, Font::plain));
    messageLabel->setJustificationType(Justification::centred);
    messageLabel->setEditable(false, false, false);

    this->cancelButton.reset(new TextButton(String()));
    this->addAndMakeVisible(cancelButton.get());
    cancelButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    cancelButton->addListener(this);

    this->okButton.reset(new TextButton(String()));
    this->addAndMakeVisible(okButton.get());
    okButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    okButton->addListener(this);


    //[UserPreSize]
    this->messageLabel->setText(message, dontSendNotification);
    this->okButton->setButtonText(okText);
    this->cancelButton->setButtonText(cancelText);
    this->messageLabel->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(410, 180);

    //[Constructor]
    this->updatePosition();
    this->setWantsKeyboardFocus(true);
    this->setInterceptsMouseClicks(true, true);
    this->setAlwaysOnTop(true);
    this->toFront(true);
    //[/Constructor]
}

ModalDialogConfirmation::~ModalDialogConfirmation()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    messageLabel = nullptr;
    cancelButton = nullptr;
    okButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ModalDialogConfirmation::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ModalDialogConfirmation::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    messageLabel->setBounds((getWidth() / 2) - ((getWidth() - 32) / 2), 12, getWidth() - 32, 96);
    cancelButton->setBounds(4, getHeight() - 4 - 48, 200, 48);
    okButton->setBounds(getWidth() - 4 - 201, getHeight() - 4 - 48, 201, 48);
    //[UserResized] Add your own custom resize handling here..
    if (this->isShowing())
    {
        this->grabKeyboardFocus();
    }
    //[/UserResized]
}

void ModalDialogConfirmation::buttonClicked(Button *buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == cancelButton.get())
    {
        //[UserButtonCode_cancelButton] -- add your button handler code here..
        this->cancel();
        //[/UserButtonCode_cancelButton]
    }
    else if (buttonThatWasClicked == okButton.get())
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        this->okay();
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void ModalDialogConfirmation::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void ModalDialogConfirmation::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}

void ModalDialogConfirmation::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancel();
    }
    //[/UserCode_handleCommandMessage]
}

bool ModalDialogConfirmation::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancel();
        return true;
    }
    else if (key.isKeyCode(KeyPress::returnKey))
    {
        this->okay();
        return true;
    }

    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}

void ModalDialogConfirmation::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]
void ModalDialogConfirmation::cancel()
{
    const BailOutChecker checker(this);

    if (this->onCancel != nullptr)
    {
        this->onCancel();
    }

    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

void ModalDialogConfirmation::okay()
{
    const BailOutChecker checker(this);

    if (this->onOk != nullptr)
    {
        this->onOk();
    }

    // a user might have created another dialog in onOk
    // and showed it as a modal component, which destroyed this,
    // because there can only be one model component:
    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::deleteProject()
{
    return UniquePointer<ModalDialogConfirmation>(
        new ModalDialogConfirmation(
            TRANS(I18n::Dialog::deleteProjectCaption),
            TRANS(I18n::Dialog::deleteProjectProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::forceCheckout()
{
    return UniquePointer<ModalDialogConfirmation>(
        new ModalDialogConfirmation(
            TRANS(I18n::Dialog::vcsCheckoutWarning),
            TRANS(I18n::Dialog::vcsCheckoutProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::resetChanges()
{
    return UniquePointer<ModalDialogConfirmation>(
        new ModalDialogConfirmation(
            TRANS(I18n::Dialog::vcsResetCaption),
            TRANS(I18n::Dialog::vcsResetProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::confirmOpenGL()
{
    return UniquePointer<ModalDialogConfirmation>(
        new ModalDialogConfirmation(
            TRANS(I18n::Dialog::openglCaption),
            TRANS(I18n::Dialog::openglProceed),
            TRANS(I18n::Dialog::cancel)));
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ModalDialogConfirmation"
                 template="../../Template" componentName="" parentClasses="public DialogBase"
                 constructorParams="const String &amp;message, const String &amp;okText, const String &amp;cancelText"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="410" initialHeight="180">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 12 32M 96" posRelativeY="e96b77baef792d3a"
         labelText="" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="4 4Rr 200 48" buttonText="" connectedEdges="6"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 201 48" buttonText="" connectedEdges="5"
              needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif




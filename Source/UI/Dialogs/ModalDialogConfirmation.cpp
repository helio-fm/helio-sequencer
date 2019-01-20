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
    addAndMakeVisible (background = new DialogPanel());
    addAndMakeVisible (messageLabel = new Label (String(),
                                                 TRANS("...")));
    messageLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    messageLabel->setJustificationType (Justification::centred);
    messageLabel->setEditable (false, false, false);

    addAndMakeVisible (cancelButton = new TextButton (String()));
    cancelButton->setButtonText (TRANS("..."));
    cancelButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    cancelButton->addListener (this);

    addAndMakeVisible (okButton = new TextButton (String()));
    okButton->setButtonText (TRANS("..."));
    okButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    okButton->addListener (this);

    addAndMakeVisible (separatorH = new SeparatorHorizontal());
    addAndMakeVisible (separatorV = new SeparatorVertical());

    //[UserPreSize]
    this->messageLabel->setText(message, dontSendNotification);
    this->okButton->setButtonText(okText);
    this->cancelButton->setButtonText(cancelText);
    this->separatorH->setAlphaMultiplier(2.5f);
    //[/UserPreSize]

    setSize (410, 180);

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

    background = nullptr;
    messageLabel = nullptr;
    cancelButton = nullptr;
    okButton = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ModalDialogConfirmation::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour = Colour (0x59000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ModalDialogConfirmation::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds ((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    messageLabel->setBounds ((getWidth() / 2) - ((getWidth() - 32) / 2), 4 + 12, getWidth() - 32, 96);
    cancelButton->setBounds (4, getHeight() - 4 - 48, 200, 48);
    okButton->setBounds (getWidth() - 4 - 201, getHeight() - 4 - 48, 201, 48);
    separatorH->setBounds (4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds ((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    //[UserResized] Add your own custom resize handling here..
    if (this->isShowing())
    {
        this->grabKeyboardFocus();
    }
    //[/UserResized]
}

void ModalDialogConfirmation::buttonClicked (Button* buttonThatWasClicked)
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

ScopedPointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::deleteProject()
{
    return { new ModalDialogConfirmation(
        TRANS("dialog::deleteproject::caption"),
        TRANS("dialog::deleteproject::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::forceCheckout()
{
    return { new ModalDialogConfirmation(
        TRANS("dialog::vcs::checkout::warning"),
        TRANS("dialog::vcs::checkout::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::resetChanges()
{
    return { new ModalDialogConfirmation(
        TRANS("dialog::vcs::reset::caption"),
        TRANS("dialog::vcs::reset::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::confirmOpenGL()
{
    return { new ModalDialogConfirmation(
        TRANS("dialog::opengl::caption"),
        TRANS("dialog::opengl::proceed"),
        TRANS("dialog::common::cancel")) };
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ModalDialogConfirmation"
                 template="../../Template" componentName="" parentClasses="public FadingDialog"
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
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10.00000000000000000000" fill="solid: 59000000"
               hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/DialogPanel.cpp" constructorParams=""/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 12 32M 96" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="4 4Rr 200 48" buttonText="..." connectedEdges="6"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 201 48" buttonText="..."
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1fb927654787aaf4" memberName="separatorV" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4Rr 2 48" sourceFile="../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

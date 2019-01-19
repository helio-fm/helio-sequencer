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

#include "ModalDialogInput.h"

//[MiscUserDefs]
#include "CommandIDs.h"

#if HELIO_DESKTOP
#    define INPUT_DIALOG_FONT_SIZE (21)
#elif HELIO_MOBILE
#    define INPUT_DIALOG_FONT_SIZE (28)
#endif
//[/MiscUserDefs]

ModalDialogInput::ModalDialogInput(const String &text, const String &message, const String &okText, const String &cancelText)
    : input(text)
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

    addAndMakeVisible (textEditor = new TextEditor (String()));
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (false);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (String());

    addAndMakeVisible (separatorH = new SeparatorHorizontal());
    addAndMakeVisible (separatorV = new SeparatorVertical());

    //[UserPreSize]
    this->messageLabel->setText(message, dontSendNotification);
    this->okButton->setButtonText(okText);
    this->cancelButton->setButtonText(cancelText);

    this->textEditor->setFont(Font(INPUT_DIALOG_FONT_SIZE));
    this->textEditor->setText(this->input, dontSendNotification);
    this->textEditor->addListener(this);

    this->separatorH->setAlphaMultiplier(2.5f);
    //[/UserPreSize]

    setSize (450, 165);

    //[Constructor]
    this->updatePosition();
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->textEditor->setTextToShowWhenEmpty(message, Colours::black.withAlpha(0.5f));
    this->setAlwaysOnTop(true);
    this->toFront(true);
    this->updateOkButtonState();

    this->startTimer(100);
    //[/Constructor]
}

ModalDialogInput::~ModalDialogInput()
{
    //[Destructor_pre]
    this->stopTimer();
    textEditor->removeListener(this);
    //[/Destructor_pre]

    background = nullptr;
    messageLabel = nullptr;
    cancelButton = nullptr;
    okButton = nullptr;
    textEditor = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ModalDialogInput::paint (Graphics& g)
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

void ModalDialogInput::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds ((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    messageLabel->setBounds ((getWidth() / 2) - ((getWidth() - 60) / 2), 4 + 12, getWidth() - 60, 36);
    cancelButton->setBounds (4, getHeight() - 4 - 48, 220, 48);
    okButton->setBounds (getWidth() - 4 - 221, getHeight() - 4 - 48, 221, 48);
    textEditor->setBounds ((getWidth() / 2) - ((getWidth() - 60) / 2), 58, getWidth() - 60, 36);
    separatorH->setBounds (4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds ((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    //[UserResized] Add your own custom resize handling here..
    if (this->isShowing())
    {
        this->textEditor->grabKeyboardFocus();
    }
    //[/UserResized]
}

void ModalDialogInput::buttonClicked (Button* buttonThatWasClicked)
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

void ModalDialogInput::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    if (this->isShowing())
    {
        this->textEditor->grabKeyboardFocus();
    }
    //[/UserCode_visibilityChanged]
}

void ModalDialogInput::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void ModalDialogInput::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}

void ModalDialogInput::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancel();
    }
    //[/UserCode_handleCommandMessage]
}

void ModalDialogInput::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void ModalDialogInput::textEditorTextChanged(TextEditor &editor)
{
    this->input = editor.getText();
    this->updateOkButtonState();
}

void ModalDialogInput::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->textEditorFocusLost(ed);
}

void ModalDialogInput::textEditorEscapeKeyPressed(TextEditor &)
{
    this->cancel();
}

void ModalDialogInput::textEditorFocusLost(TextEditor &ed)
{
    this->updateOkButtonState();

    const Component *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->input.isNotEmpty() &&
        focusedComponent != this->okButton &&
        focusedComponent != this->cancelButton)
    {
        this->okay();
    }
    else
    {
        ed.grabKeyboardFocus();
    }
}

void ModalDialogInput::cancel()
{
    const BailOutChecker checker(this);

    if (this->onCancel != nullptr)
    {
        this->onCancel(this->input);
    }

    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

void ModalDialogInput::okay()
{
    if (textEditor->getText().isEmpty())
    {
        return;
    }

    const BailOutChecker checker(this);

    if (this->onOk != nullptr)
    {
        this->onOk(this->input);
    }

    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

void ModalDialogInput::updateOkButtonState()
{
    const bool textIsEmpty = this->input.isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void ModalDialogInput::timerCallback()
{
    if (! this->textEditor->hasKeyboardFocus(true))
    {
        this->textEditor->grabKeyboardFocus();
        this->textEditor->selectAll();
        this->stopTimer();
    }
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::renameAnnotation(const String &name)
{
    return { new ModalDialogInput(name,
        TRANS("dialog::annotation::rename::caption"),
        TRANS("dialog::annotation::rename::proceed"),
        TRANS("dialog::annotation::rename::cancel")) };
}

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::renameInstrument(const String &name)
{
    return { new ModalDialogInput(name,
        TRANS("dialog::instrument::rename::caption"),
        TRANS("dialog::instrument::rename::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::changeTimeSignature(const String &name)
{
    return { new ModalDialogInput(name,
        TRANS("dialog::timesignature::edit::caption"),
        TRANS("dialog::timesignature::edit::apply"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::renameTrack(const String &name)
{
    return { new ModalDialogInput(name,
        TRANS("dialog::renametrack::caption"),
        TRANS("dialog::renametrack::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::newTrack()
{
    return { new ModalDialogInput(
        TRANS("defaults::newtrack::name"),
        TRANS("dialog::addtrack::caption"),
        TRANS("dialog::addtrack::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::deleteProjectConfirmation()
{
    return { new ModalDialogInput({},
        TRANS("dialog::deleteproject::confirm::caption"),
        TRANS("dialog::deleteproject::confirm::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::commit(const String &name)
{
    return { new ModalDialogInput(name,
        TRANS("dialog::vcs::commit::caption"),
        TRANS("dialog::vcs::commit::proceed"),
        TRANS("dialog::common::cancel")) };
}

ScopedPointer<ModalDialogInput> ModalDialogInput::Presets::newArpeggiator()
{
    return { new ModalDialogInput({},
        TRANS("dialog::addarp::caption"),
        TRANS("dialog::addarp::proceed"),
        TRANS("dialog::common::cancel")) };
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ModalDialogInput" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public TextEditor::Listener, private Timer"
                 constructorParams="const String &amp;text, const String &amp;message, const String &amp;okText, const String &amp;cancelText"
                 variableInitialisers="input(text)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="450"
                 initialHeight="165">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="visibilityChanged()"/>
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
         explicitFocusOrder="0" pos="0Cc 12 60M 36" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="4 4Rr 220 48" buttonText="..." connectedEdges="6"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 221 48" buttonText="..."
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <TEXTEDITOR name="" id="4d16d51ea0c579db" memberName="textEditor" virtualName=""
              explicitFocusOrder="0" pos="0Cc 58 60M 36" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="1"/>
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

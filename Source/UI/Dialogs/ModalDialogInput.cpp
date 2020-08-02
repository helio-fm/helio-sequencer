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

    this->textEditor.reset(new TextEditor(String()));
    this->addAndMakeVisible(textEditor.get());
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (false);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (String());


    //[UserPreSize]
    this->messageLabel->setText(message, dontSendNotification);
    this->okButton->setButtonText(okText);
    this->cancelButton->setButtonText(cancelText);

    this->textEditor->setFont(Font(INPUT_DIALOG_FONT_SIZE));
    this->textEditor->setText(this->input, dontSendNotification);
    this->textEditor->addListener(this);

    this->messageLabel->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(450, 165);

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

    messageLabel = nullptr;
    cancelButton = nullptr;
    okButton = nullptr;
    textEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ModalDialogInput::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ModalDialogInput::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    messageLabel->setBounds((getWidth() / 2) - ((getWidth() - 60) / 2), 12, getWidth() - 60, 36);
    cancelButton->setBounds(4, getHeight() - 4 - 48, 220, 48);
    okButton->setBounds(getWidth() - 4 - 221, getHeight() - 4 - 48, 221, 48);
    textEditor->setBounds((getWidth() / 2) - ((getWidth() - 60) / 2), 58, getWidth() - 60, 36);
    //[UserResized] Add your own custom resize handling here..
    if (this->isShowing())
    {
        this->textEditor->grabKeyboardFocus();
    }
    //[/UserResized]
}

void ModalDialogInput::buttonClicked(Button *buttonThatWasClicked)
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
        focusedComponent != this->okButton.get() &&
        focusedComponent != this->cancelButton.get())
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

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::renameAnnotation(const String &name)
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput(name,
            TRANS(I18n::Dialog::annotationRenameCaption),
            TRANS(I18n::Dialog::annotationRenameProceed),
            TRANS(I18n::Dialog::annotationRenameCancel)));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::renameInstrument(const String &name)
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput(name,
            TRANS(I18n::Dialog::instrumentRenameCaption),
            TRANS(I18n::Dialog::instrumentRenameProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::changeTimeSignature(const String &name)
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput(name,
            TRANS(I18n::Dialog::timeSignatureEditCaption),
            TRANS(I18n::Dialog::timeSignatureEditApply),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::renameTrack(const String &name)
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput(name,
            TRANS(I18n::Dialog::renameTrackCaption),
            TRANS(I18n::Dialog::renameTrackProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::newTrack()
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput(
            TRANS(I18n::Defaults::midiTrackName),
            TRANS(I18n::Dialog::addTrackCaption),
            TRANS(I18n::Dialog::addTrackProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::deleteProjectConfirmation()
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput({},
            TRANS(I18n::Dialog::deleteProjectConfirmCaption),
            TRANS(I18n::Dialog::deleteProjectConfirmProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::commit(const String &name)
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput(name,
            TRANS(I18n::Dialog::vcsCommitCaption),
            TRANS(I18n::Dialog::vcsCommitProceed),
            TRANS(I18n::Dialog::cancel)));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::newArpeggiator()
{
    return UniquePointer<ModalDialogInput>(
        new ModalDialogInput({},
            TRANS(I18n::Dialog::addArpCaption),
            TRANS(I18n::Dialog::addArpProceed),
            TRANS(I18n::Dialog::cancel)));
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ModalDialogInput" template="../../Template"
                 componentName="" parentClasses="public DialogBase, public TextEditor::Listener, private Timer"
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
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 12 60M 36" posRelativeY="e96b77baef792d3a"
         labelText="" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="4 4Rr 220 48" buttonText="" connectedEdges="6"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 221 48" buttonText="" connectedEdges="5"
              needsCallback="1" radioGroupId="0"/>
  <TEXTEDITOR name="" id="4d16d51ea0c579db" memberName="textEditor" virtualName=""
              explicitFocusOrder="0" pos="0Cc 58 60M 36" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif




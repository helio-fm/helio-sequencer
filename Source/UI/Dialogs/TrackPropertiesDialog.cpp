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

#include "TrackPropertiesDialog.h"

//[MiscUserDefs]
#include "CommandIDs.h"
#include "UndoStack.h"
#include "ProjectNode.h"
#include "MidiTrackActions.h"
//[/MiscUserDefs]

TrackPropertiesDialog::TrackPropertiesDialog(ProjectNode &project, WeakReference<MidiTrack> track)
    : project(project),
      track(track)
{
    this->background.reset(new DialogPanel());
    this->addAndMakeVisible(background.get());
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

    this->separatorH.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separatorH.get());
    this->separatorV.reset(new SeparatorVertical());
    this->addAndMakeVisible(separatorV.get());
    this->colourSwatches.reset(new ColourSwatches());
    this->addAndMakeVisible(colourSwatches.get());

    this->textEditor.reset(new TextEditor(String()));
    this->addAndMakeVisible(textEditor.get());
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (String());


    //[UserPreSize]
    this->textEditor->setFont(21.f);
    this->textEditor->addListener(this);

    this->originalName = this->track->getTrackName();
    this->originalColour = this->track->getTrackColour();
    this->newName = this->originalName;
    this->newColour = this->originalColour;

    this->colourSwatches->setSelectedColour(this->originalColour);
    this->textEditor->setText(this->originalName, dontSendNotification);

    this->messageLabel->setText(TRANS(I18n::Dialog::renameTrackCaption), dontSendNotification);
    this->okButton->setButtonText(TRANS(I18n::Dialog::renameTrackProceed));
    this->cancelButton->setButtonText(TRANS(I18n::Dialog::cancel));

    this->separatorH->setAlphaMultiplier(2.5f);
    //[/UserPreSize]

    this->setSize(450, 220);

    //[Constructor]
    this->updatePosition();
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->updateOkButtonState();
    this->startTimer(100);
    //[/Constructor]
}

TrackPropertiesDialog::~TrackPropertiesDialog()
{
    //[Destructor_pre]
    this->textEditor->removeListener(this);
    //[/Destructor_pre]

    background = nullptr;
    messageLabel = nullptr;
    cancelButton = nullptr;
    okButton = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;
    colourSwatches = nullptr;
    textEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TrackPropertiesDialog::paint (Graphics& g)
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

void TrackPropertiesDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    messageLabel->setBounds((getWidth() / 2) - ((getWidth() - 48) / 2), 4 + 16, getWidth() - 48, 36);
    cancelButton->setBounds(4, getHeight() - 4 - 48, 220, 48);
    okButton->setBounds(getWidth() - 4 - 221, getHeight() - 4 - 48, 221, 48);
    separatorH->setBounds(4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    colourSwatches->setBounds((getWidth() / 2) + 2 - ((getWidth() - 56) / 2), 106, getWidth() - 56, 34);
    textEditor->setBounds((getWidth() / 2) - ((getWidth() - 48) / 2), 66, getWidth() - 48, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TrackPropertiesDialog::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == cancelButton.get())
    {
        //[UserButtonCode_cancelButton] -- add your button handler code here..
        this->doCancel();
        return;
        //[/UserButtonCode_cancelButton]
    }
    else if (buttonThatWasClicked == okButton.get())
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        this->doOk();
        return;
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void TrackPropertiesDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void TrackPropertiesDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}

void TrackPropertiesDialog::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelChangesIfAny();
        this->dismiss();
    }
    //[/UserCode_handleCommandMessage]
}

void TrackPropertiesDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void TrackPropertiesDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void TrackPropertiesDialog::onColourButtonClicked(ColourButton *clickedButton)
{
    this->newColour = clickedButton->getColour();
    this->applyChangesIfAny();
}

bool TrackPropertiesDialog::hasChanges() const
{
    return this->newColour != this->originalColour ||
        (this->newName != this->originalName && this->newName.isNotEmpty());
}

void TrackPropertiesDialog::cancelChangesIfAny()
{
    if (this->hasChanges())
    {
        this->project.getUndoStack()->undoCurrentTransactionOnly();
    }
}

void TrackPropertiesDialog::applyChangesIfAny()
{
    const auto &trackId = this->track->getTrackId();

    if (this->hasMadeChanges)
    {
        this->project.getUndoStack()->undoCurrentTransactionOnly();
    }

    this->project.getUndoStack()->beginNewTransaction();

    if (this->newName != this->originalName)
    {
        this->project.getUndoStack()->perform(new MidiTrackRenameAction(this->project, trackId, this->newName));
    }

    if (this->newColour != this->originalColour)
    {
        this->project.getUndoStack()->perform(new MidiTrackChangeColourAction(this->project, trackId, this->newColour));
    }

    this->hasMadeChanges = true;
}

void TrackPropertiesDialog::textEditorTextChanged(TextEditor&)
{
    this->updateOkButtonState();
    this->newName = this->textEditor->getText();
    this->applyChangesIfAny();
}

void TrackPropertiesDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->textEditorFocusLost(ed);
}

void TrackPropertiesDialog::textEditorEscapeKeyPressed(TextEditor&)
{
    this->cancelChangesIfAny();
    this->dismiss();
}

void TrackPropertiesDialog::textEditorFocusLost(TextEditor&)
{
    this->updateOkButtonState();

    const auto *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->textEditor->getText().isNotEmpty() &&
        focusedComponent != this->okButton.get() &&
        focusedComponent != this->cancelButton.get())
    {
        this->dismiss();
    }
    else
    {
        this->textEditor->grabKeyboardFocus();
    }
}

void TrackPropertiesDialog::timerCallback()
{
    if (!this->textEditor->hasKeyboardFocus(false))
    {
        this->textEditor->grabKeyboardFocus();
        this->textEditor->selectAll();
        this->stopTimer();
    }
}

void TrackPropertiesDialog::doCancel()
{
    this->cancelChangesIfAny();

    if (this->onCancel != nullptr)
    {
        BailOutChecker checker(this);

        this->onCancel();

        if (checker.shouldBailOut())
        {
            jassertfalse; // not expected
            return;
        }
    }

    this->dismiss();
}

void TrackPropertiesDialog::doOk()
{
    if (textEditor->getText().isNotEmpty())
    {
        if (this->onOk != nullptr)
        {
            BailOutChecker checker(this);

            this->onOk();

            if (checker.shouldBailOut())
            {
                jassertfalse; // not expected
                return;
            }
        }

        this->dismiss();
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TrackPropertiesDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public TextEditor::Listener, public ColourButtonListener, private Timer"
                 constructorParams="ProjectNode &amp;project, WeakReference&lt;MidiTrack&gt; track"
                 variableInitialisers="project(project),&#10;track(track)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="450" initialHeight="220">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10.0" fill="solid: 59000000" hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/DialogPanel.cpp" constructorParams=""/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 16 48M 36" posRelativeY="e96b77baef792d3a"
         labelText="" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="4 4Rr 220 48" buttonText="" connectedEdges="6"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 221 48" buttonText="" connectedEdges="5"
              needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1fb927654787aaf4" memberName="separatorV" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4Rr 2 48" sourceFile="../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="123ea615ffefd36f" memberName="colourSwatches" virtualName=""
                    explicitFocusOrder="0" pos="2Cc 106 56M 34" class="ColourSwatches"
                    params=""/>
  <TEXTEDITOR name="" id="3f330f1d57714294" memberName="textEditor" virtualName=""
              explicitFocusOrder="0" pos="0Cc 66 48M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif




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

#include "TimeSignatureDialog.h"

//[MiscUserDefs]
#include "CommandIDs.h"
#include "TimeSignaturesSequence.h"

static StringPairArray getDefaultMeters()
{
    StringPairArray c;
    c.set("Common time", "4/4");
    c.set("Alla breve", "2/4");
    c.set("Waltz time", "3/4");
    c.set("5/4", "5/4");
    c.set("6/4", "6/4");
    c.set("7/4", "7/4");
    c.set("5/8", "5/8");
    c.set("6/8", "6/8");
    c.set("7/8", "7/8");
    c.set("9/8", "9/8");
    c.set("12/8", "12/8");
    return c;
}
//[/MiscUserDefs]

TimeSignatureDialog::TimeSignatureDialog(Component &owner, TimeSignaturesSequence *timeSequence, const TimeSignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat)
    : originalEvent(editedEvent),
      originalSequence(timeSequence),
      ownerComponent(owner),
      defailtMeters(getDefaultMeters()),
      addsNewEvent(shouldAddNewEvent),
      hasMadeChanges(false)
{
    this->background.reset(new DialogPanel());
    this->addAndMakeVisible(background.get());
    this->comboPrimer.reset(new MobileComboBox::Primer());
    this->addAndMakeVisible(comboPrimer.get());

    this->messageLabel.reset(new Label(String(),
                                        TRANS("...")));
    this->addAndMakeVisible(messageLabel.get());
    this->messageLabel->setFont(Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    messageLabel->setJustificationType(Justification::centred);
    messageLabel->setEditable(false, false, false);

    this->removeEventButton.reset(new TextButton(String()));
    this->addAndMakeVisible(removeEventButton.get());
    removeEventButton->setButtonText(TRANS("..."));
    removeEventButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    removeEventButton->addListener(this);

    this->okButton.reset(new TextButton(String()));
    this->addAndMakeVisible(okButton.get());
    okButton->setButtonText(TRANS("..."));
    okButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    okButton->addListener(this);

    this->separatorH.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separatorH.get());
    this->separatorV.reset(new SeparatorVertical());
    this->addAndMakeVisible(separatorV.get());
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
    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);
    const auto &meterNames = this->defailtMeters.getAllKeys();
    const auto &meterValues = this->defailtMeters.getAllValues();

    if (this->addsNewEvent)
    {
        Random r;
        const String meter(meterValues[r.nextInt(this->defailtMeters.size())]);
        int numerator;
        int denominator;
        TimeSignatureEvent::parseString(meter, numerator, denominator);
        this->originalEvent = TimeSignatureEvent(this->originalSequence, targetBeat, numerator, denominator);

        this->originalSequence->checkpoint();
        this->originalSequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS("dialog::timesignature::add::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::timesignature::add::proceed"));
        this->removeEventButton->setButtonText(TRANS("dialog::common::cancel"));
    }
    else
    {
        this->messageLabel->setText(TRANS("dialog::timesignature::edit::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::timesignature::edit::apply"));
        this->removeEventButton->setButtonText(TRANS("dialog::timesignature::edit::delete"));
    }

    this->textEditor->addListener(this);
    this->textEditor->setFont(21.f);
    this->textEditor->setText(this->originalEvent.toString(), dontSendNotification);

    this->separatorH->setAlphaMultiplier(2.5f);
    //[/UserPreSize]

    this->setSize(370, 185);

    //[Constructor]
    this->updatePosition();
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->updateOkButtonState();

    MenuPanel::Menu menu;
    for (int i = 0; i < this->defailtMeters.size(); ++i)
    {
        const auto &s = meterNames[i];
        menu.add(MenuItem::item(Icons::empty, CommandIDs::SelectTimeSignature + i, s));
    }
    this->comboPrimer->initWith(this->textEditor.get(), menu);

    this->startTimer(100);
    //[/Constructor]
}

TimeSignatureDialog::~TimeSignatureDialog()
{
    //[Destructor_pre]
    textEditor->removeListener(this);
    //[/Destructor_pre]

    background = nullptr;
    comboPrimer = nullptr;
    messageLabel = nullptr;
    removeEventButton = nullptr;
    okButton = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;
    textEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TimeSignatureDialog::paint (Graphics& g)
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

void TimeSignatureDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    comboPrimer->setBounds((getWidth() / 2) - ((getWidth() - 24) / 2), 12, getWidth() - 24, getHeight() - 72);
    messageLabel->setBounds((getWidth() / 2) - ((getWidth() - 32) / 2), 4 + 20, getWidth() - 32, 36);
    removeEventButton->setBounds(4, getHeight() - 4 - 48, 180, 48);
    okButton->setBounds(getWidth() - 4 - 181, getHeight() - 4 - 48, 181, 48);
    separatorH->setBounds(4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    textEditor->setBounds((getWidth() / 2) - ((getWidth() - 48) / 2), 68, getWidth() - 48, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TimeSignatureDialog::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == removeEventButton.get())
    {
        //[UserButtonCode_removeEventButton] -- add your button handler code here..
        if (this->addsNewEvent)
        {
            this->cancelAndDisappear();
        }
        else
        {
            this->removeEvent();
            this->dismiss();
        }
        //[/UserButtonCode_removeEventButton]
    }
    else if (buttonThatWasClicked == okButton.get())
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        if (textEditor->getText().isNotEmpty())
        {
            this->dismiss();
        }
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void TimeSignatureDialog::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    //[/UserCode_visibilityChanged]
}

void TimeSignatureDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void TimeSignatureDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}

void TimeSignatureDialog::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDisappear();
    }
    else
    {
        const int targetIndex = commandId - CommandIDs::SelectTimeSignature;
        if (targetIndex >= 0 && targetIndex < this->defailtMeters.size())
        {
            const String title = this->defailtMeters.getAllKeys()[targetIndex];
            const String time(this->defailtMeters[title]);
            this->textEditor->setText(time, true);
        }
    }
    //[/UserCode_handleCommandMessage]
}

void TimeSignatureDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

TimeSignatureDialog *TimeSignatureDialog::createEditingDialog(Component &owner, const TimeSignatureEvent &event)
{
    return new TimeSignatureDialog(owner, static_cast<TimeSignaturesSequence *>(event.getSequence()), event, false, 0.f);
}

TimeSignatureDialog *TimeSignatureDialog::createAddingDialog(Component &owner, TimeSignaturesSequence *annotationsLayer, float targetBeat)
{
    return new TimeSignatureDialog(owner, annotationsLayer, TimeSignatureEvent(), true, targetBeat);
}

void TimeSignatureDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void TimeSignatureDialog::sendEventChange(const TimeSignatureEvent &newEvent)
{
    if (this->originalSequence != nullptr)
    {
        if (this->addsNewEvent)
        {
            this->originalSequence->change(this->originalEvent, newEvent, true);
            this->originalEvent = newEvent;
        }
        else
        {
            this->cancelChangesIfAny();
            this->originalSequence->checkpoint();
            this->originalSequence->change(this->originalEvent, newEvent, true);
            this->hasMadeChanges = true;
        }
    }
}

void TimeSignatureDialog::removeEvent()
{
    if (this->originalSequence != nullptr)
    {
        if (this->addsNewEvent)
        {
            this->originalSequence->remove(this->originalEvent, true);
        }
        else
        {
            this->cancelChangesIfAny();
            this->originalSequence->checkpoint();
            this->originalSequence->remove(this->originalEvent, true);
            this->hasMadeChanges = true;
        }
    }
}

void TimeSignatureDialog::cancelChangesIfAny()
{
    if (!this->addsNewEvent &&
        this->hasMadeChanges &&
        this->originalSequence != nullptr)
    {
        this->originalSequence->undo();
        this->hasMadeChanges = false;
    }
}

void TimeSignatureDialog::textEditorTextChanged(TextEditor&)
{
    this->updateOkButtonState();

    const String meterString(this->textEditor->getText());
    if (meterString.isNotEmpty())
    {
        int numerator;
        int denominator;
        TimeSignatureEvent::parseString(meterString, numerator, denominator);

        TimeSignatureEvent newEvent = this->originalEvent
            .withNumerator(numerator)
            .withDenominator(denominator);

        this->sendEventChange(newEvent);
    }
}

void TimeSignatureDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->textEditorFocusLost(ed);
}

void TimeSignatureDialog::textEditorEscapeKeyPressed(TextEditor&)
{
    this->cancelAndDisappear();
}

void TimeSignatureDialog::textEditorFocusLost(TextEditor&)
{
    this->updateOkButtonState();

    const auto *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->textEditor->getText().isNotEmpty() &&
        focusedComponent != this->okButton.get() &&
        focusedComponent != this->removeEventButton.get())
    {
        this->dismiss();
    }
    else
    {
        this->textEditor->grabKeyboardFocus();
    }
}

void TimeSignatureDialog::timerCallback()
{
    if (!this->textEditor->hasKeyboardFocus(false))
    {
        this->textEditor->grabKeyboardFocus();
        this->textEditor->selectAll();
        this->stopTimer();
    }
}

void TimeSignatureDialog::cancelAndDisappear()
{
    this->cancelChangesIfAny(); // Discards possible changes

    if (this->addsNewEvent &&
        this->originalSequence != nullptr)
    {
        this->originalSequence->undo(); // Discards new event
    }

    this->dismiss();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TimeSignatureDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public TextEditor::Listener, private Timer"
                 constructorParams="Component &amp;owner, TimeSignaturesSequence *timeSequence, const TimeSignatureEvent &amp;editedEvent, bool shouldAddNewEvent, float targetBeat"
                 variableInitialisers="originalEvent(editedEvent),&#10;originalSequence(timeSequence),&#10;ownerComponent(owner),&#10;defailtMeters(getDefaultMeters()),&#10;addsNewEvent(shouldAddNewEvent),&#10;hasMadeChanges(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="370" initialHeight="185">
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
  <GENERICCOMPONENT name="" id="524df900a9089845" memberName="comboPrimer" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 12 24M 72M" class="MobileComboBox::Primer"
                    params=""/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 20 32M 36" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="removeEventButton"
              virtualName="" explicitFocusOrder="0" pos="4 4Rr 180 48" buttonText="..."
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 181 48" buttonText="..."
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1fb927654787aaf4" memberName="separatorV" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4Rr 2 48" sourceFile="../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <TEXTEDITOR name="" id="3f330f1d57714294" memberName="textEditor" virtualName=""
              explicitFocusOrder="0" pos="0Cc 68 48M 32" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

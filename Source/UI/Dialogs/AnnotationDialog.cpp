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

#include "AnnotationDialog.h"

//[MiscUserDefs]
#include "AnnotationsSequence.h"
#include "CommandIDs.h"

static Array<String> getDynamics()
{
    return {
        "Pianissimo",
        "Piano",
        "Mezzo-piano",
        "Mezzo-forte",
        "Forte",
        "Fortissimo",
        "Al niente",
        "Calmando",
        "Crescendo",
        "Dal niente",
        "Diminuendo",
        "Marcato",
        "Smorzando"
    };
}

static Array<Colour> getColours()
{
    return {
        Colours::greenyellow,
        Colours::gold,
        Colours::tomato,
        Colours::orangered,
        Colours::red,
        Colours::fuchsia,
        Colours::white,
        Colours::royalblue,
        Colours::red,
        Colours::aqua,
        Colours::blue,
        Colours::lime,
        Colours::greenyellow
    };
}
//[/MiscUserDefs]

AnnotationDialog::AnnotationDialog(Component &owner, AnnotationsSequence *sequence, const AnnotationEvent &editedEvent, bool shouldAddNewEvent, float targetBeat)
    : originalEvent(editedEvent),
      originalSequence(sequence),
      ownerComponent(owner),
      addsNewEvent(shouldAddNewEvent),
      hasMadeChanges(false)
{
    addAndMakeVisible (background = new DialogPanel());
    addAndMakeVisible (comboPrimer = new MobileComboBox::Primer());

    addAndMakeVisible (messageLabel = new Label (String(),
                                                 TRANS("...")));
    messageLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    messageLabel->setJustificationType (Justification::centred);
    messageLabel->setEditable (false, false, false);

    addAndMakeVisible (removeEventButton = new TextButton (String()));
    removeEventButton->setButtonText (TRANS("..."));
    removeEventButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    removeEventButton->addListener (this);

    addAndMakeVisible (okButton = new TextButton (String()));
    okButton->setButtonText (TRANS("..."));
    okButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    okButton->addListener (this);

    addAndMakeVisible (separatorH = new SeparatorHorizontal());
    addAndMakeVisible (separatorV = new SeparatorVertical());
    addAndMakeVisible (colourSwatches = new ColourSwatches());

    addAndMakeVisible (textEditor = new TextEditor (String()));
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (String());


    //[UserPreSize]
    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);

    if (this->addsNewEvent)
    {
        Random r;
        const auto keys(getDynamics());
        const int i = r.nextInt(keys.size());
        const String key(keys[i]);
        const Colour colour(getColours()[i]);
        this->originalEvent = AnnotationEvent(sequence, targetBeat, key, colour);

        sequence->checkpoint();
        sequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS("dialog::annotation::add::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::annotation::add::proceed"));
        this->removeEventButton->setButtonText(TRANS("dialog::common::cancel"));
    }
    else
    {
        this->messageLabel->setText(TRANS("dialog::annotation::edit::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::annotation::edit::apply"));
        this->removeEventButton->setButtonText(TRANS("dialog::annotation::edit::delete"));
    }

    this->colourSwatches->setSelectedColour(this->originalEvent.getTrackColour());

    this->textEditor->addListener(this);
    this->textEditor->setFont(21.f);
    this->textEditor->setText(this->originalEvent.getDescription(), dontSendNotification);

    this->separatorH->setAlphaMultiplier(2.5f);
    //[/UserPreSize]

    setSize (450, 220);

    //[Constructor]
    this->updatePosition();
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->updateOkButtonState();

    const auto dynamics = getDynamics();
    const auto colours = getColours();
    MenuPanel::Menu menu;
    for (int i = 0; i < getDynamics().size(); ++i)
    {
        const auto cmd = MenuItem::item(Icons::annotation,
            CommandIDs::JumpToAnnotation + i, dynamics[i])->
            colouredWith(colours[i]);
        menu.add(cmd);
    }
    this->comboPrimer->initWith(this->textEditor.get(), menu);

    this->startTimer(100);
    //[/Constructor]
}

AnnotationDialog::~AnnotationDialog()
{
    //[Destructor_pre]
    this->textEditor->removeListener(this);
    //[/Destructor_pre]

    background = nullptr;
    comboPrimer = nullptr;
    messageLabel = nullptr;
    removeEventButton = nullptr;
    okButton = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;
    colourSwatches = nullptr;
    textEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AnnotationDialog::paint (Graphics& g)
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

void AnnotationDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds ((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    comboPrimer->setBounds ((getWidth() / 2) - ((getWidth() - 24) / 2), 12, getWidth() - 24, getHeight() - 72);
    messageLabel->setBounds ((getWidth() / 2) - ((getWidth() - 48) / 2), 4 + 16, getWidth() - 48, 36);
    removeEventButton->setBounds (4, getHeight() - 4 - 48, 220, 48);
    okButton->setBounds (getWidth() - 4 - 221, getHeight() - 4 - 48, 221, 48);
    separatorH->setBounds (4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds ((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    colourSwatches->setBounds ((getWidth() / 2) + 2 - ((getWidth() - 56) / 2), 106, getWidth() - 56, 34);
    textEditor->setBounds ((getWidth() / 2) - ((getWidth() - 48) / 2), 66, getWidth() - 48, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AnnotationDialog::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == removeEventButton)
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
    else if (buttonThatWasClicked == okButton)
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

void AnnotationDialog::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    //[/UserCode_visibilityChanged]
}

void AnnotationDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void AnnotationDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}

void AnnotationDialog::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDisappear();
    }
    else
    {
        const int targetIndex = commandId - CommandIDs::JumpToAnnotation;
        const auto dynamics = getDynamics();
        const auto colours = getColours();
        if (targetIndex >= 0 && targetIndex < dynamics.size())
        {
            const String text = dynamics[targetIndex];
            this->colourSwatches->setSelectedColour(colours[targetIndex]);
            this->textEditor->setText(text, true);
        }
    }
    //[/UserCode_handleCommandMessage]
}

void AnnotationDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

AnnotationDialog *AnnotationDialog::createEditingDialog(Component &owner, const AnnotationEvent &event)
{
    return new AnnotationDialog(owner, static_cast<AnnotationsSequence *>(event.getSequence()), event, false, 0.f);
}

AnnotationDialog *AnnotationDialog::createAddingDialog(Component &owner, AnnotationsSequence *annotationsLayer, float targetBeat)
{
    return new AnnotationDialog(owner, annotationsLayer, AnnotationEvent(), true, targetBeat);
}

void AnnotationDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void AnnotationDialog::onColourButtonClicked(ColourButton *clickedButton)
{
    const Colour c(clickedButton->getColour());
    const AnnotationEvent newEvent =
        this->originalEvent.withDescription(this->textEditor->getText()).withColour(c);
    this->sendEventChange(newEvent);
}

void AnnotationDialog::sendEventChange(const AnnotationEvent &newEvent)
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

void AnnotationDialog::removeEvent()
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

void AnnotationDialog::cancelChangesIfAny()
{
    if (!this->addsNewEvent &&
        this->hasMadeChanges &&
        this->originalSequence != nullptr)
    {
        this->originalSequence->undo();
        this->hasMadeChanges = false;
    }
}

void AnnotationDialog::textEditorTextChanged(TextEditor&)
{
    this->updateOkButtonState();

    const String text(this->textEditor->getText());
    AnnotationEvent newEvent = this->originalEvent.withDescription(text);
    const Colour c(this->colourSwatches->getColour());
    this->colourSwatches->setSelectedColour(c);
    newEvent = newEvent.withColour(c);
    this->sendEventChange(newEvent);
}

void AnnotationDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->textEditorFocusLost(ed);
}

void AnnotationDialog::textEditorEscapeKeyPressed(TextEditor&)
{
    this->cancelAndDisappear();
}

void AnnotationDialog::textEditorFocusLost(TextEditor&)
{
    this->updateOkButtonState();

    const Component *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->textEditor->getText().isNotEmpty() &&
        focusedComponent != this->okButton &&
        focusedComponent != this->removeEventButton)
    {
        this->dismiss();
    }
    else
    {
        this->textEditor->grabKeyboardFocus();
    }
}

void AnnotationDialog::timerCallback()
{
    if (!this->textEditor->hasKeyboardFocus(false))
    {
        this->textEditor->grabKeyboardFocus();
        this->textEditor->selectAll();
        this->stopTimer();
    }
}

void AnnotationDialog::cancelAndDisappear()
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

<JUCER_COMPONENT documentType="Component" className="AnnotationDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public TextEditorListener, public ColourButtonListener, private Timer"
                 constructorParams="Component &amp;owner, AnnotationsSequence *sequence, const AnnotationEvent &amp;editedEvent, bool shouldAddNewEvent, float targetBeat"
                 variableInitialisers="originalEvent(editedEvent),&#10;originalSequence(sequence),&#10;ownerComponent(owner),&#10;addsNewEvent(shouldAddNewEvent),&#10;hasMadeChanges(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="450" initialHeight="220">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10" fill="solid: 59000000" hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/DialogPanel.cpp" constructorParams=""/>
  <GENERICCOMPONENT name="" id="524df900a9089845" memberName="comboPrimer" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 12 24M 72M" class="MobileComboBox::Primer"
                    params=""/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 16 48M 36" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="removeEventButton"
              virtualName="" explicitFocusOrder="0" pos="4 4Rr 220 48" buttonText="..."
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 221 48" buttonText="..."
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
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

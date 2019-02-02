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

#include "KeySignatureDialog.h"

//[MiscUserDefs]
#include "CommandIDs.h"
#include "KeySignaturesSequence.h"
#include "Transport.h"
#include "SerializationKeys.h"
#include "Config.h"

static inline void copyColourIfSpecified(Label& l, TextEditor& ed, int colourID, int targetColourID)
{
    if (l.isColourSpecified(colourID) || l.getLookAndFeel().isColourSpecified(colourID))
        ed.setColour(targetColourID, l.findColour(colourID));
}

//[/MiscUserDefs]

KeySignatureDialog::KeySignatureDialog(Component &owner, Transport &transport, KeySignaturesSequence *keySequence, const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat)
    : transport(transport),
      originalEvent(editedEvent),
      originalSequence(keySequence),
      ownerComponent(owner),
      defaultScales(App::Config().getScales()->getAll()),
      addsNewEvent(shouldAddNewEvent),
      hasMadeChanges(false),
      key(0)
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
    this->keySelector.reset(new KeySelector());
    this->addAndMakeVisible(keySelector.get());

    this->scaleEditor.reset(new ScaleEditor());
    this->addAndMakeVisible(scaleEditor.get());

    this->playButton.reset(new PlayButton(this));
    this->addAndMakeVisible(playButton.get());
    this->scaleNameEditor.reset(new TextEditor(String()));
    this->addAndMakeVisible(scaleNameEditor.get());
    scaleNameEditor->setMultiLine (false);
    scaleNameEditor->setReturnKeyStartsNewLine (false);
    scaleNameEditor->setReadOnly (false);
    scaleNameEditor->setScrollbarsShown (true);
    scaleNameEditor->setCaretVisible (true);
    scaleNameEditor->setPopupMenuEnabled (true);
    scaleNameEditor->setText (String());


    //[UserPreSize]
    this->transport.stopPlayback();

    this->separatorH->setAlphaMultiplier(2.5f);
    this->scaleNameEditor->addListener(this);
    this->scaleNameEditor->setFont(21.f);

    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);

    if (this->addsNewEvent)
    {
        Random r;
        const auto i = r.nextInt(this->defaultScales.size());
        this->key = 0;
        this->scale = this->defaultScales[i];
        this->scaleEditor->setScale(this->scale);
        this->keySelector->setSelectedKey(this->key);
        this->scaleNameEditor->setText(this->scale->getLocalizedName());
        this->originalEvent = KeySignatureEvent(this->originalSequence, this->scale, targetBeat, this->key);

        this->originalSequence->checkpoint();
        this->originalSequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS("dialog::keysignature::add::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::keysignature::add::proceed"));
        this->removeEventButton->setButtonText(TRANS("dialog::common::cancel"));
    }
    else
    {
        this->key = this->originalEvent.getRootKey();
        this->scale = this->originalEvent.getScale();
        this->scaleEditor->setScale(this->scale);
        this->keySelector->setSelectedKey(this->key);
        this->scaleNameEditor->setText(this->scale->getLocalizedName(), dontSendNotification);

        this->messageLabel->setText(TRANS("dialog::keysignature::edit::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::keysignature::edit::apply"));
        this->removeEventButton->setButtonText(TRANS("dialog::keysignature::edit::delete"));
    }
    //[/UserPreSize]

    this->setSize(460, 260);

    //[Constructor]
    this->updatePosition();
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->updateOkButtonState();

    MenuPanel::Menu menu;
    for (int i = 0; i < this->defaultScales.size(); ++i)
    {
        const auto &s = this->defaultScales.getUnchecked(i);
        menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::SelectScale + i, s->getLocalizedName()));
    }
    this->comboPrimer->initWith(this->scaleNameEditor.get(), menu);

    this->startTimer(100);
    //[/Constructor]
}

KeySignatureDialog::~KeySignatureDialog()
{
    //[Destructor_pre]
    this->comboPrimer->cleanup();
    this->transport.stopPlayback();
    this->scaleNameEditor->removeListener(this);
    //[/Destructor_pre]

    background = nullptr;
    comboPrimer = nullptr;
    messageLabel = nullptr;
    removeEventButton = nullptr;
    okButton = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;
    keySelector = nullptr;
    scaleEditor = nullptr;
    playButton = nullptr;
    scaleNameEditor = nullptr;

    //[Destructor]
    //[/Destructor]
}

void KeySignatureDialog::paint (Graphics& g)
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

void KeySignatureDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    comboPrimer->setBounds((getWidth() / 2) - ((getWidth() - 24) / 2), 12, getWidth() - 24, getHeight() - 72);
    messageLabel->setBounds((getWidth() / 2) - ((getWidth() - 32) / 2), 4 + 12, getWidth() - 32, 36);
    removeEventButton->setBounds(4, getHeight() - 4 - 48, 225, 48);
    okButton->setBounds(getWidth() - 4 - 226, getHeight() - 4 - 48, 226, 48);
    separatorH->setBounds(4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    keySelector->setBounds((getWidth() / 2) + 2 - ((getWidth() - 40) / 2), 58, getWidth() - 40, 34);
    scaleEditor->setBounds((getWidth() / 2) + 2 - ((getWidth() - 40) / 2), 108, getWidth() - 40, 34);
    playButton->setBounds(getWidth() - 25 - 40, 148, 40, 40);
    scaleNameEditor->setBounds((getWidth() / 2) + -20 - ((getWidth() - 100) / 2), 152, getWidth() - 100, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void KeySignatureDialog::buttonClicked(Button* buttonThatWasClicked)
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
        if (this->scaleNameEditor->getText().isNotEmpty())
        {
            this->dismiss();
        }
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void KeySignatureDialog::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    //[/UserCode_visibilityChanged]
}

void KeySignatureDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void KeySignatureDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}

void KeySignatureDialog::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDisappear();
    }
    else if (commandId == CommandIDs::TransportStartPlayback)
    {
        // Play scale forward (and backward?)
        auto scaleKeys = this->scale->getUpScale();
        scaleKeys.addArray(this->scale->getDownScale());
        const float timeFactor = 1.0; // MS_PER_BEAT;

        MidiMessageSequence s;
        for (int i = 0; i < scaleKeys.size(); ++i)
        {
            const int key = MIDDLE_C + this->key + scaleKeys.getUnchecked(i);

            MidiMessage eventNoteOn(MidiMessage::noteOn(1, key, 1.f));
            const double startTime = double(i) * timeFactor;
            eventNoteOn.setTimeStamp(startTime);

            MidiMessage eventNoteOff(MidiMessage::noteOff(1, key));
            const double endTime = double(i + 0.5) * timeFactor;
            eventNoteOff.setTimeStamp(endTime);

            s.addEvent(eventNoteOn);
            s.addEvent(eventNoteOff);
        }

        s.updateMatchedPairs();
        this->transport.probeSequence(s);
        //this->playButton->setPlaying(true);
    }
    else if (commandId == CommandIDs::TransportPausePlayback)
    {
        this->transport.stopPlayback();
        //this->playButton->setPlaying(false);
    }
    else
    {
        const int scaleIndex = commandId - CommandIDs::SelectScale;
        if (scaleIndex >= 0 && scaleIndex < this->defaultScales.size())
        {
            this->scale = this->defaultScales[scaleIndex];
            this->scaleEditor->setScale(this->scale);
            this->scaleNameEditor->setText(this->scale->getLocalizedName(), false);
            const KeySignatureEvent newEvent = this->originalEvent.withRootKey(this->key).withScale(this->scale);
            this->sendEventChange(newEvent);
        }
    }
    //[/UserCode_handleCommandMessage]
}

void KeySignatureDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

KeySignatureDialog *KeySignatureDialog::createEditingDialog(Component &owner, Transport &transport, const KeySignatureEvent &event)
{
    return new KeySignatureDialog(owner, transport, static_cast<KeySignaturesSequence *>(event.getSequence()), event, false, 0.f);
}

KeySignatureDialog *KeySignatureDialog::createAddingDialog(Component &owner, Transport &transport, KeySignaturesSequence *annotationsLayer, float targetBeat)
{
    return new KeySignatureDialog(owner, transport, annotationsLayer, KeySignatureEvent(), true, targetBeat);
}

void KeySignatureDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->scaleNameEditor->getText().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void KeySignatureDialog::sendEventChange(const KeySignatureEvent &newEvent)
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

void KeySignatureDialog::removeEvent()
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

bool KeySignatureDialog::cancelChangesIfAny()
{
    if (!this->addsNewEvent &&
        this->hasMadeChanges &&
        this->originalSequence != nullptr)
    {
        this->originalSequence->undo();
        this->hasMadeChanges = false;
        return true;
    }

    return false;
}

void KeySignatureDialog::cancelAndDisappear()
{
    this->cancelChangesIfAny(); // Discards possible changes

    if (this->addsNewEvent &&
        this->originalSequence != nullptr)
    {
        this->originalSequence->undo(); // Discards new event
    }

    this->dismiss();
}

void KeySignatureDialog::onKeyChanged(int key)
{
    if (this->key != key)
    {
        this->key = key;
        KeySignatureEvent newEvent = this->originalEvent.withRootKey(key).withScale(this->scale);
        this->sendEventChange(newEvent);
    }
}

void KeySignatureDialog::onScaleChanged(const Scale::Ptr scale)
{
    if (!this->scale->isEquivalentTo(scale))
    {
        this->scale = scale;

        // Update name, if found equivalent:
        for (int i = 0; i < this->defaultScales.size(); ++i)
        {
            const auto &s = this->defaultScales.getUnchecked(i);
            if (s->isEquivalentTo(scale))
            {
                this->scaleNameEditor->setText(s->getLocalizedName());
                this->scaleEditor->setScale(s);
                this->scale = s;
                break;
            }
        }

        KeySignatureEvent newEvent = this->originalEvent.withRootKey(this->key).withScale(this->scale);
        this->sendEventChange(newEvent);

        // Don't erase user's text, but let user know the scale is unknown - how?
        //this->scaleNameEditor->setText({}, dontSendNotification);
    }
}

void KeySignatureDialog::textEditorTextChanged(TextEditor &ed)
{
    this->updateOkButtonState();
    this->scale = this->scale->withName(this->scaleNameEditor->getText());
    this->scaleEditor->setScale(this->scale);
    const KeySignatureEvent newEvent = this->originalEvent.withRootKey(this->key).withScale(scale);
    this->sendEventChange(newEvent);
}

void KeySignatureDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->textEditorFocusLost(ed);
}

void KeySignatureDialog::textEditorEscapeKeyPressed(TextEditor &)
{
    this->cancelAndDisappear();
}

void KeySignatureDialog::textEditorFocusLost(TextEditor &)
{
    this->updateOkButtonState();

    const Component *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->scaleNameEditor->getText().isNotEmpty() &&
        focusedComponent != this->okButton.get() &&
        focusedComponent != this->removeEventButton.get())
    {
        this->dismiss();
    }
    else
    {
        this->scaleNameEditor->grabKeyboardFocus();
    }
}

void KeySignatureDialog::timerCallback()
{
    if (!this->scaleNameEditor->hasKeyboardFocus(false))
    {
        this->scaleNameEditor->grabKeyboardFocus();
        this->scaleNameEditor->selectAll();
        this->stopTimer();
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="KeySignatureDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public TextEditor::Listener, public ScaleEditor::Listener, public KeySelector::Listener, private Timer"
                 constructorParams="Component &amp;owner, Transport &amp;transport, KeySignaturesSequence *keySequence, const KeySignatureEvent &amp;editedEvent, bool shouldAddNewEvent, float targetBeat"
                 variableInitialisers="transport(transport),&#10;originalEvent(editedEvent),&#10;originalSequence(keySequence),&#10;ownerComponent(owner),&#10;defaultScales(App::Config().getScales()->getAll()),&#10;addsNewEvent(shouldAddNewEvent),&#10;hasMadeChanges(false),&#10;key(0)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="460" initialHeight="260">
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
         explicitFocusOrder="0" pos="0Cc 12 32M 36" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="removeEventButton"
              virtualName="" explicitFocusOrder="0" pos="4 4Rr 225 48" buttonText="..."
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 226 48" buttonText="..."
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1fb927654787aaf4" memberName="separatorV" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4Rr 2 48" sourceFile="../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="fa164e6b39caa19f" memberName="keySelector" virtualName=""
                    explicitFocusOrder="0" pos="2Cc 58 40M 34" class="KeySelector"
                    params=""/>
  <GENERICCOMPONENT name="" id="9716b1069cf3430e" memberName="scaleEditor" virtualName=""
                    explicitFocusOrder="0" pos="2Cc 108 40M 34" class="ScaleEditor"
                    params=""/>
  <JUCERCOMP name="" id="a80d33e93bb4cadb" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="25Rr 148 40 40" sourceFile="../Common/PlayButton.cpp"
             constructorParams="this"/>
  <TEXTEDITOR name="" id="3f330f1d57714294" memberName="scaleNameEditor" virtualName=""
              explicitFocusOrder="0" pos="-20Cc 152 100M 32" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

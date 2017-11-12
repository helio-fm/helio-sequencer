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

// TODO scales manager or resource manager

static Array<Scale> loadDefaultScales()
{
    Array<Scale> result;
    const String scalesXml =
        String(CharPointer_UTF8(BinaryData::DefaultScales_xml),
            BinaryData::DefaultScales_xmlSize);

    ScopedPointer<XmlElement> xml(XmlDocument::parse(scalesXml));

    if (xml == nullptr) { return result; }

    const XmlElement *root = xml->hasTagName(Serialization::Core::scales) ?
        xml.get() : xml->getChildByName(Serialization::Core::scales);

    if (root == nullptr) { return result; }

    forEachXmlChildElementWithTagName(*root, scaleRoot, Serialization::Core::scale)
    {
        Scale s;
        s.deserialize(*scaleRoot);
        result.add(s);
    }

    return result;
}

static Array<Scale> kDefaultScales = loadDefaultScales();

//[/MiscUserDefs]

KeySignatureDialog::KeySignatureDialog(Component &owner, Transport &transport, KeySignaturesSequence *keySequence, const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat)
    : transport(transport),
      originalEvent(editedEvent),
      originalSequence(keySequence),
      ownerComponent(owner),
      addsNewEvent(shouldAddNewEvent),
      hasMadeChanges(false),
      key(0)
{
    addAndMakeVisible (background = new PanelC());
    addAndMakeVisible (messageLabel = new Label (String(),
                                                 TRANS("...")));
    messageLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    messageLabel->setJustificationType (Justification::centred);
    messageLabel->setEditable (false, false, false);
    messageLabel->setColour (Label::textColourId, Colours::white);
    messageLabel->setColour (TextEditor::textColourId, Colours::black);
    messageLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (removeEventButton = new TextButton (String()));
    removeEventButton->setButtonText (TRANS("..."));
    removeEventButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    removeEventButton->addListener (this);

    addAndMakeVisible (okButton = new TextButton (String()));
    okButton->setButtonText (TRANS("..."));
    okButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    okButton->addListener (this);

    addAndMakeVisible (scaleNameEditor = new ComboBox (String()));
    scaleNameEditor->setEditableText (true);
    scaleNameEditor->setJustificationType (Justification::centredLeft);
    scaleNameEditor->setTextWhenNothingSelected (String());
    scaleNameEditor->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    scaleNameEditor->addListener (this);

    addAndMakeVisible (separatorH = new SeparatorHorizontal());
    addAndMakeVisible (separatorV = new SeparatorVertical());
    addAndMakeVisible (keySelector = new KeySelector());

    addAndMakeVisible (scaleEditor = new ScaleEditor());

    addAndMakeVisible (playButton = new PlayButton());

    //[UserPreSize]
    this->transport.stopPlayback();

    this->separatorH->setAlphaMultiplier(2.5f);
    this->scaleNameEditor->addListener(this);

    // TODO my own nice popup menu

    for (int i = 0; i < kDefaultScales.size(); ++i)
    {
        const auto &s = kDefaultScales.getUnchecked(i);
        this->scaleNameEditor->addItem(s.getName(), i + 1);
    }

    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);

    if (this->addsNewEvent)
    {
        Random r;
        const auto i = r.nextInt(kDefaultScales.size());
        this->key = 0;
        this->scale = kDefaultScales[i];
        this->scaleEditor->setScale(this->scale);
        this->keySelector->setSelectedKey(this->key);
        this->scaleNameEditor->setSelectedItemIndex(i, dontSendNotification);
        this->originalEvent = KeySignatureEvent(this->originalSequence, targetBeat, this->key, this->scale);

        this->originalSequence->checkpoint();
        this->originalSequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS("dialog::keysignature::add::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::keysignature::add::proceed"));
        this->removeEventButton->setButtonText(TRANS("dialog::keysignature::add::cancel"));
    }
    else
    {
        this->key = this->originalEvent.getRootKey();
        this->scale = this->originalEvent.getScale();
        this->scaleEditor->setScale(this->scale);
        this->keySelector->setSelectedKey(this->key);
        this->scaleNameEditor->setText(this->scale.getName(), dontSendNotification);

        this->messageLabel->setText(TRANS("dialog::keysignature::edit::caption"), dontSendNotification);
        this->okButton->setButtonText(TRANS("dialog::keysignature::edit::apply"));
        this->removeEventButton->setButtonText(TRANS("dialog::keysignature::edit::delete"));
    }
    //[/UserPreSize]

    setSize (430, 250);

    //[Constructor]
    this->rebound();
    this->setWantsKeyboardFocus(true);
    this->setInterceptsMouseClicks(true, true);
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->updateOkButtonState();
    //[/Constructor]
}

KeySignatureDialog::~KeySignatureDialog()
{
    //[Destructor_pre]
    this->transport.stopPlayback();
    scaleNameEditor->removeListener(this);
    FadingDialog::fadeOut();
    //[/Destructor_pre]

    background = nullptr;
    messageLabel = nullptr;
    removeEventButton = nullptr;
    okButton = nullptr;
    scaleNameEditor = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;
    keySelector = nullptr;
    scaleEditor = nullptr;
    playButton = nullptr;

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

    background->setBounds ((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    messageLabel->setBounds ((getWidth() / 2) - ((getWidth() - 32) / 2), 4 + 12, getWidth() - 32, 36);
    removeEventButton->setBounds (4, getHeight() - 4 - 48, 210, 48);
    okButton->setBounds (getWidth() - 4 - 211, getHeight() - 4 - 48, 211, 48);
    scaleNameEditor->setBounds ((getWidth() / 2) + -19 - ((getWidth() - 102) / 2), 146, getWidth() - 102, 36);
    separatorH->setBounds (4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds ((getWidth() / 2) - (2 / 2), getHeight() - 4 - 48, 2, 48);
    keySelector->setBounds ((getWidth() / 2) + 2 - ((getWidth() - 40) / 2), 56, getWidth() - 40, 34);
    scaleEditor->setBounds ((getWidth() / 2) + 2 - ((getWidth() - 40) / 2), 104, getWidth() - 40, 34);
    playButton->setBounds ((getWidth() / 2) + 170 - (40 / 2), 144, 40, 40);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void KeySignatureDialog::buttonClicked (Button* buttonThatWasClicked)
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
            this->disappear();
        }
        //[/UserButtonCode_removeEventButton]
    }
    else if (buttonThatWasClicked == okButton)
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        if (scaleNameEditor->getText().isNotEmpty())
        {
            this->disappear();
        }
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void KeySignatureDialog::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == scaleNameEditor)
    {
        //[UserComboBoxCode_scaleNameEditor] -- add your combo box handling code here..
        for (const auto &s : kDefaultScales)
        {
            if (s.getName() == this->scaleNameEditor->getText())
            {
                this->scale = s;
                this->scaleEditor->setScale(this->scale);
                KeySignatureEvent newEvent = this->originalEvent.withRootKey(this->key).withScale(scale);
                this->sendEventChange(newEvent);
                return;
            }
        }

        this->scale = this->scale.withName(this->scaleNameEditor->getText());
        this->scaleEditor->setScale(this->scale);
        KeySignatureEvent newEvent = this->originalEvent.withRootKey(this->key).withScale(scale);
        this->sendEventChange(newEvent);
        return;

        //[/UserComboBoxCode_scaleNameEditor]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void KeySignatureDialog::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    //[/UserCode_visibilityChanged]
}

void KeySignatureDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentHierarchyChanged]
}

void KeySignatureDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->rebound();
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
        auto scaleKeys = this->scale.getUpScale();
        scaleKeys.addArray(this->scale.getDownScale());

        MidiMessageSequence s;
        for (int i = 0; i < scaleKeys.size(); ++i)
        {
            const int key = KEY_C5 + this->key + scaleKeys.getUnchecked(i);

            MidiMessage eventNoteOn(MidiMessage::noteOn(1, key, 1.f));
            const float &startTime = float(i * Transport::millisecondsPerBeat);
            eventNoteOn.setTimeStamp(startTime);

            MidiMessage eventNoteOff(MidiMessage::noteOff(1, key));
            const float &endTime = (i + 0.5f) * Transport::millisecondsPerBeat;
            eventNoteOff.setTimeStamp(endTime);

            s.addEvent(eventNoteOn);
            s.addEvent(eventNoteOff);
        }

        double timeOffset = (scaleKeys.size() + 1.0)  * Transport::millisecondsPerBeat;

        // Then play triad chord
        const auto triadKeys = scale.getTriad(Scale::Tonic, false);
        for (int i = 0; i < triadKeys.size(); ++i)
        {
            const int key = KEY_C5 + this->key + triadKeys.getUnchecked(i);

            MidiMessage eventNoteOn(MidiMessage::noteOn(1, key, 1.f));
            const float &startTime = float(i * Transport::millisecondsPerBeat);
            eventNoteOn.setTimeStamp(timeOffset);

            MidiMessage eventNoteOff(MidiMessage::noteOff(1, key));
            const float &endTime = float((timeOffset + 0.5f) * Transport::millisecondsPerBeat);
            eventNoteOff.setTimeStamp(endTime);

            s.addEvent(eventNoteOn);
            s.addEvent(eventNoteOff);
        }

        s.updateMatchedPairs();
        this->transport.playSequence(s);
        //this->playButton->setPlaying(true);
    }
    else if (commandId == CommandIDs::TransportPausePlayback)
    {
        this->transport.stopPlayback();
        //this->playButton->setPlaying(false);
    }
    //[/UserCode_handleCommandMessage]
}

bool KeySignatureDialog::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancelAndDisappear();
        return true;
    }
    else if (key.isKeyCode(KeyPress::returnKey) ||
             key.isKeyCode(KeyPress::tabKey))
    {
        if (scaleNameEditor->getText().isNotEmpty())
        {
            this->disappear();
        }

        return true;
    }

    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
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

void KeySignatureDialog::sendEventChange(KeySignatureEvent newEvent)
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

void KeySignatureDialog::disappear()
{
    delete this;
}

void KeySignatureDialog::cancelAndDisappear()
{
    this->cancelChangesIfAny(); // Discards possible changes

    if (this->addsNewEvent &&
        this->originalSequence != nullptr)
    {
        this->originalSequence->undo(); // Discards new event
    }

    this->disappear();
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

void KeySignatureDialog::onScaleChanged(Scale scale)
{
    if (!this->scale.isEquivalentTo(scale))
    {
        this->scale = scale;

        // Update name, if found equivalent:
        for (int i = 0; i < kDefaultScales.size(); ++i)
        {
            const auto &s = kDefaultScales.getUnchecked(i);
            if (s.isEquivalentTo(scale))
            {
                this->scaleNameEditor->setSelectedItemIndex(i, dontSendNotification);
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

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="KeySignatureDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog, public TextEditorListener, public ScaleEditor::Listener, public KeySelector::Listener"
                 constructorParams="Component &amp;owner, Transport &amp;transport, KeySignaturesSequence *keySequence, const KeySignatureEvent &amp;editedEvent, bool shouldAddNewEvent, float targetBeat"
                 variableInitialisers="transport(transport),&#10;originalEvent(editedEvent),&#10;originalSequence(keySequence),&#10;ownerComponent(owner),&#10;addsNewEvent(shouldAddNewEvent),&#10;hasMadeChanges(false),&#10;key(0)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="430" initialHeight="250">
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
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/PanelC.cpp" constructorParams=""/>
  <LABEL name="" id="cf32360d33639f7f" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 12 32M 36" posRelativeY="e96b77baef792d3a"
         textCol="ffffffff" edTextCol="ff000000" edBkgCol="0" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21" kerning="0" bold="0"
         italic="0" justification="36"/>
  <TEXTBUTTON name="" id="ccad5f07d4986699" memberName="removeEventButton"
              virtualName="" explicitFocusOrder="0" pos="4 4Rr 210 48" buttonText="..."
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 211 48" buttonText="..."
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="" id="1923d71c308d2169" memberName="scaleNameEditor" virtualName=""
            explicitFocusOrder="0" pos="-19Cc 146 102M 36" editable="1" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1fb927654787aaf4" memberName="separatorV" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4Rr 2 48" sourceFile="../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="fa164e6b39caa19f" memberName="keySelector" virtualName=""
                    explicitFocusOrder="0" pos="2Cc 56 40M 34" class="KeySelector"
                    params=""/>
  <GENERICCOMPONENT name="" id="9716b1069cf3430e" memberName="scaleEditor" virtualName=""
                    explicitFocusOrder="0" pos="2Cc 104 40M 34" class="ScaleEditor"
                    params=""/>
  <JUCERCOMP name="" id="a80d33e93bb4cadb" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="170Cc 144 40 40" sourceFile="../Common/PlayButton.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

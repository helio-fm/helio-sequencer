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

#include "ArpeggiatorEditorPanel.h"

//[MiscUserDefs]
#include "PianoRoll.h"
#include "Lasso.h"
#include "PianoRollToolbox.h"
#include "MidiLayer.h"
#include "NoteComponent.h"
#include "ProjectTreeItem.h"
#include "Transport.h"
#include "CommandItemComponent.h"
#include "SerializationKeys.h"
#include "InternalClipboard.h"
#include "ArpeggiatorsManager.h"


//ScopedPointer<CodeEditorComponent> codeEditor;
//[/MiscUserDefs]

ArpeggiatorEditorPanel::ArpeggiatorEditorPanel(ProjectTreeItem &parentProject, PianoRoll &targetRoll)
    : roll(targetRoll),
      project(parentProject),
      hasMadeChanges(false)
{
    addAndMakeVisible (bg = new PanelBackgroundC());
    addAndMakeVisible (editor = new CodeEditorComponent (this->document, &this->tokeniser));

    addAndMakeVisible (resetButton = new CommandItemComponent (this, nullptr, CommandItem::withParams(Icons::reset, CommandIDs::ResetArpeggiatorChanges)));

    addAndMakeVisible (playButton = new PlayButton());
    addAndMakeVisible (shadowDown = new ShadowDownwards());
    addAndMakeVisible (s2 = new SeparatorHorizontal());
    addAndMakeVisible (reverseToggleButton = new ToggleButton (String()));
    reverseToggleButton->setButtonText (TRANS("Should be reversed"));
    reverseToggleButton->addListener (this);
    reverseToggleButton->setToggleState (true, dontSendNotification);

    addAndMakeVisible (arpsList = new ComboBox (String()));
    arpsList->setEditableText (false);
    arpsList->setJustificationType (Justification::centredLeft);
    arpsList->setTextWhenNothingSelected (String());
    arpsList->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    arpsList->addListener (this);

    addAndMakeVisible (relativeMappingToggleButton = new ToggleButton (String()));
    relativeMappingToggleButton->setButtonText (TRANS("Arp seqence as is (don\'t map to chord)"));
    relativeMappingToggleButton->addListener (this);
    relativeMappingToggleButton->setToggleState (true, dontSendNotification);

    addAndMakeVisible (limitToChordToggleButton = new ToggleButton (String()));
    limitToChordToggleButton->setButtonText (TRANS("Every chord starts from 0"));
    limitToChordToggleButton->addListener (this);
    limitToChordToggleButton->setToggleState (true, dontSendNotification);

    addAndMakeVisible (s1 = new SeparatorHorizontal());
    addAndMakeVisible (label = new Label (String(),
                                          TRANS("Scale:")));
    label->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (scaleList = new ComboBox (String()));
    scaleList->setEditableText (false);
    scaleList->setJustificationType (Justification::centredLeft);
    scaleList->setTextWhenNothingSelected (TRANS("1.0"));
    scaleList->setTextWhenNoChoicesAvailable (TRANS("1.0"));
    scaleList->addItem (TRANS("0.25"), 1);
    scaleList->addItem (TRANS("0.5"), 2);
    scaleList->addItem (TRANS("0.66"), 3);
    scaleList->addItem (TRANS("0.75"), 4);
    scaleList->addItem (TRANS("1.0"), 5);
    scaleList->addItem (TRANS("1.5"), 6);
    scaleList->addItem (TRANS("2.0"), 7);
    scaleList->addItem (TRANS("2.5"), 8);
    scaleList->addItem (TRANS("3.0"), 9);
    scaleList->addListener (this);

    addAndMakeVisible (addArpButton = new TextButton (String()));
    addArpButton->setButtonText (TRANS("Create new"));
    addArpButton->addListener (this);

    addAndMakeVisible (s3 = new SeparatorHorizontal());
    addAndMakeVisible (nameLabel = new Label (String(),
                                              TRANS("Name")));
    nameLabel->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    nameLabel->setJustificationType (Justification::centredLeft);
    nameLabel->setEditable (true, true, false);
    nameLabel->setColour (Label::textColourId, Colours::white);
    nameLabel->setColour (TextEditor::textColourId, Colours::black);
    nameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    nameLabel->addListener (this);


    //[UserPreSize]
    this->reloadData();

    this->grabKeyboardFocus();
    this->document.replaceAllContent(InternalClipboard::getCurrentContentAsString());

    ScopedPointer<XmlElement> xml(XmlDocument::parse(this->document.getAllContent()));

    if (xml != nullptr)
    {
        this->currentArp = this->currentArp.withSequenceFromXml(*xml);
        this->applyArpToSelectedNotes(this->currentArp);
    }
    //[/UserPreSize]

    setSize (300, 450);

    //[Constructor]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        Component *c = this->getChildComponent(i);
        c->setFocusContainer(false);
        c->setWantsKeyboardFocus(false);
        c->setMouseClickGrabsKeyboardFocus(false);
    }

    this->project.getTransport().addTransportListener(this);
    ArpeggiatorsManager::getInstance().addChangeListener(this);
    //[/Constructor]
}

ArpeggiatorEditorPanel::~ArpeggiatorEditorPanel()
{
    //[Destructor_pre]
    this->project.getTransport().stopPlayback();

    // warning: crashes if not dismissed properly on app close
    ArpeggiatorsManager::getInstance().removeChangeListener(this);
    this->project.getTransport().removeTransportListener(this);
    //[/Destructor_pre]

    bg = nullptr;
    editor = nullptr;
    resetButton = nullptr;
    playButton = nullptr;
    shadowDown = nullptr;
    s2 = nullptr;
    reverseToggleButton = nullptr;
    arpsList = nullptr;
    relativeMappingToggleButton = nullptr;
    limitToChordToggleButton = nullptr;
    s1 = nullptr;
    label = nullptr;
    scaleList = nullptr;
    addArpButton = nullptr;
    s3 = nullptr;
    nameLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ArpeggiatorEditorPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ArpeggiatorEditorPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    editor->setBounds (8, 120, getWidth() - 18, 120);
    resetButton->setBounds (109 - (58 / 2), 417 - (58 / 2), 58, 58);
    playButton->setBounds (184 - (64 / 2), 418 - (64 / 2), 64, 64);
    shadowDown->setBounds (0, 384, getWidth() - 0, 40);
    s2->setBounds (8, 296, 280, 8);
    reverseToggleButton->setBounds (16, 304, 264, 24);
    arpsList->setBounds (8, 8, 280, 24);
    relativeMappingToggleButton->setBounds (16, 328, 272, 24);
    limitToChordToggleButton->setBounds (32, 352, 256, 24);
    s1->setBounds (8, 252, 280, 8);
    label->setBounds (16, 264, 48, 24);
    scaleList->setBounds (72, 264, 216, 24);
    addArpButton->setBounds (8, 80, 280, 24);
    s3->setBounds (8, 40, 280, 8);
    nameLabel->setBounds (16, 48, 264, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ArpeggiatorEditorPanel::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == reverseToggleButton)
    {
        //[UserButtonCode_reverseToggleButton] -- add your button handler code here..
        this->updateCurrentArp();
        this->syncDataToManager();
        //[/UserButtonCode_reverseToggleButton]
    }
    else if (buttonThatWasClicked == relativeMappingToggleButton)
    {
        //[UserButtonCode_relativeMappingToggleButton] -- add your button handler code here..
        this->updateCurrentArp();
        this->syncDataToManager();
        //[/UserButtonCode_relativeMappingToggleButton]
    }
    else if (buttonThatWasClicked == limitToChordToggleButton)
    {
        //[UserButtonCode_limitToChordToggleButton] -- add your button handler code here..
        this->updateCurrentArp();
        this->syncDataToManager();
        //[/UserButtonCode_limitToChordToggleButton]
    }
    else if (buttonThatWasClicked == addArpButton)
    {
        //[UserButtonCode_addArpButton] -- add your button handler code here..
        this->updateCurrentArp();

        ScopedPointer<XmlElement> xml(XmlDocument::parse(this->document.getAllContent()));
        this->currentArp = this->currentArp.withSequenceFromXml(*xml);

        ArpeggiatorsManager::getInstance().addArp(this->currentArp);
        //[/UserButtonCode_addArpButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void ArpeggiatorEditorPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == arpsList)
    {
        //[UserComboBoxCode_arpsList] -- add your combo box handling code here..
        const int index = arpsList->getSelectedItemIndex();
        const Array<Arpeggiator> arps = ArpeggiatorsManager::getInstance().getArps();
        this->currentArp = arps[index];
        this->updateControls(this->currentArp);
        this->applyArpToSelectedNotes(this->currentArp);
        //[/UserComboBoxCode_arpsList]
    }
    else if (comboBoxThatHasChanged == scaleList)
    {
        //[UserComboBoxCode_scaleList] -- add your combo box handling code here..
        this->updateCurrentArp();
        this->syncDataToManager();
        //[/UserComboBoxCode_scaleList]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void ArpeggiatorEditorPanel::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == nameLabel)
    {
        //[UserLabelCode_nameLabel] -- add your label text handling code here..
        this->updateCurrentArp();
        this->syncDataToManager();
        //[/UserLabelCode_nameLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void ArpeggiatorEditorPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::ResetArpeggiatorChanges)
    {
        if (this->hasMadeChanges)
        {
            this->roll.getPrimaryActiveMidiLayer()->undo();
            this->sendChangeMessage();
            this->hasMadeChanges = false;
        }
    }
    else if (commandId == CommandIDs::TransportStartPlayback)
    {
        const float startBeat = PianoRollToolbox::findStartBeat(this->roll.getLassoSelection());
        const float endBeat = PianoRollToolbox::findEndBeat(this->roll.getLassoSelection());

        const double loopStart = this->roll.getTransportPositionByBeat(startBeat);
        const double loopEnd = this->roll.getTransportPositionByBeat(endBeat);

        this->project.getTransport().startPlaybackLooped(loopStart, loopEnd);
    }
    else if (commandId == CommandIDs::TransportPausePlayback)
    {
        this->project.getTransport().stopPlayback();
    }

    //[/UserCode_handleCommandMessage]
}

bool ArpeggiatorEditorPanel::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    return true;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// TransportListener
//

void ArpeggiatorEditorPanel::onSeek(const double newPosition,
                              const double currentTimeMs,
                              const double totalTimeMs)
{
}

void ArpeggiatorEditorPanel::onTempoChanged(const double newTempo)
{
}

void ArpeggiatorEditorPanel::onTotalTimeChanged(const double timeMs)
{
}

void ArpeggiatorEditorPanel::onPlay()
{
    this->playButton->setPlaying(true);
}

void ArpeggiatorEditorPanel::onStop()
{
    this->playButton->setPlaying(false);
}


//===----------------------------------------------------------------------===//
// ChangeListener
//

void ArpeggiatorEditorPanel::changeListenerCallback(ChangeBroadcaster *source)
{
    this->reloadData();
    this->applyArpToSelectedNotes(this->currentArp);
}

void ArpeggiatorEditorPanel::reloadData()
{
    const int index = arpsList->getSelectedItemIndex();

    this->arpsList->clear();
    const Array<Arpeggiator> arps = ArpeggiatorsManager::getInstance().getArps();

    for (int i = 0; i < arps.size(); ++i)
    {
        //Logger::writeToLog(">> " + arps.getUnchecked(i).getName());
        this->arpsList->addItem(arps.getUnchecked(i).getName(), i + 1);
    }

    this->currentArp = arps[index];
    this->arpsList->setSelectedItemIndex(index);
    this->updateControls(this->currentArp);
}

void ArpeggiatorEditorPanel::syncDataToManager()
{
    const bool syncOk = ArpeggiatorsManager::getInstance().replaceArpWithId(this->currentArp.getId(), this->currentArp);

    if (!syncOk)
    {
        this->applyArpToSelectedNotes(this->currentArp);
    }
}

void ArpeggiatorEditorPanel::updateCurrentArp()
{
    const float scale = this->scaleList->getText().isEmpty() ? 1.f : this->scaleList->getText().getFloatValue();

    this->currentArp = this->currentArp.
        reversed(this->reverseToggleButton->getToggleState()).
        mappedRelative(this->relativeMappingToggleButton->getToggleState()).
        limitedToChord(this->limitToChordToggleButton->getToggleState()).
        withName(this->nameLabel->getText()).
        withScale(scale);
}

void ArpeggiatorEditorPanel::updateControls(const Arpeggiator &arp)
{
    this->reverseToggleButton->setToggleState(arp.isReversed(), dontSendNotification);
    this->relativeMappingToggleButton->setToggleState(arp.hasRelativeMapping(), dontSendNotification);
    this->limitToChordToggleButton->setToggleState(arp.limitsToChord(), dontSendNotification);

    this->scaleList->setText(String(arp.getScale(), dontSendNotification));
    this->nameLabel->setText(arp.getName(), dontSendNotification);
    this->document.replaceAllContent(arp.exportSequenceAsTrack());
}

void ArpeggiatorEditorPanel::applyArpToSelectedNotes(const Arpeggiator &arp)
{
    if (this->roll.getLassoSelection().getNumSelected() == 0)
    {
        return;
    }

    if (this->roll.getLassoSelection().getNumSelected() > 20)
    {
        this->roll.setVisible(false);
    }

    if (this->hasMadeChanges)
    {
        this->roll.getPrimaryActiveMidiLayer()->undo();
    }

    if (bool arpedOk = PianoRollToolbox::arpeggiate(this->roll.getLassoSelection(), arp))
    {
        this->hasMadeChanges = true;
    }

    if (! this->roll.isVisible())
    {
        this->roll.setVisible(true);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ArpeggiatorEditorPanel" template="../../Template"
                 componentName="" parentClasses="public Component, public ChangeBroadcaster, private TransportListener, private ChangeListener"
                 constructorParams="ProjectTreeItem &amp;parentProject, PianoRoll &amp;targetRoll"
                 variableInitialisers="roll(targetRoll),&#10;project(parentProject),&#10;hasMadeChanges(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="300" initialHeight="450">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="274b1411c9ae170b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="808594cf08a73350" memberName="editor" virtualName=""
                    explicitFocusOrder="0" pos="8 120 18M 120" class="CodeEditorComponent"
                    params="this-&gt;document, &amp;this-&gt;tokeniser"/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="resetButton" virtualName=""
                    explicitFocusOrder="0" pos="109c 417c 58 58" class="CommandItemComponent"
                    params="this, nullptr, CommandItem::withParams(Icons::reset, CommandIDs::ResetArpeggiatorChanges)"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="184c 418c 64 64" sourceFile="../Common/PlayButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="e71c535b988926e8" memberName="shadowDown" virtualName=""
             explicitFocusOrder="0" pos="0 384 0M 40" sourceFile="../Themes/ShadowDownwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="234394c333186033" memberName="s2" virtualName=""
             explicitFocusOrder="0" pos="8 296 280 8" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <TOGGLEBUTTON name="" id="6fb9676315255c3e" memberName="reverseToggleButton"
                virtualName="" explicitFocusOrder="0" pos="16 304 264 24" buttonText="Should be reversed"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <COMBOBOX name="" id="f69a67273172e51e" memberName="arpsList" virtualName=""
            explicitFocusOrder="0" pos="8 8 280 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="" id="ca4859caa5ba4851" memberName="relativeMappingToggleButton"
                virtualName="" explicitFocusOrder="0" pos="16 328 272 24" buttonText="Arp seqence as is (don't map to chord)"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <TOGGLEBUTTON name="" id="9005a36ba93f97e1" memberName="limitToChordToggleButton"
                virtualName="" explicitFocusOrder="0" pos="32 352 256 24" buttonText="Every chord starts from 0"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <JUCERCOMP name="" id="8910718a24db2d86" memberName="s1" virtualName=""
             explicitFocusOrder="0" pos="8 252 280 8" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <LABEL name="" id="e767a45eea00a91b" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="16 264 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Scale:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="" id="662752ccbefedf0b" memberName="scaleList" virtualName=""
            explicitFocusOrder="0" pos="72 264 216 24" editable="0" layout="33"
            items="0.25&#10;0.5&#10;0.66&#10;0.75&#10;1.0&#10;1.5&#10;2.0&#10;2.5&#10;3.0"
            textWhenNonSelected="1.0" textWhenNoItems="1.0"/>
  <TEXTBUTTON name="" id="6a904824ca619531" memberName="addArpButton" virtualName=""
              explicitFocusOrder="0" pos="8 80 280 24" buttonText="Create new"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e6595e9968eeb574" memberName="s3" virtualName=""
             explicitFocusOrder="0" pos="8 40 280 8" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <LABEL name="" id="cddce1bf4458c4c9" memberName="nameLabel" virtualName=""
         explicitFocusOrder="0" pos="16 48 264 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="Name" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

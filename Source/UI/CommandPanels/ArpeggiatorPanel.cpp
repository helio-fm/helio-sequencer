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

#include "ArpeggiatorPanel.h"

//[MiscUserDefs]
#include "PianoRoll.h"
#include "Lasso.h"
#include "PianoRollToolbox.h"
#include "MidiLayer.h"
#include "NoteComponent.h"
#include "Transport.h"
#include "CommandItemComponent.h"
#include "CommandPanel.h"
#include "PianoRollToolbox.h"
#include "SerializationKeys.h"
#include "ArpeggiatorsManager.h"
//[/MiscUserDefs]

ArpeggiatorPanel::ArpeggiatorPanel(Transport &targetTransport, PianoRoll &targetRoll)
    : roll(targetRoll),
      transport(targetTransport),
      hasMadeChanges(false),
      lastSelectedArpIndex(-1)
{
    addAndMakeVisible (bg = new PanelBackgroundC());
    addAndMakeVisible (listBox = new ListBox());

    addAndMakeVisible (resetButton = new CommandItemComponent (this, nullptr, CommandItem::withParams(Icons::reset, CommandIDs::ResetArpeggiatorChanges)));

    addAndMakeVisible (playButton = new PlayButton());
    addAndMakeVisible (shadowDown = new ShadowDownwards());

    //[UserPreSize]
    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->listBox->setRowHeight(COMMAND_PANEL_BUTTON_HEIGHT);

    this->reloadList();
    this->grabKeyboardFocus();
    //[/UserPreSize]

    setSize (250, 350);

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

    this->transport.addTransportListener(this);
    ArpeggiatorsManager::getInstance().addChangeListener(this);
    //[/Constructor]
}

ArpeggiatorPanel::~ArpeggiatorPanel()
{
    //[Destructor_pre]
    this->stopPlaybackLoop();

    // warning: crashes if not dismissed properly on app close
    ArpeggiatorsManager::getInstance().removeChangeListener(this);
    this->transport.removeTransportListener(this);
    //[/Destructor_pre]

    bg = nullptr;
    listBox = nullptr;
    resetButton = nullptr;
    playButton = nullptr;
    shadowDown = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ArpeggiatorPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ArpeggiatorPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    listBox->setBounds (0, 0, getWidth() - 0, getHeight() - 63);
    resetButton->setBounds (85 - (58 / 2), 317 - (58 / 2), 58, 58);
    playButton->setBounds (160 - (64 / 2), 318 - (64 / 2), 64, 64);
    shadowDown->setBounds (0, 284, getWidth() - 0, 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ArpeggiatorPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::ResetArpeggiatorChanges)
    {
        if (this->hasMadeChanges)
        {
            this->stopPlaybackLoop();

            if (this->roll.getLassoSelection().getNumSelected() > 20)
            { this->roll.setVisible(false); }

            this->roll.getPrimaryActiveMidiLayer()->undo();
            this->sendChangeMessage();
            this->hasMadeChanges = false;
            this->lastSelectedArpIndex = -1;

            this->reloadList();

            if (! this->roll.isVisible())
            { this->roll.setVisible(true); }
        }
    }
    else if (commandId == CommandIDs::TransportStartPlayback)
    {
        this->startPlaybackLoop();
    }
    else if (commandId == CommandIDs::TransportPausePlayback)
    {
        this->stopPlaybackLoop();
    }
    else
    {
        const int arpIndex = commandId - CommandIDs::ApplyArpeggiator;
        const Array<Arpeggiator> arps = ArpeggiatorsManager::getInstance().getArps();

        if (arpIndex >= 0 &&
            arpIndex < arps.size() &&
            arpIndex != this->lastSelectedArpIndex)
        {
            this->reloadList(arpIndex);
        }

        this->lastSelectedArpIndex = arpIndex;

        this->startPlaybackLoop();
    }

    //[/UserCode_handleCommandMessage]
}

bool ArpeggiatorPanel::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    return true;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}


//[MiscUserCode]

void ArpeggiatorPanel::startPlaybackLoop() const
{
    const float startBeat = PianoRollToolbox::findStartBeat(this->roll.getLassoSelection());
    const float endBeat = PianoRollToolbox::findEndBeat(this->roll.getLassoSelection());

    const float deltaCorrection = 0.01f;

    const double loopStart = this->roll.getTransportPositionByBeat(startBeat - deltaCorrection);
    const double loopEnd = this->roll.getTransportPositionByBeat(endBeat + deltaCorrection);

    this->transport.startPlaybackLooped(loopStart, loopEnd);
}

void ArpeggiatorPanel::stopPlaybackLoop() const
{
    this->transport.stopPlayback();
}


//===----------------------------------------------------------------------===//
// TransportListener
//

void ArpeggiatorPanel::onSeek(const double newPosition,
                              const double currentTimeMs,
                              const double totalTimeMs)
{
}

void ArpeggiatorPanel::onTempoChanged(const double newTempo)
{
}

void ArpeggiatorPanel::onTotalTimeChanged(const double timeMs)
{
}

void ArpeggiatorPanel::onPlay()
{
    this->playButton->setPlaying(true);
}

void ArpeggiatorPanel::onStop()
{
    this->playButton->setPlaying(false);
}


//===----------------------------------------------------------------------===//
// ChangeListener
//

void ArpeggiatorPanel::changeListenerCallback(ChangeBroadcaster *source)
{
    this->reloadList();
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//

int ArpeggiatorPanel::getNumRows()
{
    return this->commandDescriptions.size();
}

void ArpeggiatorPanel::paintListBoxItem(int rowNumber,
                                    Graphics &g,
                                    int width, int height,
                                    bool rowIsSelected)
{
    //
}

Component *ArpeggiatorPanel::refreshComponentForRow(int rowNumber, bool isRowSelected,
                                                Component *existingComponentToUpdate)
{
    if (rowNumber >= this->commandDescriptions.size())
    {
        return existingComponentToUpdate;
    }

    const CommandItem::Ptr itemDescription = this->commandDescriptions[rowNumber];

    if (existingComponentToUpdate != nullptr)
    {
        if (CommandItemComponent *row = dynamic_cast<CommandItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(itemDescription);
        }
    }
    else
    {
        CommandItemComponent *row = new CommandItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

void ArpeggiatorPanel::listWasScrolled()
{
    //
}


//===----------------------------------------------------------------------===//
// Private
//

void ArpeggiatorPanel::reloadList(int selectedRow)
{
    this->commandDescriptions.clear();
    const Array<Arpeggiator> arps = ArpeggiatorsManager::getInstance().getArps();

    for (int i = 0; i < arps.size(); ++i)
    {
        const bool isSelectedArp = (i == selectedRow);

        this->commandDescriptions.add(CommandItem::withParams(Icons::group,
                                                              (CommandIDs::ApplyArpeggiator + i),
                                                              arps.getUnchecked(i).getName())->toggled(isSelectedArp));

        if (isSelectedArp)
        {
            this->applyArpToSelectedNotes(arps.getUnchecked(i));
        }
    }

    this->listBox->updateContent();
}

void ArpeggiatorPanel::applyArpToSelectedNotes(const Arpeggiator &arp)
{
    if (this->hasMadeChanges)
    {
        this->roll.setVisible(false);
        this->roll.getPrimaryActiveMidiLayer()->undo();
        this->roll.setVisible(true);
    }

    if (this->roll.getLassoSelection().getNumSelected() > 20)
    { this->roll.setVisible(false); }

    PianoRollToolbox::arpeggiate(this->roll.getLassoSelection(), arp);

    this->hasMadeChanges = true;

    if (! this->roll.isVisible())
    { this->roll.setVisible(true); }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ArpeggiatorPanel" template="../../Template"
                 componentName="" parentClasses="public Component, public ChangeBroadcaster, private TransportListener, private ChangeListener, private ListBoxModel"
                 constructorParams="Transport &amp;targetTransport, PianoRoll &amp;targetRoll"
                 variableInitialisers="roll(targetRoll),&#10;transport(targetTransport),&#10;hasMadeChanges(false),&#10;lastSelectedArpIndex(-1)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="250" initialHeight="350">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="274b1411c9ae170b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="808594cf08a73350" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 63M" class="ListBox" params=""/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="resetButton" virtualName=""
                    explicitFocusOrder="0" pos="85c 317c 58 58" class="CommandItemComponent"
                    params="this, nullptr, CommandItem::withParams(Icons::reset, CommandIDs::ResetArpeggiatorChanges)"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="160c 318c 64 64" sourceFile="../Common/PlayButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="shadowDown" id="e71c535b988926e8" memberName="shadowDown"
             virtualName="" explicitFocusOrder="0" pos="0 284 0M 20" sourceFile="../Themes/ShadowDownwards.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "ArpTuningPanel.h"

//[MiscUserDefs]
#include "MenuPanel.h"
#include "PianoRoll.h"
#include "Lasso.h"
#include "SequencerOperations.h"
#include "MidiTrack.h"
#include "MidiSequence.h"
#include "NoteComponent.h"
#include "ProjectNode.h"
#include "Transport.h"
#include "CommandIDs.h"
#include "MenuItemComponent.h"
#include "PopupButton.h"
//[/MiscUserDefs]

ArpTuningPanel::ArpTuningPanel(ProjectNode &parentProject, PianoRoll &targetRoll)
    : roll(targetRoll),
      project(parentProject),
      hasMadeChanges(false)
{
    this->bg.reset(new PanelBackgroundC());
    this->addAndMakeVisible(bg.get());
    this->sliderLinearButton.reset(new PopupButton(false));
    this->addAndMakeVisible(sliderLinearButton.get());

    this->sliderSineButton.reset(new PopupButton(false));
    this->addAndMakeVisible(sliderSineButton.get());

    this->volumeMultiplier.reset(new Slider(String()));
    this->addAndMakeVisible(volumeMultiplier.get());
    volumeMultiplier->setRange (0, 10, 1);
    volumeMultiplier->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    volumeMultiplier->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    volumeMultiplier->addListener (this);

    this->randomMultiplier.reset(new Slider(String()));
    this->addAndMakeVisible(randomMultiplier.get());
    randomMultiplier->setRange (0, 10, 1);
    randomMultiplier->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    randomMultiplier->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    randomMultiplier->addListener (this);

    this->sliderMultiplyButton.reset(new PopupButton(false));
    this->addAndMakeVisible(sliderMultiplyButton.get());

    this->speedMultiplier.reset(new Slider(String()));
    this->addAndMakeVisible(speedMultiplier.get());
    speedMultiplier->setRange (0, 10, 1);
    speedMultiplier->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    speedMultiplier->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    speedMultiplier->addListener (this);

    this->arpsList.reset(new MenuPanel());
    this->addAndMakeVisible(arpsList.get());

    this->resetButton.reset(new MenuItemComponent(this, nullptr, MenuItem::item(Icons::reset, CommandIDs::ResetPreviewChanges)));
    this->addAndMakeVisible(resetButton.get());

    this->playButton.reset(new PlayButton(nullptr));
    this->addAndMakeVisible(playButton.get());

    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);

    //this->volumeMultiplier->setValue(5.0, dontSendNotification);
    //this->speedMultiplier->setValue(5.0, dontSendNotification);
    //this->randomMultiplier->setValue(5.0, dontSendNotification);
    //[/UserPreSize]

    this->setSize(250, 344);

    //[Constructor]
    this->project.getTransport().addTransportListener(this);
    //[/Constructor]
}

ArpTuningPanel::~ArpTuningPanel()
{
    //[Destructor_pre]
    this->project.getTransport().removeTransportListener(this);
    //[/Destructor_pre]

    bg = nullptr;
    sliderLinearButton = nullptr;
    sliderSineButton = nullptr;
    volumeMultiplier = nullptr;
    randomMultiplier = nullptr;
    sliderMultiplyButton = nullptr;
    speedMultiplier = nullptr;
    arpsList = nullptr;
    resetButton = nullptr;
    playButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ArpTuningPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ArpTuningPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    sliderLinearButton->setBounds((46 - (56 / 2)) + 56 / 2 - (56 / 2), 238 + 56 / 2 - (56 / 2), 56, 56);
    sliderSineButton->setBounds((206 - (56 / 2)) + 56 / 2 - (56 / 2), 238 + 56 / 2 - (56 / 2), 56, 56);
    volumeMultiplier->setBounds(46 - (56 / 2), 238, 56, 56);
    randomMultiplier->setBounds(206 - (56 / 2), 238, 56, 56);
    sliderMultiplyButton->setBounds((126 - (56 / 2)) + 56 / 2 - (56 / 2), 238 + 56 / 2 - (56 / 2), 56, 56);
    speedMultiplier->setBounds(126 - (56 / 2), 238, 56, 56);
    arpsList->setBounds(8, 8, getWidth() - 16, 224);
    resetButton->setBounds((getWidth() / 2) + -16 - 48, 317 - (48 / 2), 48, 48);
    playButton->setBounds((getWidth() / 2) + 16, 318 - (48 / 2), 48, 48);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ArpTuningPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == volumeMultiplier.get())
    {
        //[UserSliderCode_volumeMultiplier] -- add your slider handling code here..
        //[/UserSliderCode_volumeMultiplier]
    }
    else if (sliderThatWasMoved == randomMultiplier.get())
    {
        //[UserSliderCode_randomMultiplier] -- add your slider handling code here..
        //[/UserSliderCode_randomMultiplier]
    }
    else if (sliderThatWasMoved == speedMultiplier.get())
    {
        //[UserSliderCode_speedMultiplier] -- add your slider handling code here..
        //[/UserSliderCode_speedMultiplier]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void ArpTuningPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::ResetPreviewChanges)
    {
        this->undoIfNeeded();
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void ArpsPanel::undoIfNeeded()
{
    if (this->hasMadeChanges)
    {
        this->project.undo();
        this->syncSliders();
        this->sendChangeMessage();
        this->hasMadeChanges = false;
    }
}


//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void ArpsPanel::onTempoChanged(double) {}
void ArpsPanel::onTotalTimeChanged(double) {}
void ArpsPanel::onSeek(double, double, double) {}

void ArpsPanel::onPlay()
{
    this->playButton->setPlaying(true);
}

void ArpsPanel::onStop()
{
    this->playButton->setPlaying(false);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ArpTuningPanel" template="../../Template"
                 componentName="" parentClasses="public Component, public ChangeBroadcaster, private TransportListener"
                 constructorParams="ProjectNode &amp;parentProject, PianoRoll &amp;targetRoll"
                 variableInitialisers="roll(targetRoll),&#10;project(parentProject),&#10;hasMadeChanges(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="250" initialHeight="344">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="274b1411c9ae170b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="a4c6c295088640a8" memberName="sliderLinearButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0Cc 56 56" posRelativeX="612822c144ea1163"
                    posRelativeY="612822c144ea1163" class="PopupButton" params="false"/>
  <GENERICCOMPONENT name="" id="922ef78567538854" memberName="sliderSineButton" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 56 56" posRelativeX="bdc5e7b689607511"
                    posRelativeY="bdc5e7b689607511" class="PopupButton" params="false"/>
  <SLIDER name="" id="612822c144ea1163" memberName="volumeMultiplier" virtualName=""
          explicitFocusOrder="0" pos="46c 238 56 56" posRelativeX="901299ec4e766469"
          posRelativeY="901299ec4e766469" min="0.0" max="10.0" int="1.0"
          style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="0" textBoxHeight="0" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="" id="bdc5e7b689607511" memberName="randomMultiplier" virtualName=""
          explicitFocusOrder="0" pos="206c 238 56 56" posRelativeX="901299ec4e766469"
          posRelativeY="901299ec4e766469" min="0.0" max="10.0" int="1.0"
          style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="0" textBoxHeight="0" skewFactor="1.0" needsCallback="1"/>
  <GENERICCOMPONENT name="" id="fd2a7dfd7daba8e3" memberName="sliderMultiplyButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0Cc 56 56" posRelativeX="645d4540b1ee1178"
                    posRelativeY="645d4540b1ee1178" class="PopupButton" params="false"/>
  <SLIDER name="" id="645d4540b1ee1178" memberName="speedMultiplier" virtualName=""
          explicitFocusOrder="0" pos="126c 238 56 56" min="0.0" max="10.0"
          int="1.0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="0" textBoxHeight="0" skewFactor="1.0"
          needsCallback="1"/>
  <GENERICCOMPONENT name="" id="808594cf08a73350" memberName="arpsList" virtualName=""
                    explicitFocusOrder="0" pos="8 8 16M 224" class="MenuPanel" params=""/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="resetButton" virtualName=""
                    explicitFocusOrder="0" pos="-16Cr 317c 48 48" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::reset, CommandIDs::ResetPreviewChanges)"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="16C 318c 48 48" sourceFile="../Common/PlayButton.cpp"
             constructorParams="nullptr"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif




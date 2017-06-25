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

#include "NotesTuningPanel.h"

//[MiscUserDefs]
#include "PianoRoll.h"
#include "Lasso.h"
#include "PianoRollToolbox.h"
#include "MidiLayer.h"
#include "NoteComponent.h"
#include "ProjectTreeItem.h"
#include "Transport.h"
#include "CommandIDs.h"
#include "CommandItemComponent.h"

class NotesTuningDiagram : public Component, private ChangeListener
{
public:

    NotesTuningDiagram(NotesTuningPanel *parentPanel,
                       const Lasso &targetSelection) :
        parent(parentPanel),
        selection(targetSelection)
    {
        this->parent->addChangeListener(this);
    }

    ~NotesTuningDiagram() override
    {
        this->parent->removeChangeListener(this);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::white);
        g.fillPath(this->path);
    }

    void resized() override
    {
        this->updateDiagram();
    }

private:

    void changeListenerCallback(ChangeBroadcaster *source) override
    {
        this->updateDiagram();
        this->repaint();
    }

    void updateDiagram()
    {
        if (this->selection.getNumSelected() == 0)
        {
            return;
        }

        const float startBeat = PianoRollToolbox::findStartBeat(this->selection);
        const float endBeat = PianoRollToolbox::findEndBeat(this->selection);

        Array<MidiEventComponent *> sortedSelection;

        for (int i = 0; i < this->selection.getNumSelected(); ++i)
        {
            MidiEventComponent *mc = static_cast<MidiEventComponent *>(this->selection.getSelectedItem(i));
            sortedSelection.addSorted(*mc, mc);
        }

        const float h = float(this->getHeight());

        this->path.clear();
        this->path.startNewSubPath(0.f, h);

        for (auto i : sortedSelection)
        {
            if (NoteComponent *nc = dynamic_cast<NoteComponent *>(i))
            {
                const float xAbsPosition = (nc->getNote().getBeat() - startBeat) / (endBeat - startBeat);
                const int x = this->proportionOfWidth(xAbsPosition);
                const int y = this->proportionOfHeight(1.f - nc->getNote().getVelocity());

                //Logger::writeToLog(String(x) + " : " + String(y));
                //this->path.quadraticTo(x, y, x, y);
                this->path.lineTo(float(x), h);
                this->path.lineTo(float(x), float(y));
                this->path.lineTo(float(x + 1.5f), float(y));
                this->path.lineTo(float(x + 1.5f), h);
            }
        }

        this->path.lineTo(float(this->getWidth()), h);
        this->path.closeSubPath();

//        this->path.setUsingNonZeroWinding(false);
//        PathStrokeType stroke(1, PathStrokeType::beveled, PathStrokeType::butt);
//        stroke.createStrokedPath(this->path, this->path);
    }

private:

    Path path;

    SafePointer<NotesTuningPanel> parent;

    const Lasso &selection;

};

//[/MiscUserDefs]

NotesTuningPanel::NotesTuningPanel(ProjectTreeItem &parentProject, PianoRoll &targetRoll)
    : roll(targetRoll),
      project(parentProject),
      hasMadeChanges(false)
{
    addAndMakeVisible (bg = new PanelBackgroundC());
    addAndMakeVisible (sliderLinearButton = new PopupButton (false));
    addAndMakeVisible (sliderMultiplyButton = new PopupButton (false));
    addAndMakeVisible (volumeSliderMulti = new Slider (String()));
    volumeSliderMulti->setRange (0, 10, 0);
    volumeSliderMulti->setSliderStyle (Slider::RotaryHorizontalDrag);
    volumeSliderMulti->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    volumeSliderMulti->addListener (this);

    addAndMakeVisible (volumeSliderLinear = new Slider (String()));
    volumeSliderLinear->setRange (0, 10, 0);
    volumeSliderLinear->setSliderStyle (Slider::RotaryHorizontalDrag);
    volumeSliderLinear->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    volumeSliderLinear->setColour (Slider::textBoxTextColourId, Colour (0x00000000));
    volumeSliderLinear->addListener (this);

    addAndMakeVisible (tuningDiagram = new NotesTuningDiagram (this, this->roll.getLassoSelection()));

    addAndMakeVisible (panel = new PanelA());
    addAndMakeVisible (resetButton = new CommandItemComponent (this, nullptr, CommandItem::withParams(Icons::reset, CommandIDs::ResetVolumeChanges)));

    addAndMakeVisible (playButton = new PlayButton());
    addAndMakeVisible (shadowDown = new ShadowDownwards());
    addAndMakeVisible (sliderSineButton = new PopupButton (false));
    addAndMakeVisible (sineSlider = new Slider (String()));
    sineSlider->setRange (0, 10, 0);
    sineSlider->setSliderStyle (Slider::RotaryHorizontalDrag);
    sineSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    sineSlider->setColour (Slider::textBoxTextColourId, Colour (0x00000000));
    sineSlider->addListener (this);

    addAndMakeVisible (linearLabel = new Label (String(),
                                                TRANS("+")));
    linearLabel->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    linearLabel->setJustificationType (Justification::centred);
    linearLabel->setEditable (false, false, false);
    linearLabel->setColour (Label::textColourId, Colour (0x3fffffff));
    linearLabel->setColour (TextEditor::textColourId, Colours::black);
    linearLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (multiLabel = new Label (String(),
                                               TRANS("*")));
    multiLabel->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    multiLabel->setJustificationType (Justification::centred);
    multiLabel->setEditable (false, false, false);
    multiLabel->setColour (Label::textColourId, Colour (0x3fffffff));
    multiLabel->setColour (TextEditor::textColourId, Colours::black);
    multiLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (sineLabel = new Label (String(),
                                              TRANS("~")));
    sineLabel->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    sineLabel->setJustificationType (Justification::centred);
    sineLabel->setEditable (false, false, false);
    sineLabel->setColour (Label::textColourId, Colour (0x3fffffff));
    sineLabel->setColour (TextEditor::textColourId, Colours::black);
    sineLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]

    this->grabKeyboardFocus();
//    this->volumeSliderLinear->setVelocityBasedMode(true);
//    this->volumeSliderMulti->setVelocityBasedMode(true);

    this->syncSliders();

    //[/UserPreSize]

    setSize (250, 240);

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
    //[/Constructor]
}

NotesTuningPanel::~NotesTuningPanel()
{
    //[Destructor_pre]
    this->project.getTransport().stopPlayback();

    // warning: crashes if not dismissed properly on app close
    this->project.getTransport().removeTransportListener(this);
    //[/Destructor_pre]

    bg = nullptr;
    sliderLinearButton = nullptr;
    sliderMultiplyButton = nullptr;
    volumeSliderMulti = nullptr;
    volumeSliderLinear = nullptr;
    tuningDiagram = nullptr;
    panel = nullptr;
    resetButton = nullptr;
    playButton = nullptr;
    shadowDown = nullptr;
    sliderSineButton = nullptr;
    sineSlider = nullptr;
    linearLabel = nullptr;
    multiLabel = nullptr;
    sineLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void NotesTuningPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void NotesTuningPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    sliderLinearButton->setBounds (46 - (64 / 2), 56 - (64 / 2), 64, 64);
    sliderMultiplyButton->setBounds (126 - (64 / 2), 56 - (64 / 2), 64, 64);
    volumeSliderMulti->setBounds (126 - (64 / 2), 56 - (64 / 2), 64, 64);
    volumeSliderLinear->setBounds (46 - (64 / 2), 56 - (64 / 2), 64, 64);
    tuningDiagram->setBounds (0, 96, getWidth() - 0, 88);
    panel->setBounds (0, 96, getWidth() - 0, 88);
    resetButton->setBounds (85 - (58 / 2), 213 - (58 / 2), 58, 58);
    playButton->setBounds (160 - (64 / 2), 214 - (64 / 2), 64, 64);
    shadowDown->setBounds (0, 180, getWidth() - 0, 24);
    sliderSineButton->setBounds (206 - (64 / 2), 56 - (64 / 2), 64, 64);
    sineSlider->setBounds (206 - (64 / 2), 56 - (64 / 2), 64, 64);
    linearLabel->setBounds (18, 0, 56, 24);
    multiLabel->setBounds (98, 5, 56, 24);
    sineLabel->setBounds (178, 0, 56, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void NotesTuningPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == volumeSliderMulti)
    {
        //[UserSliderCode_volumeSliderMulti] -- add your slider handling code here..
        this->syncVolumeMultiplied(sliderThatWasMoved);
        this->sendChangeMessage();
        //[/UserSliderCode_volumeSliderMulti]
    }
    else if (sliderThatWasMoved == volumeSliderLinear)
    {
        //[UserSliderCode_volumeSliderLinear] -- add your slider handling code here..
        this->syncVolumeLinear(sliderThatWasMoved);
        this->sendChangeMessage();
        //[/UserSliderCode_volumeSliderLinear]
    }
    else if (sliderThatWasMoved == sineSlider)
    {
        //[UserSliderCode_sineSlider] -- add your slider handling code here..
        this->syncVolumeSine(sliderThatWasMoved);
        this->sendChangeMessage();
        //[/UserSliderCode_sineSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void NotesTuningPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::ResetVolumeChanges)
    {
        if (this->hasMadeChanges)
        {
            this->roll.getPrimaryActiveMidiLayer()->undo();
            this->syncSliders();
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

bool NotesTuningPanel::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    return true;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}


//[MiscUserCode]

void NotesTuningPanel::sliderDragStarted(Slider *slider)
{
    if (slider == this->volumeSliderMulti)
    {
        this->volumeAnchorMulti = float(this->volumeSliderMulti->getValue());
    }
    else if (slider == this->volumeSliderLinear)
    {
        this->volumeAnchorLinear = float(this->volumeSliderLinear->getValue());
    }
    else if (slider == this->sineSlider)
    {
        this->volumeAnchorSine = float(this->sineSlider->getValue());
    }

    this->startTuning();
}

void NotesTuningPanel::sliderDragEnded(Slider *slider)
{
    this->endTuning();
}

void NotesTuningPanel::syncVolumeLinear(Slider *volumeSlider)
{
    if (! this->hasMadeChanges)
    {
        this->roll.getPrimaryActiveMidiLayer()->checkpoint();
        this->hasMadeChanges = true;
    }

    const float volumeDelta = this->volumeAnchorLinear - float(volumeSlider->getValue());
    Lasso &selection = this->roll.getLassoSelection();
    PianoRollToolbox::changeVolumeLinear(selection, volumeDelta);
}

void NotesTuningPanel::syncVolumeMultiplied(Slider *volumeSlider)
{
    if (! this->hasMadeChanges)
    {
        this->roll.getPrimaryActiveMidiLayer()->checkpoint();
        this->hasMadeChanges = true;
    }

    // 0 .. volumeAnchor    ->  -1 .. 0
    // volumeAnchor .. 1.0  ->   0 .. 1
    const float volume = float(volumeSlider->getValue());
    const float anchor = this->volumeAnchorMulti;
    const float volumeFactor = (volume < anchor) ? ((volume / anchor) - 1.f) : ((volume - anchor) / (1.f - anchor));

    Lasso &selection = this->roll.getLassoSelection();
    PianoRollToolbox::changeVolumeMultiplied(selection, volumeFactor);
}

void NotesTuningPanel::syncVolumeSine(Slider *volumeSlider)
{
    if (! this->hasMadeChanges)
    {
        this->roll.getPrimaryActiveMidiLayer()->checkpoint();
        this->hasMadeChanges = true;
    }

    // 0 .. sineAnchor    ->  -1 .. 0
    // sineAnchor .. 1.0  ->   0 .. 1
    const float volume = float(volumeSlider->getValue());
    const float anchor = this->volumeAnchorSine;
    const float volumeFactor = (volume < anchor) ? ((volume / anchor) - 1.f) : ((volume - anchor) / (1.f - anchor));

    Lasso &selection = this->roll.getLassoSelection();
    PianoRollToolbox::changeVolumeSine(selection, volumeFactor);
}

void NotesTuningPanel::syncSliders()
{
    float midValue = this->calcSliderValue();

    // for linear stuff
    this->volumeSliderLinear->setRange(0.0, 1.0, 0.006);
    this->volumeSliderLinear->setValue(midValue, dontSendNotification);

    // for multipliers
    this->volumeSliderMulti->setRange(0.0, 1.0, 0.003);
    this->volumeSliderMulti->setValue(this->flattenValue(this->flattenValue(midValue)), dontSendNotification);

    // for sine
    this->sineSlider->setRange(0.0, 1.0, 0.003);
    this->sineSlider->setValue(this->flattenValue(midValue), dontSendNotification);
}

float NotesTuningPanel::calcSliderValue()
{
    float volumeSum = 0.f;
    Lasso &selection = this->roll.getLassoSelection();

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        NoteComponent *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        volumeSum += nc->getVelocity();
    }

    volumeSum = volumeSum / selection.getNumSelected();
    return volumeSum;
}

float NotesTuningPanel::flattenValue(float value) const
{
    return (value + 0.5f) / 2.f;
}

void NotesTuningPanel::startTuning()
{
    Lasso &selection = this->roll.getLassoSelection();
    PianoRollToolbox::startTuning(selection);
}

void NotesTuningPanel::endTuning()
{
    Lasso &selection = this->roll.getLassoSelection();
    PianoRollToolbox::endTuning(selection);
}


//===----------------------------------------------------------------------===//
// TransportListener
//

void NotesTuningPanel::onSeek(const double newPosition,
                              const double currentTimeMs,
                              const double totalTimeMs)
{
}

void NotesTuningPanel::onTempoChanged(const double newTempo)
{
}

void NotesTuningPanel::onTotalTimeChanged(const double timeMs)
{
}

void NotesTuningPanel::onPlay()
{
    this->playButton->setPlaying(true);
}

void NotesTuningPanel::onStop()
{
    this->playButton->setPlaying(false);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="NotesTuningPanel" template="../../Template"
                 componentName="" parentClasses="public Component, public ChangeBroadcaster, private TransportListener"
                 constructorParams="ProjectTreeItem &amp;parentProject, PianoRoll &amp;targetRoll"
                 variableInitialisers="roll(targetRoll),&#10;project(parentProject),&#10;hasMadeChanges(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="250" initialHeight="240">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="274b1411c9ae170b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="a4c6c295088640a8" memberName="sliderLinearButton"
             virtualName="" explicitFocusOrder="0" pos="46c 56c 64 64" sourceFile="../Popups/PopupButton.cpp"
             constructorParams="false"/>
  <JUCERCOMP name="" id="fd2a7dfd7daba8e3" memberName="sliderMultiplyButton"
             virtualName="" explicitFocusOrder="0" pos="126c 56c 64 64" sourceFile="../Popups/PopupButton.cpp"
             constructorParams="false"/>
  <SLIDER name="" id="645d4540b1ee1178" memberName="volumeSliderMulti"
          virtualName="" explicitFocusOrder="0" pos="126c 56c 64 64" min="0"
          max="10" int="0" style="RotaryHorizontalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
  <SLIDER name="" id="612822c144ea1163" memberName="volumeSliderLinear"
          virtualName="" explicitFocusOrder="0" pos="46c 56c 64 64" posRelativeX="901299ec4e766469"
          posRelativeY="901299ec4e766469" textboxtext="0" min="0" max="10"
          int="0" style="RotaryHorizontalDrag" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1" needsCallback="1"/>
  <GENERICCOMPONENT name="" id="808594cf08a73350" memberName="tuningDiagram" virtualName=""
                    explicitFocusOrder="0" pos="0 96 0M 88" class="NotesTuningDiagram"
                    params="this, this-&gt;roll.getLassoSelection()"/>
  <JUCERCOMP name="" id="8013b7ac0b043720" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="0 96 0M 88" sourceFile="../Themes/PanelA.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="resetButton" virtualName=""
                    explicitFocusOrder="0" pos="85c 213c 58 58" class="CommandItemComponent"
                    params="this, nullptr, CommandItem::withParams(Icons::reset, CommandIDs::ResetVolumeChanges)"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="160c 214c 64 64" sourceFile="../Common/PlayButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="shadowDown" id="e71c535b988926e8" memberName="shadowDown"
             virtualName="" explicitFocusOrder="0" pos="0 180 0M 24" sourceFile="../Themes/ShadowDownwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="922ef78567538854" memberName="sliderSineButton" virtualName=""
             explicitFocusOrder="0" pos="206c 56c 64 64" sourceFile="../Popups/PopupButton.cpp"
             constructorParams="false"/>
  <SLIDER name="" id="bdc5e7b689607511" memberName="sineSlider" virtualName=""
          explicitFocusOrder="0" pos="206c 56c 64 64" posRelativeX="901299ec4e766469"
          posRelativeY="901299ec4e766469" textboxtext="0" min="0" max="10"
          int="0" style="RotaryHorizontalDrag" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1" needsCallback="1"/>
  <LABEL name="" id="2ef200f2e484c3e7" memberName="linearLabel" virtualName=""
         explicitFocusOrder="0" pos="18 0 56 24" textCol="3fffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="+" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="434928c32f07c6b9" memberName="multiLabel" virtualName=""
         explicitFocusOrder="0" pos="98 5 56 24" textCol="3fffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="*" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="6fcffdd210c02711" memberName="sineLabel" virtualName=""
         explicitFocusOrder="0" pos="178 0 56 24" textCol="3fffffff" edTextCol="ff000000"
         edBkgCol="0" labelText="~" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

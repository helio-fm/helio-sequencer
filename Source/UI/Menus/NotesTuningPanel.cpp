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
#include "SequencerOperations.h"
#include "MidiTrack.h"
#include "MidiSequence.h"
#include "NoteComponent.h"
#include "ProjectNode.h"
#include "Transport.h"
#include "CommandIDs.h"
#include "MenuItemComponent.h"

class NotesTuningDiagram final : public Component, private ChangeListener
{
public:

    NotesTuningDiagram(NotesTuningPanel *parentPanel, const Lasso &selection) :
        parent(parentPanel),
        startBeat(SequencerOperations::findStartBeat(selection)),
        endBeat(SequencerOperations::findEndBeat(selection))
    {
        bool foundColour = false;
        const Colour baseColour(findDefaultColour(Label::textColourId));

        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            auto *mc = selection.getItemAs<MidiEventComponent>(i);
            this->sortedSelection.addSorted(*mc, mc);

            if (!foundColour && dynamic_cast<NoteComponent *>(mc))
            {
                foundColour = true;
                this->colour = static_cast<NoteComponent *>(mc)->getNote()
                    .getSequence()->getTrack()->getTrackColour()
                    .interpolatedWith(baseColour, 0.55f).withAlpha(0.55f);
            }
        }

        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);

        this->parent->addChangeListener(this);
    }

    ~NotesTuningDiagram() override
    {
        this->parent->removeChangeListener(this);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->colour);
        for (const auto &i : this->peaks)
        {
            g.fillRect(i);
        }
        g.drawVerticalLine(0, 1.f, float(this->getHeight() - 1));
        g.drawVerticalLine(this->getWidth() - 1, 1.f, float(this->getHeight() - 1));
        g.drawHorizontalLine(0, 1.f, float(this->getWidth() - 1));
        g.drawHorizontalLine(this->getHeight() - 1, 1.f, float(this->getWidth() - 1));
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
        this->peaks.clearQuick();
        for (auto *i : this->sortedSelection)
        {
            if (auto *nc = dynamic_cast<NoteComponent *>(i))
            {
                const float absPos = (nc->getBeat() - this->startBeat) / (this->endBeat - this->startBeat);
                const float absLen = nc->getLength() / (this->endBeat - this->startBeat);
                const int x = this->proportionOfWidth(absPos);
                const int w = this->proportionOfWidth(absLen);
                const int y = this->proportionOfHeight(1.f - nc->getNote().getVelocity());
                const int h = this->proportionOfHeight(nc->getNote().getVelocity());
                this->peaks.add({ x, y, w, h });
            }
        }
    }

private:

    const float startBeat;
    const float endBeat;
    Colour colour;

    Array<Rectangle<int>> peaks;
    Array<MidiEventComponent *> sortedSelection;

    SafePointer<NotesTuningPanel> parent;
};

//[/MiscUserDefs]

NotesTuningPanel::NotesTuningPanel(ProjectNode &parentProject, PianoRoll &targetRoll)
    : roll(targetRoll),
      project(parentProject),
      hasMadeChanges(false)
{
    this->bg.reset(new PanelBackgroundC());
    this->addAndMakeVisible(bg.get());
    this->sliderLinearButton.reset(new PopupButton(false));
    this->addAndMakeVisible(sliderLinearButton.get());
    this->sliderMultiplyButton.reset(new PopupButton(false));
    this->addAndMakeVisible(sliderMultiplyButton.get());
    this->volumeSliderMulti.reset(new Slider(String()));
    this->addAndMakeVisible(volumeSliderMulti.get());
    volumeSliderMulti->setRange (0, 10, 0);
    volumeSliderMulti->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    volumeSliderMulti->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    volumeSliderMulti->addListener (this);

    this->volumeSliderLinear.reset(new Slider(String()));
    this->addAndMakeVisible(volumeSliderLinear.get());
    volumeSliderLinear->setRange (0, 10, 0);
    volumeSliderLinear->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    volumeSliderLinear->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    volumeSliderLinear->addListener (this);

    this->tuningDiagram.reset(new NotesTuningDiagram(this, this->roll.getLassoSelection()));
    this->addAndMakeVisible(tuningDiagram.get());

    this->resetButton.reset(new MenuItemComponent(this, nullptr, MenuItem::item(Icons::reset, CommandIDs::ResetVolumeChanges)));
    this->addAndMakeVisible(resetButton.get());

    this->playButton.reset(new PlayButton(nullptr));
    this->addAndMakeVisible(playButton.get());
    this->sliderSineButton.reset(new PopupButton(false));
    this->addAndMakeVisible(sliderSineButton.get());
    this->sineSlider.reset(new Slider(String()));
    this->addAndMakeVisible(sineSlider.get());
    sineSlider->setRange (0, 10, 0);
    sineSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    sineSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    sineSlider->addListener (this);

    this->linearLabel.reset(new Label(String(),
                                       TRANS("+")));
    this->addAndMakeVisible(linearLabel.get());
    this->linearLabel->setFont(Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    linearLabel->setJustificationType(Justification::centred);
    linearLabel->setEditable(false, false, false);

    this->multiLabel.reset(new Label(String(),
                                      TRANS("*")));
    this->addAndMakeVisible(multiLabel.get());
    this->multiLabel->setFont(Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    multiLabel->setJustificationType(Justification::centred);
    multiLabel->setEditable(false, false, false);

    this->sineLabel.reset(new Label(String(),
                                     TRANS("~")));
    this->addAndMakeVisible(sineLabel.get());
    this->sineLabel->setFont(Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    sineLabel->setJustificationType(Justification::centred);
    sineLabel->setEditable(false, false, false);


    //[UserPreSize]
    this->syncSliders();
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->linearLabel->setInterceptsMouseClicks(false, false);
    this->multiLabel->setInterceptsMouseClicks(false, false);
    this->sineLabel->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(250, 222);

    //[Constructor]
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
    resetButton = nullptr;
    playButton = nullptr;
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

    bg->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    sliderLinearButton->setBounds((46 - (56 / 2)) + 56 / 2 - (56 / 2), 118 + 56 / 2 - (56 / 2), 56, 56);
    sliderMultiplyButton->setBounds((126 - (56 / 2)) + 56 / 2 - (56 / 2), 118 + 56 / 2 - (56 / 2), 56, 56);
    volumeSliderMulti->setBounds(126 - (56 / 2), 118, 56, 56);
    volumeSliderLinear->setBounds(46 - (56 / 2), 118, 56, 56);
    tuningDiagram->setBounds(8, 8, getWidth() - 16, 102);
    resetButton->setBounds((getWidth() / 2) + -16 - 48, 197 - (48 / 2), 48, 48);
    playButton->setBounds((getWidth() / 2) + 16, 198 - (48 / 2), 48, 48);
    sliderSineButton->setBounds((206 - (56 / 2)) + 56 / 2 - (56 / 2), 118 + 56 / 2 - (56 / 2), 56, 56);
    sineSlider->setBounds(206 - (56 / 2), 118, 56, 56);
    linearLabel->setBounds((46 - (56 / 2)) + 56 / 2 - (30 / 2), 118 + 56 / 2 + -2 - (30 / 2), 30, 30);
    multiLabel->setBounds((126 - (56 / 2)) + 56 / 2 - (30 / 2), 118 + 56 / 2 + 2 - (30 / 2), 30, 30);
    sineLabel->setBounds((206 - (56 / 2)) + 56 / 2 - (30 / 2), 118 + 56 / 2 + -1 - (30 / 2), 30, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void NotesTuningPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == volumeSliderMulti.get())
    {
        //[UserSliderCode_volumeSliderMulti] -- add your slider handling code here..
        this->syncVolumeMultiplied(sliderThatWasMoved);
        this->sendChangeMessage();
        //[/UserSliderCode_volumeSliderMulti]
    }
    else if (sliderThatWasMoved == volumeSliderLinear.get())
    {
        //[UserSliderCode_volumeSliderLinear] -- add your slider handling code here..
        this->syncVolumeLinear(sliderThatWasMoved);
        this->sendChangeMessage();
        //[/UserSliderCode_volumeSliderLinear]
    }
    else if (sliderThatWasMoved == sineSlider.get())
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
            this->project.undo();
            this->syncSliders();
            this->sendChangeMessage();
            this->hasMadeChanges = false;
        }
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
    if (slider == this->volumeSliderMulti.get())
    {
        this->volumeAnchorMulti = float(this->volumeSliderMulti->getValue());
    }
    else if (slider == this->volumeSliderLinear.get())
    {
        this->volumeAnchorLinear = float(this->volumeSliderLinear->getValue());
    }
    else if (slider == this->sineSlider.get())
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
        this->project.checkpoint();
        this->hasMadeChanges = true;
    }

    const float volumeDelta = this->volumeAnchorLinear - float(volumeSlider->getValue());
    Lasso &selection = this->roll.getLassoSelection();
    SequencerOperations::changeVolumeLinear(selection, volumeDelta);
}

void NotesTuningPanel::syncVolumeMultiplied(Slider *volumeSlider)
{
    if (! this->hasMadeChanges)
    {
        this->project.checkpoint();
        this->hasMadeChanges = true;
    }

    // 0 .. volumeAnchor    ->  -1 .. 0
    // volumeAnchor .. 1.0  ->   0 .. 1
    const float volume = float(volumeSlider->getValue());
    const float anchor = this->volumeAnchorMulti;
    const float volumeFactor = (volume < anchor) ? ((volume / anchor) - 1.f) : ((volume - anchor) / (1.f - anchor));

    Lasso &selection = this->roll.getLassoSelection();
    SequencerOperations::changeVolumeMultiplied(selection, volumeFactor);
}

void NotesTuningPanel::syncVolumeSine(Slider *volumeSlider)
{
    if (! this->hasMadeChanges)
    {
        this->project.checkpoint();
        this->hasMadeChanges = true;
    }

    // 0 .. sineAnchor    ->  -1 .. 0
    // sineAnchor .. 1.0  ->   0 .. 1
    const float volume = float(volumeSlider->getValue());
    const float anchor = this->volumeAnchorSine;
    const float volumeFactor = (volume < anchor) ? ((volume / anchor) - 1.f) : ((volume - anchor) / (1.f - anchor));

    Lasso &selection = this->roll.getLassoSelection();
    SequencerOperations::changeVolumeSine(selection, volumeFactor);
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
    SequencerOperations::startTuning(selection);
}

void NotesTuningPanel::endTuning()
{
    Lasso &selection = this->roll.getLassoSelection();
    SequencerOperations::endTuning(selection);
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void NotesTuningPanel::onTempoChanged(double msPerQuarter) {}
void NotesTuningPanel::onTotalTimeChanged(double timeMs) {}
void NotesTuningPanel::onSeek(double absolutePosition,
                              double currentTimeMs,
                              double totalTimeMs) {}

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
                 constructorParams="ProjectNode &amp;parentProject, PianoRoll &amp;targetRoll"
                 variableInitialisers="roll(targetRoll),&#10;project(parentProject),&#10;hasMadeChanges(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="250" initialHeight="222">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="274b1411c9ae170b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="a4c6c295088640a8" memberName="sliderLinearButton"
             virtualName="" explicitFocusOrder="0" pos="0Cc 0Cc 56 56" posRelativeX="612822c144ea1163"
             posRelativeY="612822c144ea1163" sourceFile="../Popups/PopupButton.cpp"
             constructorParams="false"/>
  <JUCERCOMP name="" id="fd2a7dfd7daba8e3" memberName="sliderMultiplyButton"
             virtualName="" explicitFocusOrder="0" pos="0Cc 0Cc 56 56" posRelativeX="645d4540b1ee1178"
             posRelativeY="645d4540b1ee1178" sourceFile="../Popups/PopupButton.cpp"
             constructorParams="false"/>
  <SLIDER name="" id="645d4540b1ee1178" memberName="volumeSliderMulti"
          virtualName="" explicitFocusOrder="0" pos="126c 118 56 56" min="0.00000000000000000000"
          max="10.00000000000000000000" int="0.00000000000000000000" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.00000000000000000000" needsCallback="1"/>
  <SLIDER name="" id="612822c144ea1163" memberName="volumeSliderLinear"
          virtualName="" explicitFocusOrder="0" pos="46c 118 56 56" posRelativeX="901299ec4e766469"
          posRelativeY="901299ec4e766469" min="0.00000000000000000000"
          max="10.00000000000000000000" int="0.00000000000000000000" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.00000000000000000000" needsCallback="1"/>
  <GENERICCOMPONENT name="" id="808594cf08a73350" memberName="tuningDiagram" virtualName=""
                    explicitFocusOrder="0" pos="8 8 16M 102" class="NotesTuningDiagram"
                    params="this, this-&gt;roll.getLassoSelection()"/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="resetButton" virtualName=""
                    explicitFocusOrder="0" pos="-16Cr 197c 48 48" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::reset, CommandIDs::ResetVolumeChanges)"/>
  <JUCERCOMP name="" id="bb2e14336f795a57" memberName="playButton" virtualName=""
             explicitFocusOrder="0" pos="16C 198c 48 48" sourceFile="../Common/PlayButton.cpp"
             constructorParams="nullptr"/>
  <JUCERCOMP name="" id="922ef78567538854" memberName="sliderSineButton" virtualName=""
             explicitFocusOrder="0" pos="0Cc 0Cc 56 56" posRelativeX="bdc5e7b689607511"
             posRelativeY="bdc5e7b689607511" sourceFile="../Popups/PopupButton.cpp"
             constructorParams="false"/>
  <SLIDER name="" id="bdc5e7b689607511" memberName="sineSlider" virtualName=""
          explicitFocusOrder="0" pos="206c 118 56 56" posRelativeX="901299ec4e766469"
          posRelativeY="901299ec4e766469" min="0.00000000000000000000"
          max="10.00000000000000000000" int="0.00000000000000000000" style="RotaryHorizontalVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1.00000000000000000000" needsCallback="1"/>
  <LABEL name="" id="2ef200f2e484c3e7" memberName="linearLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc -2Cc 30 30" posRelativeX="612822c144ea1163"
         posRelativeY="612822c144ea1163" labelText="+" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
  <LABEL name="" id="434928c32f07c6b9" memberName="multiLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 2Cc 30 30" posRelativeX="645d4540b1ee1178"
         posRelativeY="645d4540b1ee1178" labelText="*" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
  <LABEL name="" id="6fcffdd210c02711" memberName="sineLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc -1Cc 30 30" posRelativeX="bdc5e7b689607511"
         posRelativeY="bdc5e7b689607511" labelText="~" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "MidiTrackHeader.h"

//[MiscUserDefs]
#include "MidiTrack.h"
//[/MiscUserDefs]

MidiTrackHeader::MidiTrackHeader(const MidiTrack &track)
    : track(track)
{
    addAndMakeVisible (trackNameLabel = new Label (String(),
                                                   TRANS("label text")));
    trackNameLabel->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    trackNameLabel->setJustificationType (Justification::centredLeft);
    trackNameLabel->setEditable (true, true, false);
    trackNameLabel->setColour (TextEditor::textColourId, Colours::black);
    trackNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    trackNameLabel->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (400, 24);

    //[Constructor]
    this->updateContent();
    //[/Constructor]
}

MidiTrackHeader::~MidiTrackHeader()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    trackNameLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void MidiTrackHeader::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

    //g.setColour(this->findColour(MidiTrackHeaderComponent::headerColourId));
    //g.fillRect(this->getLocalBounds());

    //g.setColour(this->bevelLightColour);
    //g.drawHorizontalLine(0, 0.f, float(this->getWidth()));

    //g.setColour(this->bevelDarkColour);
    //g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));

    //[/UserPrePaint]

    g.fillAll (Colour (0xff464646));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MidiTrackHeader::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    trackNameLabel->setBounds (0, 0, proportionOfWidth (0.5000f), getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MidiTrackHeader::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == trackNameLabel)
    {
        //[UserLabelCode_trackNameLabel] -- add your label text handling code here..
        //[/UserLabelCode_trackNameLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}


//[MiscUserCode]
void MidiTrackHeader::updateContent()
{
    this->trackNameLabel->setText(this->track.getTrackName(), dontSendNotification);
    // TODO colours etc.
}

const MidiTrack &MidiTrackHeader::getTrack() const noexcept
{
    return this->track;
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MidiTrackHeader" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="const MidiTrack &amp;track"
                 variableInitialisers="track(track)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="400"
                 initialHeight="24">
  <BACKGROUND backgroundColour="ff464646"/>
  <LABEL name="" id="e57adff1b4a0a2b6" memberName="trackNameLabel" virtualName=""
         explicitFocusOrder="0" pos="0 0 50% 0M" edTextCol="ff000000"
         edBkgCol="0" labelText="label text" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

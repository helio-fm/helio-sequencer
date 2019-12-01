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

#include "NoteNameGuide.h"

//[MiscUserDefs]
#include "ColourIDs.h"
#include "CachedLabelImage.h"
//[/MiscUserDefs]

NoteNameGuide::NoteNameGuide(int noteNumber)
    : noteNumber(noteNumber),
      fillColour(findDefaultColour(ColourIDs::Roll::noteNameFill)),
      borderColour(findDefaultColour(ColourIDs::Roll::noteNameBorder)),
      shadowColour(findDefaultColour(ColourIDs::Roll::noteNameShadow))
{
    this->noteNameLabel.reset(new Label(String(),
                                         String()));
    this->addAndMakeVisible(noteNameLabel.get());
    this->noteNameLabel->setFont(Font (16.00f, Font::plain));
    noteNameLabel->setJustificationType(Justification::centredLeft);
    noteNameLabel->setEditable(false, false, false);


    //[UserPreSize]
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);

    this->noteNameLabel->setBufferedToImage(true);
    this->noteNameLabel->setCachedComponentImage(new CachedLabelImage(*this->noteNameLabel));
    this->noteNameLabel->setText(MidiMessage::getMidiNoteName(noteNumber, true, true, 3), dontSendNotification);
    //[/UserPreSize]

    this->setSize(36, 32);

    //[Constructor]
    //[/Constructor]
}

NoteNameGuide::~NoteNameGuide()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    noteNameLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void NoteNameGuide::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0x44ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0xc1000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath2, AffineTransform::translation(x, y));
    }

    {
        int x = 0, y = 1, width = 2, height = getHeight() - 1;
        Colour fillColour = Colour (0x88ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    g.setColour(this->shadowColour);
    g.fillPath(this->internalPath1);

    g.setColour(this->fillColour);
    g.fillPath(this->internalPath2);

    g.setColour(this->borderColour);
    g.fillRect(0, 1, 2, this->getHeight() - 1);

    //[/UserPaint]
}

void NoteNameGuide::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    noteNameLabel->setBounds(1, (getHeight() / 2) - (21 / 2), 38, 21);
    internalPath1.clear();
    internalPath1.startNewSubPath (3.0f, 1.0f);
    internalPath1.lineTo (30.0f, 1.0f);
    internalPath1.lineTo (34.0f, static_cast<float> (getHeight()));
    internalPath1.lineTo (3.0f, static_cast<float> (getHeight()));
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath (0.0f, 1.0f);
    internalPath2.lineTo (29.0f, 1.0f);
    internalPath2.lineTo (33.0f, static_cast<float> (getHeight()));
    internalPath2.lineTo (0.0f, static_cast<float> (getHeight()));
    internalPath2.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="NoteNameGuide" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="int noteNumber"
                 variableInitialisers="noteNumber(noteNumber),&#10;fillColour(findDefaultColour(ColourIDs::Roll::noteNameFill)),&#10;borderColour(findDefaultColour(ColourIDs::Roll::noteNameBorder)),&#10;shadowColour(findDefaultColour(ColourIDs::Roll::noteNameShadow))"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="36" initialHeight="32">
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: 44ffffff" hasStroke="0" nonZeroWinding="1">s 3 1 l 30 1 l 34 0R l 3 0R x</PATH>
    <PATH pos="0 0 100 100" fill="solid: c1000000" hasStroke="0" nonZeroWinding="1">s 0 1 l 29 1 l 33 0R l 0 0R x</PATH>
    <RECT pos="0 1 2 1M" fill="solid: 88ffffff" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="bfd24d0a91476b7" memberName="noteNameLabel" virtualName=""
         explicitFocusOrder="0" pos="1 0.5Cc 38 21" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif




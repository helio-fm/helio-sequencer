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
#include "HelioTheme.h"
#include "CachedLabelImage.h"
#include "ColourIDs.h"
//[/MiscUserDefs]

MidiTrackHeader::MidiTrackHeader(const MidiTrack *track)
    : track(track)
{
    addAndMakeVisible (trackNameEditor = new TextEditor (String()));
    trackNameEditor->setMultiLine (false);
    trackNameEditor->setReturnKeyStartsNewLine (false);
    trackNameEditor->setReadOnly (false);
    trackNameEditor->setScrollbarsShown (false);
    trackNameEditor->setCaretVisible (true);
    trackNameEditor->setPopupMenuEnabled (true);
    trackNameEditor->setText (TRANS("..."));

    addAndMakeVisible (trackNameLabel = new Label (String(),
                                                   String()));
    trackNameLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    trackNameLabel->setJustificationType (Justification::centredLeft);
    trackNameLabel->setEditable (false, false, false);
    trackNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (setNameButton = new ImageButton (String()));
    setNameButton->addListener (this);

    setNameButton->setImages (false, true, false,
                              Image(), 1.000f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000));

    //[UserPreSize]
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);

    this->trackNameLabel->setBufferedToImage(true);
    this->trackNameLabel->setCachedComponentImage(new CachedLabelImage(*this->trackNameLabel));
    //this->trackNameLabel->setInterceptsMouseClicks(false, false);

    this->trackNameLabel->setFocusContainer(false);
    this->trackNameLabel->setWantsKeyboardFocus(false);

    //[/UserPreSize]

    setSize (400, 32);

    //[Constructor]
    this->trackNameEditor->setFont(this->trackNameLabel->getFont());

    // Prevent grabbing a focus by track name editor:
    //g.setColour(this->findColour(TextEditor::backgroundColourId));
    //g.setColour(this->findColour(TextEditor::outlineColourId));

    this->trackNameEditor->addListener(this);
    this->trackNameEditor->setFocusContainer(true);
    this->trackNameEditor->setVisible(false);

    this->updateContent();
    //[/Constructor]
}

MidiTrackHeader::~MidiTrackHeader()
{
    //[Destructor_pre]
    this->trackNameEditor->removeListener(this);
    //[/Destructor_pre]

    trackNameEditor = nullptr;
    trackNameLabel = nullptr;
    setNameButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void MidiTrackHeader::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setTiledImageFill(this->fillImage, 0, 0, 1.f);
    g.fillRect(this->getLocalBounds());

    g.setColour(this->lineColour);

    const int x = 1; // JUCE_LIVE_CONSTANT(1);
    const float y1 = 2.f; // JUCE_LIVE_CONSTANT(2.f);
    const float y2 = 29.f; // JUCE_LIVE_CONSTANT(29.f);

    g.drawVerticalLine(x, y1, y2);
    g.drawVerticalLine(x + 1, y1, y2);

    //g.drawVerticalLine(this->getWidth() - x - 1, y1, y2);
    //g.drawVerticalLine(this->getWidth() - x - 2, y1, y2);

    g.setColour(this->borderLightColour);
    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));

    //g.setColour(this->borderDarkColour);
    g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));
    //[/UserPaint]
}

void MidiTrackHeader::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    trackNameEditor->setBounds (5, 0, getWidth() - 6, getHeight() - 0);
    trackNameLabel->setBounds (5, 5, 256, 20);
    setNameButton->setBounds (8, 0, 256, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MidiTrackHeader::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == setNameButton)
    {
        //[UserButtonCode_setNameButton] -- add your button handler code here..
        this->trackNameLabel->setVisible(false);
        this->trackNameEditor->setVisible(true);
        this->trackNameEditor->grabKeyboardFocus();
        //[/UserButtonCode_setNameButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}


//[MiscUserCode]
void MidiTrackHeader::textEditorReturnKeyPressed(TextEditor&)
{
    this->trackNameEditor->setVisible(false);
    this->trackNameLabel->setVisible(true);
}

void MidiTrackHeader::textEditorEscapeKeyPressed(TextEditor&)
{
    this->trackNameEditor->setVisible(false);
    this->trackNameLabel->setVisible(true);
}

void MidiTrackHeader::textEditorFocusLost(TextEditor&)
{
    this->trackNameEditor->setVisible(false);
    this->trackNameLabel->setVisible(true);
}

void MidiTrackHeader::updateContent()
{
    const Colour defaultColour(Colours::black);
    const Colour trackColour(this->track ? this->track->getTrackColour() : defaultColour);
    this->lineColour = trackColour.withMultipliedAlpha(0.55f).brighter(0.5f);
    this->borderLightColour = this->findColour(ColourIDs::Roll::trackHeaderBorderLight);
    this->borderDarkColour = this->findColour(ColourIDs::Roll::trackHeaderBorderDark);
    this->fillImage = Image(Image::RGB, 128, this->getHeight(), false);

    {
        Graphics g(this->fillImage);
        g.setColour(this->findColour(ColourIDs::Roll::trackHeaderFill).interpolatedWith(trackColour, 0.01f));
        g.fillRect(this->fillImage.getBounds());
        const auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
        HelioTheme::drawNoise(theme, g, 2.5f);
    }

    const String addTrackLabel = TRANS("menu::project::addlayer") + "..."; // TODO more detailed text? like "type a name to add new track"
    const Colour labelColour = this->getLookAndFeel().findColour(Label::textColourId);
    const Colour newLabelColour = labelColour.interpolatedWith(trackColour, 0.2f).withMultipliedAlpha(this->track ? 1.f : 0.75f);

    this->trackNameLabel->setColour(Label::textColourId, newLabelColour);
    this->trackNameLabel->setText(this->track ? this->track->getTrackName() : addTrackLabel, dontSendNotification);

    this->trackNameEditor->setColour(TextEditor::textColourId, newLabelColour);
    this->trackNameEditor->setText(this->track ? this->track->getTrackName() : "", false);

    this->textWidth = jmin(this->trackNameLabel->getWidth(),
        this->trackNameLabel->getFont().getStringWidth(this->trackNameLabel->getText()));
}

const MidiTrack *MidiTrackHeader::getTrack() const noexcept
{
    return this->track;
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MidiTrackHeader" template="../../../Template"
                 componentName="" parentClasses="public Component, public TextEditor::Listener"
                 constructorParams="const MidiTrack *track" variableInitialisers="track(track)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="400" initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <TEXTEDITOR name="" id="fb4a58b4245a4b25" memberName="trackNameEditor" virtualName=""
              explicitFocusOrder="0" pos="5 0 6M 0M" initialText="..." multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="1"/>
  <LABEL name="" id="d3d4f85f2ceefa1c" memberName="trackNameLabel" virtualName=""
         explicitFocusOrder="0" pos="5 5 256 20" edBkgCol="0" labelText=""
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18" kerning="0" bold="0" italic="0"
         justification="33"/>
  <IMAGEBUTTON name="" id="15bec82dddd3c004" memberName="setNameButton" virtualName=""
               explicitFocusOrder="0" pos="8 0 256 0M" buttonText="" connectedEdges="0"
               needsCallback="1" radioGroupId="0" keepProportions="0" resourceNormal=""
               opacityNormal="1" colourNormal="0" resourceOver="" opacityOver="1"
               colourOver="0" resourceDown="" opacityDown="1" colourDown="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "MidiTrackTreeItem.h"
#include "ModalDialogInput.h"
#include "MainLayout.h"
#include "App.h"
//[/MiscUserDefs]

MidiTrackHeader::MidiTrackHeader(MidiTrack *track)
    : track(track)
{
    addAndMakeVisible (trackNameLabel = new Label (String(),
                                                   String()));
    trackNameLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    trackNameLabel->setJustificationType (Justification::centredLeft);
    trackNameLabel->setEditable (false, false, false);
    trackNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    trackNameLabel->setBounds (5, 5, 256, 20);

    addAndMakeVisible (setNameButton = new ImageButton (String()));
    setNameButton->addListener (this);

    setNameButton->setImages (false, true, false,
                              Image(), 1.000f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000),
                              Image(), 1.000f, Colour (0x00000000));

    //[UserPreSize]
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setOpaque(true);

    this->trackNameLabel->setBufferedToImage(true);
    this->trackNameLabel->setCachedComponentImage(new CachedLabelImage(*this->trackNameLabel));
    this->trackNameLabel->setInterceptsMouseClicks(false, false);

    this->setNameButton->setMouseCursor(MouseCursor::PointingHandCursor);
    this->setNameButton->setMouseClickGrabsKeyboardFocus(false);
    this->setNameButton->setWantsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (400, 32);

    //[Constructor]
    this->updateContent();
    //[/Constructor]
}

MidiTrackHeader::~MidiTrackHeader()
{
    //[Destructor_pre]
    //[/Destructor_pre]

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
#if 0
    //[/UserPreResize]

    setNameButton->setBounds (8, 0, 256, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
#endif
    //[/UserResized]
}

void MidiTrackHeader::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == setNameButton)
    {
        //[UserButtonCode_setNameButton] -- add your button handler code here..
        if (auto *trackNode = dynamic_cast<MidiTrackTreeItem *>(this->track))
        {
            auto inputDialog = ModalDialogInput::Presets::renameTrack(trackNode->getXPath());
            inputDialog->onOk = trackNode->getRenameCallback();
            App::Layout().showModalComponentUnowned(inputDialog.release());
        }
        //[/UserButtonCode_setNameButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}


//[MiscUserCode]
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

    this->textWidth = jmin(this->trackNameLabel->getWidth(),
        this->trackNameLabel->getFont().getStringWidth(this->trackNameLabel->getText()));

    this->setNameButton->setBounds(8, 0, this->textWidth, this->getHeight());
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
                 componentName="" parentClasses="public Component" constructorParams="MidiTrack *track"
                 variableInitialisers="track(track)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="400"
                 initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="d3d4f85f2ceefa1c" memberName="trackNameLabel" virtualName=""
         explicitFocusOrder="0" pos="5 5 256 20" edBkgCol="0" labelText=""
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <IMAGEBUTTON name="" id="15bec82dddd3c004" memberName="setNameButton" virtualName=""
               explicitFocusOrder="0" pos="8 0 256 0M" buttonText="" connectedEdges="0"
               needsCallback="1" radioGroupId="0" keepProportions="0" resourceNormal=""
               opacityNormal="1.00000000000000000000" colourNormal="0" resourceOver=""
               opacityOver="1.00000000000000000000" colourOver="0" resourceDown=""
               opacityDown="1.00000000000000000000" colourDown="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

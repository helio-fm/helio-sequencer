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

#include "HeadlineItemHighlighter.h"

//[MiscUserDefs]
#include "Headline.h"
#include "IconComponent.h"
#include "ColourIDs.h"
//[/MiscUserDefs]

HeadlineItemHighlighter::HeadlineItemHighlighter(WeakReference<HeadlineItemDataSource> targetItem)
    : item(targetItem)
{
    this->titleLabel.reset(new Label(String(),
                                      TRANS("Project")));
    this->addAndMakeVisible(titleLabel.get());
    this->titleLabel->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType(Justification::centredLeft);
    titleLabel->setEditable(false, false, false);

    this->icon.reset(new IconComponent(Icons::helio));
    this->addAndMakeVisible(icon.get());


    //[UserPreSize]
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->titleLabel->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(150, 34);

    //[Constructor]
    if (this->item != nullptr)
    {
        this->icon->setIconImage(this->item->getIcon());
        this->titleLabel->setText(this->item->getName(), dontSendNotification);
        const int textWidth = this->titleLabel->getFont()
            .getStringWidth(this->titleLabel->getText());
        this->setSize(textWidth + 64, HEADLINE_HEIGHT);
    }
    //[/Constructor]
}

HeadlineItemHighlighter::~HeadlineItemHighlighter()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    titleLabel = nullptr;
    icon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineItemHighlighter::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour1 = Colour (0x33000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       0.0f - 0.0f + x,
                                       static_cast<float> ((getHeight() / 2) + -1) - 0.0f + y,
                                       fillColour2,
                                       16.0f - 0.0f + x,
                                       static_cast<float> ((getHeight() / 2) + -3) - 0.0f + y,
                                       true));
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0x15ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = findDefaultColour(ColourIDs::BackgroundA::fill).brighter(0.035f);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath2, AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x66000000), strokeColour2 = Colour (0x11000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 2) - 0.0f + x,
                                       static_cast<float> (getHeight() - 2) - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 16) - 0.0f + x,
                                       2.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath3, PathStrokeType (1.000f), AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x27ffffff), strokeColour2 = Colour (0x0bffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                       static_cast<float> (getWidth() - 3) - 0.0f + x,
                                       static_cast<float> (getHeight() - 2) - 0.0f + y,
                                       strokeColour2,
                                       static_cast<float> (getWidth() - 17) - 0.0f + x,
                                       5.0f - 0.0f + y,
                                       true));
        g.strokePath (internalPath4, PathStrokeType (0.500f), AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HeadlineItemHighlighter::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    titleLabel->setBounds(33, (getHeight() / 2) + -1 - (30 / 2), getWidth() - 44, 30);
    icon->setBounds(7, (getHeight() / 2) + -1 - (32 / 2), 32, 32);
    internalPath1.clear();
    internalPath1.startNewSubPath (0.0f, 0.0f);
    internalPath1.lineTo (40.0f, 0.0f);
    internalPath1.lineTo (40.0f, static_cast<float> (getHeight() - 2));
    internalPath1.lineTo (0.0f, static_cast<float> (getHeight() - 2));
    internalPath1.lineTo (8.0f, static_cast<float> ((getHeight() / 2) + -1));
    internalPath1.closeSubPath();

    internalPath2.clear();
    internalPath2.startNewSubPath (0.0f, 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 2), static_cast<float> (getHeight() - 2));
    internalPath2.lineTo (1.0f, static_cast<float> (getHeight() - 1));
    internalPath2.lineTo (1.0f, static_cast<float> (getHeight() - 2));
    internalPath2.lineTo (8.0f, static_cast<float> ((getHeight() / 2) + -1));
    internalPath2.closeSubPath();

    internalPath3.clear();
    internalPath3.startNewSubPath (static_cast<float> (getWidth() - -40), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath3.lineTo (static_cast<float> (getWidth() - 2), static_cast<float> (getHeight() - 2));
    internalPath3.lineTo (static_cast<float> (getWidth() - -40), static_cast<float> (getHeight() - 2));
    internalPath3.closeSubPath();

    internalPath4.clear();
    internalPath4.startNewSubPath (static_cast<float> (getWidth() - -24), 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 17), 0.0f);
    internalPath4.lineTo (static_cast<float> (getWidth() - 3), static_cast<float> (getHeight() - 2));
    internalPath4.lineTo (static_cast<float> (getWidth() - -24), static_cast<float> (getHeight() - 2));
    internalPath4.closeSubPath();

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineItemHighlighter"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="WeakReference&lt;HeadlineItemDataSource&gt; targetItem"
                 variableInitialisers="item(targetItem)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="150"
                 initialHeight="34">
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill=" radial: 0 -1C, 16 -3C, 0=33000000, 1=0"
          hasStroke="0" nonZeroWinding="1">s 0 0 l 40 0 l 40 2R l 0 2R l 8 -1C x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 15ffffff" hasStroke="0" nonZeroWinding="1">s 0 0 l 16R 0 l 2R 2R l 1 1R l 1 2R l 8 -1C x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour=" radial: 2R 2R, 16R 2, 0=66000000, 1=11000000"
          nonZeroWinding="1">s -40R 0 l 16R 0 l 2R 2R l -40R 2R x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
          strokeColour=" radial: 3R 2R, 17R 5, 0=27ffffff, 1=bffffff" nonZeroWinding="1">s -24R 0 l 17R 0 l 3R 2R l -24R 2R x</PATH>
  </BACKGROUND>
  <LABEL name="" id="9a3c449859f61884" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="33 -1Cc 44M 30" labelText="Project"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="icon" virtualName=""
                    explicitFocusOrder="0" pos="7 -1Cc 32 32" class="IconComponent"
                    params="Icons::helio"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

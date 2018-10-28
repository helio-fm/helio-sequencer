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

#include "DialogPanel.h"

//[MiscUserDefs]
#include "PanelBackgroundC.h"
#include "ColourIDs.h"
//[/MiscUserDefs]

DialogPanel::DialogPanel()
{

    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    this->setInterceptsMouseClicks(false, false);
    //[/Constructor]
}

DialogPanel::~DialogPanel()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void DialogPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour1 = Colour (0xff3b5297), fillColour2 = Colour (0xff292c57);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       0.0f - 0.0f + x,
                                       static_cast<float> (getHeight() - -200) - 0.0f + y,
                                       fillColour2,
                                       static_cast<float> (getWidth()) - 0.0f + x,
                                       0.0f - 0.0f + y,
                                       true));
        g.fillRoundedRectangle (x, y, width, height, 7.000f);
    }

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour1 = Colour (0x3a193477), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (proportionOfWidth (0.5200f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (1.2000f)) - 0.0f + y,
                                       fillColour2,
                                       static_cast<float> (proportionOfWidth (0.4800f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (-0.2000f)) - 0.0f + y,
                                       false));
        g.fillRoundedRectangle (x, y, width, height, 7.000f);
    }

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour1 = Colour (0x3a193477), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (proportionOfWidth (0.5200f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (1.3000f)) - 0.0f + y,
                                       fillColour2,
                                       static_cast<float> (proportionOfWidth (0.4800f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (-0.3000f)) - 0.0f + y,
                                       false));
        g.fillRoundedRectangle (x, y, width, height, 7.000f);
    }

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour1 = Colour (0x3a193477), fillColour2 = Colour (0x00000000);
        Colour strokeColour = Colour (0xffb9b9b9);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (proportionOfWidth (0.5200f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (1.3000f)) - 0.0f + y,
                                       fillColour2,
                                       static_cast<float> (proportionOfWidth (0.4800f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (-0.3000f)) - 0.0f + y,
                                       false));
        g.fillRoundedRectangle (x, y, width, height, 7.000f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 7.000f, 1.000f);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    g.setColour(findColour(ColourIDs::BackgroundC::fill));
    g.fillRoundedRectangle (0.0f, 0.0f, static_cast<float> (getWidth() - 0), static_cast<float> (getHeight() - 0), 7.000f);
    g.setColour (Colour (0x44b9b9b9));
    g.drawRoundedRectangle (0.5f, 0.5f, static_cast<float> (getWidth() - 1), static_cast<float> (getHeight() - 1), 6.000f, 1.000f);

    //[/UserPaint]
}

void DialogPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="DialogPanel" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="0" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="7.00000000000000000000" fill=" radial: 0 -200R, 0R 0, 0=ff3b5297, 1=ff292c57"
               hasStroke="0"/>
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="7.00000000000000000000" fill="linear: 52% 120%, 48% -20%, 0=3a193477, 1=0"
               hasStroke="0"/>
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="7.00000000000000000000" fill="linear: 52% 130%, 48% -30%, 0=3a193477, 1=0"
               hasStroke="0"/>
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="7.00000000000000000000" fill="linear: 52% 130%, 48% -30%, 0=3a193477, 1=0"
               hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ffb9b9b9"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

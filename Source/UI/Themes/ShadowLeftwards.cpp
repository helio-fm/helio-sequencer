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

#include "ShadowLeftwards.h"

//[MiscUserDefs]
#include "ColourIDs.h"
//[/MiscUserDefs]

ShadowLeftwards::ShadowLeftwards(ShadowType type)
    : ShadowComponent(type)
{

    //[UserPreSize]
    this->lineColour = findDefaultColour(ColourIDs::Common::borderLineDark);
    //[/UserPreSize]

    this->setSize(40, 400);

    //[Constructor]
    //[/Constructor]
}

ShadowLeftwards::~ShadowLeftwards()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void ShadowLeftwards::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour1 = Colour (0x15000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour1 = this->shadowColour;
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (getWidth()) - 0.0f + x,
                                       0.0f - 0.0f + y,
                                       fillColour2,
                                       0.0f - 0.0f + x,
                                       0.0f - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = getWidth() - proportionOfWidth (0.5000f), y = 0, width = proportionOfWidth (0.5000f), height = getHeight() - 0;
        Colour fillColour1 = Colour (0x15000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour1 = this->shadowColour;
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (getWidth()) - static_cast<float> (getWidth() - proportionOfWidth (0.5000f)) + x,
                                       0.0f - 0.0f + y,
                                       fillColour2,
                                       static_cast<float> ((getWidth() / 2)) - static_cast<float> (getWidth() - proportionOfWidth (0.5000f)) + x,
                                       0.0f - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    const float w = float(this->getWidth());
    g.setGradientFill(ColourGradient(this->shadowColour,
        w, 0.f,
        Colours::transparentBlack,
        0.f, 0.f, false));
    g.fillRect(this->getLocalBounds());

    g.setGradientFill(ColourGradient(this->shadowColour,
        w, 0.0f,
        Colours::transparentBlack,
        w / 2.5f, 0.f, false));
    g.fillRect(this->getLocalBounds());

    g.setColour(this->lineColour);
    g.drawVerticalLine(this->getWidth() - 1, 0.f, float(this->getHeight()));
    //[/UserPaint]
}

void ShadowLeftwards::resized()
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

<JUCER_COMPONENT documentType="Component" className="ShadowLeftwards" template="../../Template"
                 componentName="" parentClasses="public ShadowComponent" constructorParams="ShadowType type"
                 variableInitialisers="ShadowComponent(type)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="40"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="0 0 0M 0M" fill="linear: 0R 0, 0 0, 0=15000000, 1=0" hasStroke="0"/>
    <RECT pos="0Rr 0 50% 0M" fill="linear: 0R 0, 0C 0, 0=15000000, 1=0"
          hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

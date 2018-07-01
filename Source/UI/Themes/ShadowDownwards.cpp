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

#include "ShadowDownwards.h"

//[MiscUserDefs]
//[/MiscUserDefs]

ShadowDownwards::ShadowDownwards(ShadowType type)
    : ShadowComponent(type)
{

    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(600, 40);

    //[Constructor]
    //[/Constructor]
}

ShadowDownwards::~ShadowDownwards()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void ShadowDownwards::paint (Graphics& g)
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
                                       0.0f - 0.0f + x,
                                       0.0f - 0.0f + y,
                                       fillColour2,
                                       0.0f - 0.0f + x,
                                       static_cast<float> (getHeight()) - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 0, width = getWidth() - 0, height = proportionOfHeight (0.5000f);
        Colour fillColour1 = Colour (0x15000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour1 = this->shadowColour;
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       0.0f - 0.0f + x,
                                       0.0f - 0.0f + y,
                                       fillColour2,
                                       0.0f - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.5000f)) - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    const float h = float(this->getHeight());
    g.setGradientFill(ColourGradient(this->shadowColour,
        0.f, 0.f,
        Colours::transparentBlack,
        0.f, h, false));
    g.fillRect(this->getLocalBounds());

    g.setGradientFill(ColourGradient(this->shadowColour,
        0.f, 0.0f,
        Colours::transparentBlack,
        0.f, h / 2.5f, false));
    g.fillRect(this->getLocalBounds());

    //[/UserPaint]
}

void ShadowDownwards::resized()
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

<JUCER_COMPONENT documentType="Component" className="ShadowDownwards" template="../../Template"
                 componentName="" parentClasses="public ShadowComponent" constructorParams="ShadowType type"
                 variableInitialisers="ShadowComponent(type)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="40">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill="linear: 0 0, 0 0R, 0=15000000, 1=0" hasStroke="0"/>
    <RECT pos="0 0 0M 50%" fill="linear: 0 0, 0 50%, 0=15000000, 1=0" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

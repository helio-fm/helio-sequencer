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

#include "LightShadowRightwards.h"

//[MiscUserDefs]
#include "ColourIDs.h"
//[/MiscUserDefs]

LightShadowRightwards::LightShadowRightwards()
{

    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    setSize (40, 400);

    //[Constructor]
    //[/Constructor]
}

LightShadowRightwards::~LightShadowRightwards()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void LightShadowRightwards::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setGradientFill (ColourGradient (Colour (0x15000000),
                                       0.0f, 0.0f,
                                       Colour (0x00000000),
                                       static_cast<float> (getWidth()), 0.0f,
                                       false));
    g.fillRect (0, 0, getWidth() - 0, getHeight() - 0);

    g.setGradientFill (ColourGradient (Colour (0x15000000),
                                       0.0f, 0.0f,
                                       Colour (0x00000000),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       false));
    g.fillRect (0, 0, proportionOfWidth (0.5000f), getHeight() - 0);

    //[UserPaint] Add your own custom painting code here..
    g.setColour(this->findColour(ColourIDs::Common::borderLineDark));
    g.drawVerticalLine(0, 0.f, float(this->getHeight()));
    //[/UserPaint]
}

void LightShadowRightwards::resized()
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

<JUCER_COMPONENT documentType="Component" className="LightShadowRightwards" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="40" initialHeight="400">
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="0 0 0M 0M" fill="linear: 0 0, 0R 0, 0=15000000, 1=0" hasStroke="0"/>
    <RECT pos="0 0 50% 0M" fill="linear: 0 0, 0C 0, 0=15000000, 1=0" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

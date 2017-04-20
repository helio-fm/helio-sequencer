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
//[/Headers]

#include "ShadowHorizontalFading.h"

//[MiscUserDefs]
//[/MiscUserDefs]

ShadowHorizontalFading::ShadowHorizontalFading()
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (320, 320);

    //[Constructor]
    //[/Constructor]
}

ShadowHorizontalFading::~ShadowHorizontalFading()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void ShadowHorizontalFading::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setGradientFill (ColourGradient (Colour (0x09000000),
                                       128.0f, 0.0f,
                                       Colour (0x00000000),
                                       0.0f, 0.0f,
                                       false));
    g.fillRect (0, 0, 128, getHeight() - 0);

    g.setGradientFill (ColourGradient (Colour (0x09000000),
                                       static_cast<float> (getWidth() - 128), 0.0f,
                                       Colour (0x00000000),
                                       static_cast<float> (getWidth()), 0.0f,
                                       false));
    g.fillRect (getWidth() - 128, 0, 128, getHeight() - 0);

    g.setColour (Colour (0x09000000));
    g.fillRect (128, 0, getWidth() - 256, getHeight() - 0);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ShadowHorizontalFading::resized()
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

<JUCER_COMPONENT documentType="Component" className="ShadowHorizontalFading" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="320" initialHeight="320">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 128 0M" fill="linear: 128 0, 0 0, 0=9000000, 1=0" hasStroke="0"/>
    <RECT pos="0Rr 0 128 0M" fill="linear: 128R 0, 0R 0, 0=9000000, 1=0"
          hasStroke="0"/>
    <RECT pos="128 0 256M 0M" fill="solid: 9000000" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "SeparatorHorizontalFading.h"

//[MiscUserDefs]
//[/MiscUserDefs]

SeparatorHorizontalFading::SeparatorHorizontalFading()
{

    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    this->setSize(32, 32);

    //[Constructor]
    //[/Constructor]
}

SeparatorHorizontalFading::~SeparatorHorizontalFading()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void SeparatorHorizontalFading::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    {
        int x = 0, y = 1, width = getWidth() - 0, height = 1;
        Colour fillColour1 = Colour (0x35000000), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> ((getWidth() / 2)) - 0.0f + x,
                                       0.0f - 1.0f + y,
                                       fillColour2,
                                       0.0f - 0.0f + x,
                                       0.0f - 1.0f + y,
                                       true));
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 2, width = getWidth() - 0, height = 1;
        Colour fillColour1 = Colour (0x15ffffff), fillColour2 = Colour (0x00ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> ((getWidth() / 2)) - 0.0f + x,
                                       0.0f - 2.0f + y,
                                       fillColour2,
                                       0.0f - 0.0f + x,
                                       0.0f - 2.0f + y,
                                       true));
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
#endif

    g.setGradientFill (ColourGradient (Colour (0x35000000),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       Colour (0x00000000),
                                       0.0f, 0.0f,
                                       true));
    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));

    g.setGradientFill (ColourGradient (Colour (0x15ffffff),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       Colour (0x00ffffff),
                                       0.0f, 0.0f,
                                       true));
    g.drawHorizontalLine(1, 0.f, float(this->getWidth()));

    //[/UserPaint]
}

void SeparatorHorizontalFading::resized()
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

<JUCER_COMPONENT documentType="Component" className="SeparatorHorizontalFading"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="32"
                 initialHeight="32">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 1 0M 1" fill=" radial: 0C 0, 0 0, 0=35000000, 1=0" hasStroke="0"/>
    <RECT pos="0 2 0M 1" fill=" radial: 0C 0, 0 0, 0=15ffffff, 1=ffffff"
          hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

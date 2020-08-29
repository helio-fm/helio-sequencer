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

#include "HeadlineItemArrow.h"

//[MiscUserDefs]
//[/MiscUserDefs]

HeadlineItemArrow::HeadlineItemArrow()
{

    //[UserPreSize]
    this->setOpaque(false);
    this->setPaintingIsUnclipped(true);
    this->setBufferedToImage(true);
    //[/UserPreSize]

    this->setSize(32, 32);

    //[Constructor]
    //[/Constructor]
}

HeadlineItemArrow::~HeadlineItemArrow()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void HeadlineItemArrow::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x77000000), strokeColour2 = Colour (0x11000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                             static_cast<float> (getWidth() - 2) - 0.0f + x,
                                             static_cast<float> (getHeight() - 2) - 0.0f + y,
                                             strokeColour2,
                                             static_cast<float> (getWidth() - 16) - 0.0f + x,
                                             2.0f - 0.0f + y,
                                             true));
        g.strokePath (internalPath1, PathStrokeType (1.000f), AffineTransform::translation(x, y));
    }

    {
        float x = 0, y = 0;
        Colour strokeColour1 = Colour (0x33ffffff), strokeColour2 = Colour (0x0bffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (strokeColour1,
                                             static_cast<float> (getWidth() - 3) - 0.0f + x,
                                             static_cast<float> (getHeight() - 2) - 0.0f + y,
                                             strokeColour2,
                                             static_cast<float> (getWidth() - 16) - 0.0f + x,
                                             0.0f - 0.0f + y,
                                             true));
        g.strokePath (internalPath2, PathStrokeType (0.500f), AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HeadlineItemArrow::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    internalPath1.clear();
    internalPath1.startNewSubPath (static_cast<float> (getWidth() - 16), 0.0f);
    internalPath1.lineTo (static_cast<float> (getWidth() - 1), static_cast<float> (getHeight()));

    internalPath2.clear();
    internalPath2.startNewSubPath (static_cast<float> (getWidth() - 17), 0.0f);
    internalPath2.lineTo (static_cast<float> (getWidth() - 2), static_cast<float> (getHeight()));

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineItemArrow" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="32" initialHeight="32">
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="1, mitered, butt"
          strokeColour=" radial: 2R 2R, 16R 2, 0=77000000, 1=11000000"
          nonZeroWinding="1">s 16R 0 l 1R 0R</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
          strokeColour=" radial: 3R 2R, 16R 0, 0=33ffffff, 1=bffffff" nonZeroWinding="1">s 17R 0 l 2R 0R</PATH>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif




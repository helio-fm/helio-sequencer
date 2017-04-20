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

#include "LogoImage.h"

//[MiscUserDefs]
#include "BinaryData.h"
//[/MiscUserDefs]

LogoImage::LogoImage()
{
    drawable1 = Drawable::createFromImageData (BinaryData::Logo_png, BinaryData::Logo_pngSize);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (400, 400);

    //[Constructor]
    //[/Constructor]
}

LogoImage::~LogoImage()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    drawable1 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void LogoImage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colours::black);
    jassert (drawable1 != 0);
    if (drawable1 != 0)
        drawable1->drawWithin (g, Rectangle<float> (0, 0, getWidth() - 0, getHeight() - 0),
                               RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LogoImage::resized()
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

<JUCER_COMPONENT documentType="Component" className="LogoImage" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="400" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <IMAGE pos="0 0 0M 0M" resource="BinaryData::Logo_png" opacity="1" mode="2"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

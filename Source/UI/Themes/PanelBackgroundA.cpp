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

#include "PanelBackgroundA.h"

//[MiscUserDefs]
#include "HelioTheme.h"
#include "ColourIDs.h"
#include "Icons.h"
//[/MiscUserDefs]

PanelBackgroundA::PanelBackgroundA()
{

    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    this->setOpaque(true);
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    //[/Constructor]
}

PanelBackgroundA::~PanelBackgroundA()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void PanelBackgroundA::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    // Used in loading page, where caches are not available:
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour = Colour (0xff48358c);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = this->findColour(ColourIDs::BackgroundA::fill);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour fillColour1 = Colour (0x1e636363), fillColour2 = Colour (0x00000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (ColourGradient (fillColour1,
                                       static_cast<float> (proportionOfWidth (0.7500f)) - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.6500f)) - 0.0f + y,
                                       fillColour2,
                                       0.0f - 0.0f + x,
                                       static_cast<float> (proportionOfHeight (0.0000f)) - 0.0f + y,
                                       false));
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    HelioTheme::drawNoiseWithin(this->getLocalBounds().toFloat(), this, g, 1.f);
    //[/UserPaint]
}

void PanelBackgroundA::resized()
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

<JUCER_COMPONENT documentType="Component" className="PanelBackgroundA" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="0"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill="solid: ff48358c" hasStroke="0"/>
    <RECT pos="0 0 0M 0M" fill="linear: 75% 65%, 0 0%, 0=1e636363, 1=0"
          hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

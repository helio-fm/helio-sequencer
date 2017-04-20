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

#include "TreeItemMarkerCompact.h"

//[MiscUserDefs]
//[/MiscUserDefs]

TreeItemMarkerCompact::TreeItemMarkerCompact()
{

    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (48, 32);

    //[Constructor]
    //[/Constructor]
}

TreeItemMarkerCompact::~TreeItemMarkerCompact()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void TreeItemMarkerCompact::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
#if 0
    //[/UserPrePaint]

    g.setColour (Colours::white);
    g.drawEllipse (12.0f, 5.0f, static_cast<float> (getWidth() - 24), static_cast<float> (getHeight() - 10), 0.500f);

    //[UserPaint] Add your own custom painting code here..
#endif

    g.setColour (Colours::white.withAlpha(0.35f));

    const float yMargin = 5.5f;
    const float circleSize = this->getHeight() - (yMargin * 2.f);
    const float xMargin = ((this->getWidth() / 2) - (circleSize / 2.f));
    g.drawEllipse(xMargin, yMargin, circleSize, circleSize, 0.5f);

    //[/UserPaint]
}

void TreeItemMarkerCompact::resized()
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

<JUCER_COMPONENT documentType="Component" className="TreeItemMarkerCompact" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="48" initialHeight="32">
  <BACKGROUND backgroundColour="0">
    <ELLIPSE pos="12 5 24M 10M" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
             strokeColour="solid: ffffffff"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

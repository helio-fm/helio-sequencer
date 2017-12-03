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

#include "SoundProbeIndicator.h"

//[MiscUserDefs]
#include "IconComponent.h"
//[/MiscUserDefs]

SoundProbeIndicator::SoundProbeIndicator()
    : absPosition(0)
{

    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    this->setAlwaysOnTop(true);
    //[/UserPreSize]

    setSize (3, 256);

    //[Constructor]
    //[/Constructor]
}

SoundProbeIndicator::~SoundProbeIndicator()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 100, true, 0.0, 0.0);
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void SoundProbeIndicator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x1effffff));
    g.fillRect (0, 0, getWidth() - 0, getHeight() - 0);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SoundProbeIndicator::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SoundProbeIndicator::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updateBounds();
    //[/UserCode_parentHierarchyChanged]
}

void SoundProbeIndicator::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updateBounds();
    //[/UserCode_parentSizeChanged]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SoundProbeIndicator" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="absPosition(0)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="3"
                 initialHeight="256">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 0 0M 0M" fill="solid: 1effffff" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

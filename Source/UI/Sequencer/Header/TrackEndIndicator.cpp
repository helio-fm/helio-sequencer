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

#include "TrackEndIndicator.h"

//[MiscUserDefs]
#include "IconComponent.h"
//[/MiscUserDefs]

TrackEndIndicator::TrackEndIndicator()
    : absPosition(0)
{
    addAndMakeVisible (shadow = new ShadowRightwards(Normal));

    //[UserPreSize]
    this->shadow->setAlpha(0.7f);
    this->setInterceptsMouseClicks(false, false);
    this->setAlwaysOnTop(true);
    //[/UserPreSize]

    setSize (40, 256);

    //[Constructor]
    //[/Constructor]
}

TrackEndIndicator::~TrackEndIndicator()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TrackEndIndicator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
//#if ! HELIO_MOBILE
    //[/UserPrePaint]

    g.fillAll (Colour (0x16000000));

    //[UserPaint] Add your own custom painting code here..
//#endif
    //[/UserPaint]
}

void TrackEndIndicator::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shadow->setBounds (-2, 0, 12, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TrackEndIndicator::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updateBounds();
    //[/UserCode_parentHierarchyChanged]
}

void TrackEndIndicator::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    //this->updateBounds();
    //[/UserCode_parentSizeChanged]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TrackEndIndicator" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="absPosition(0)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="40"
                 initialHeight="256">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="16000000"/>
  <JUCERCOMP name="" id="960dc35b494ac9d2" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="-2 0 12 0M" sourceFile="../../Themes/ShadowRightwards.cpp"
             constructorParams="Normal"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

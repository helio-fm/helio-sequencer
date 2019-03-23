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

#include "HeaderSelectionIndicator.h"

//[MiscUserDefs]
#include "IconComponent.h"
//[/MiscUserDefs]

HeaderSelectionIndicator::HeaderSelectionIndicator()
    : startAbsPosition(0.f),
      endAbsPosition(0.f)
{

    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->setAlwaysOnTop(true);
    //[/UserPreSize]

    setSize (128, 16);

    //[Constructor]
    this->setAlpha(0.f);
    this->startTimerHz(60);
    //[/Constructor]
}

HeaderSelectionIndicator::~HeaderSelectionIndicator()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 50, true, 0.0, 0.0);
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void HeaderSelectionIndicator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x1a000000));
    g.fillRoundedRectangle (0.0f, static_cast<float> (getHeight() - 6 - (20 / 2)), static_cast<float> (getWidth() - 0), 20.0f, 7.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HeaderSelectionIndicator::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HeaderSelectionIndicator::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updateBounds();
    //[/UserCode_parentHierarchyChanged]
}

void HeaderSelectionIndicator::parentSizeChanged()
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

<JUCER_COMPONENT documentType="Component" className="HeaderSelectionIndicator"
                 template="../../../Template" componentName="" parentClasses="public Component, private Timer"
                 constructorParams="" variableInitialisers="startAbsPosition(0.f),&#10;endAbsPosition(0.f)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="16">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 6Rc 0M 20" cornerSize="7" fill="solid: 1a000000" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

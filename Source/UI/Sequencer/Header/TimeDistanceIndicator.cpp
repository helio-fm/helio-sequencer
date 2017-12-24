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

#include "TimeDistanceIndicator.h"

//[MiscUserDefs]
#include "IconComponent.h"
//[/MiscUserDefs]

TimeDistanceIndicator::TimeDistanceIndicator()
    : startAbsPosition(0.f),
      endAbsPosition(0.f)
{
    addAndMakeVisible (timeLabel = new Label (String(),
                                              TRANS("...")));
    timeLabel->setFont (Font (21.00f, Font::plain).withTypefaceStyle ("Regular"));
    timeLabel->setJustificationType (Justification::centred);
    timeLabel->setEditable (false, false, false);


    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->setAlwaysOnTop(true);
    //[/UserPreSize]

    setSize (128, 32);

    //[Constructor]
    this->setAlpha(0.f);
    this->startTimerHz(60);
    //[/Constructor]
}

TimeDistanceIndicator::~TimeDistanceIndicator()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 100, true, 0.0, 0.0);
    //[/Destructor_pre]

    timeLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TimeDistanceIndicator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 3.0f, y = 0.0f, width = static_cast<float> (getWidth() - 6), height = 2.0f;
        Colour fillColour = Colour (0x3dffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 1.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TimeDistanceIndicator::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    timeLabel->setBounds ((getWidth() / 2) - ((getWidth() - 0) / 2), 4, getWidth() - 0, 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TimeDistanceIndicator::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updateBounds();
    //[/UserCode_parentHierarchyChanged]
}

void TimeDistanceIndicator::parentSizeChanged()
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

<JUCER_COMPONENT documentType="Component" className="TimeDistanceIndicator" template="../../../Template"
                 componentName="" parentClasses="public Component, private Timer"
                 constructorParams="" variableInitialisers="startAbsPosition(0.f),&#10;endAbsPosition(0.f)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="32">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="3 0 6M 2" cornerSize="1" fill="solid: 3dffffff" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="" id="76c238702c82d339" memberName="timeLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 4 0M 20" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

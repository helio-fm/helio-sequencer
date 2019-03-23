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

#include "SuccessTooltip.h"

//[MiscUserDefs]
#include "Icons.h"
#define ONSCREEN_TIMS_MS 1000
#define FADEOUT_TIMS_MS 350
//[/MiscUserDefs]

SuccessTooltip::SuccessTooltip()
{
    addAndMakeVisible (imageRange = new Component());


    //[UserPreSize]
    this->iconShape = Icons::getPathByName(Icons::success);
    //[/UserPreSize]

    setSize (96, 96);

    //[Constructor]
    this->startTimer(ONSCREEN_TIMS_MS);
    //[/Constructor]
}

SuccessTooltip::~SuccessTooltip()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    imageRange = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SuccessTooltip::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0xa0000000));
    g.fillRoundedRectangle (static_cast<float> ((getWidth() / 2) - (96 / 2)), static_cast<float> ((getHeight() / 2) - (96 / 2)), 96.0f, 96.0f, 15.000f);

    //[UserPaint] Add your own custom painting code here..
    g.setColour(Colours::white.withAlpha(0.7f));
    g.fillPath(this->iconShape, this->iconShape.getTransformToScaleToFit(this->imageRange->getBounds().toFloat(), true, Justification::centred));
    //[/UserPaint]
}

void SuccessTooltip::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    imageRange->setBounds ((getWidth() / 2) - (48 / 2), (getHeight() / 2) - (48 / 2), 48, 48);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SuccessTooltip::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
    //[/UserCode_parentHierarchyChanged]
}


//[MiscUserCode]
void SuccessTooltip::timerCallback()
{
    this->stopTimer();
    this->dismiss();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SuccessTooltip" template="../../Template"
                 componentName="" parentClasses="public CenteredTooltipComponent, private Timer"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="96"
                 initialHeight="96">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0Cc 0Cc 96 96" cornerSize="15" fill="solid: a0000000" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="1b627fc3e6ac501f" memberName="imageRange" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 48 48" class="Component"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

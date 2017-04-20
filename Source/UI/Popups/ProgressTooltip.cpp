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

#include "ProgressTooltip.h"

//[MiscUserDefs]
const String ProgressTooltip::componentId = "<ProgressTooltip::componentId>";
#define PROGRESS_TOOLTIP_FADEOUT_TIMS_MS 350
//[/MiscUserDefs]

ProgressTooltip::ProgressTooltip()
{
    addAndMakeVisible (progressIndicator = new ProgressIndicator());


    //[UserPreSize]
    this->setComponentID(ProgressTooltip::componentId);
    this->progressIndicator->startAnimating();
    //[/UserPreSize]

    setSize (96, 96);

    //[Constructor]
    //[/Constructor]
}

ProgressTooltip::~ProgressTooltip()
{
    //[Destructor_pre]
    FadingDialog::fadeOut();
    //Desktop::getInstance().getAnimator().fadeOut(this, PROGRESS_TOOLTIP_FADEOUT_TIMS_MS);
    //[/Destructor_pre]

    progressIndicator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ProgressTooltip::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0xa0000000));
    g.fillRoundedRectangle (static_cast<float> ((getWidth() / 2) - (96 / 2)), static_cast<float> ((getHeight() / 2) - (96 / 2)), 96.0f, 96.0f, 15.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ProgressTooltip::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    progressIndicator->setBounds ((getWidth() / 2) - (64 / 2), (getHeight() / 2) - (64 / 2), 64, 64);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ProgressTooltip::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
    //[/UserCode_parentHierarchyChanged]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ProgressTooltip" template="../../Template"
                 componentName="" parentClasses="public CenteredTooltipComponent"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="96"
                 initialHeight="96">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0Cc 0Cc 96 96" cornerSize="15" fill="solid: a0000000" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="c8b225da767e9a4d" memberName="progressIndicator"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0Cc 64 64" class="ProgressIndicator"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

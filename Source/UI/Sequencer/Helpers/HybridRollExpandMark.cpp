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

#include "HybridRollExpandMark.h"

//[MiscUserDefs]
#include "HybridRoll.h"
//[/MiscUserDefs]

HybridRollExpandMark::HybridRollExpandMark(HybridRoll &parentRoll, float targetBar, int numBarsToTake)
    : roll(parentRoll),
      bar(targetBar),
      numBars(numBarsToTake),
      alpha(1.f)
{
    this->plusImage.reset(new IconComponent(Icons::expand));
    this->addAndMakeVisible(plusImage.get());


    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    this->setSize(256, 48);

    //[Constructor]
    this->startTimerHz(60);
    //[/Constructor]
}

HybridRollExpandMark::~HybridRollExpandMark()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    plusImage = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HybridRollExpandMark::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0x0fffffff));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HybridRollExpandMark::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    plusImage->setBounds((getWidth() / 2) - (16 / 2), (getHeight() / 2) - (16 / 2), 16, 16);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HybridRollExpandMark::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void HybridRollExpandMark::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentSizeChanged]
}


//[MiscUserCode]
void HybridRollExpandMark::updatePosition()
{
    const float barOffset = this->bar - this->roll.getFirstBar();
    const int xOffset = int(barOffset * this->roll.getBarWidth());
    const int newWidth = int(this->roll.getBarWidth() * this->numBars);
    this->setBounds(xOffset, 0, newWidth, this->getParentHeight());
}

void HybridRollExpandMark::timerCallback()
{
    this->alpha -= 0.015f;

    if (this->alpha <= 0)
    {
        delete this;
    }
    else
    {
        this->setAlpha(this->alpha);
    }
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HybridRollExpandMark" template="../../../Template"
                 componentName="" parentClasses="public Component, private Timer"
                 constructorParams="HybridRoll &amp;parentRoll, float targetBar, int numBarsToTake"
                 variableInitialisers="roll(parentRoll),&#10;bar(targetBar),&#10;numBars(numBarsToTake),&#10;alpha(1.f)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="256" initialHeight="48">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="fffffff"/>
  <GENERICCOMPONENT name="" id="79f90a69d0b95011" memberName="plusImage" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 16 16" class="IconComponent"
                    params="Icons::expand"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

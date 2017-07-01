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

#include "WipeSpaceHelper.h"

//[MiscUserDefs]
#include "HybridRoll.h"
//[/MiscUserDefs]

WipeSpaceHelper::WipeSpaceHelper(HybridRoll &parentRoll)
    : roll(parentRoll),
      startBeat(0.f),
      endBeat(0.f)
{
    addAndMakeVisible (shadowR = new ShadowLeftwards());
    addAndMakeVisible (shadowL = new ShadowRightwards());

    //[UserPreSize]
    this->setAlpha(0.f);
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (128, 256);

    //[Constructor]
    //[/Constructor]
}

WipeSpaceHelper::~WipeSpaceHelper()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 150, true, 0.f, 0.f);
    //[/Destructor_pre]

    shadowR = nullptr;
    shadowL = nullptr;

    //[Destructor]
    //[/Destructor]
}

void WipeSpaceHelper::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0x25000000));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void WipeSpaceHelper::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shadowR->setBounds (getWidth() - -3 - 24, 0, 24, getHeight() - 0);
    shadowL->setBounds (-3, 0, 24, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
void WipeSpaceHelper::rebound()
{
    const int xStart = this->roll.getXPositionByBeat(this->getLeftMostBeat());
    const int xEnd = this->roll.getXPositionByBeat(this->getRightMostBeat());
    const int xDelta = xEnd - xStart;
    const int newWidth = jmax(4, xDelta);

    if (this->getParentComponent() != nullptr)
    {
        this->setBounds(this->getParentComponent()->getLocalBounds().withX((xDelta == 0) ? (xStart - 1) : xStart).withWidth(newWidth));
    }
}

void WipeSpaceHelper::setStartBeat(float beat)
{
    this->startBeat = beat;
    this->rebound();
}

void WipeSpaceHelper::setEndBeat(float beat)
{
    this->endBeat = beat;
    this->rebound();
}

float WipeSpaceHelper::getLeftMostBeat() const
{
    return jmin(this->startBeat, this->endBeat);
}

float WipeSpaceHelper::getRightMostBeat() const
{
    return jmax(this->startBeat, this->endBeat);
}

bool WipeSpaceHelper::isInverted() const
{
    return (this->startBeat > this->endBeat);
}

void WipeSpaceHelper::snapWidth()
{
    this->startBeat = this->endBeat;
    this->rebound();
}

void WipeSpaceHelper::fadeIn()
{
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 1.f, 150, false, 0.f, 0.f);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="WipeSpaceHelper" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="HybridRoll &amp;parentRoll"
                 variableInitialisers="roll(parentRoll),&#10;startBeat(0.f),&#10;endBeat(0.f)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="256">
  <BACKGROUND backgroundColour="25000000"/>
  <JUCERCOMP name="" id="123c74cf94f9cec1" memberName="shadowR" virtualName=""
             explicitFocusOrder="0" pos="-3Rr 0 24 0M" sourceFile="../../Themes/ShadowLeftwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="f606d7e3c14830e" memberName="shadowL" virtualName=""
             explicitFocusOrder="0" pos="-3 0 24 0M" sourceFile="../../Themes/ShadowRightwards.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

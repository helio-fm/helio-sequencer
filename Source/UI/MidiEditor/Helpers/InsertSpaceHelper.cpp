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

#include "InsertSpaceHelper.h"

//[MiscUserDefs]
#include "HybridRoll.h"
//[/MiscUserDefs]

InsertSpaceHelper::InsertSpaceHelper(HybridRoll &parentRoll)
    : roll(parentRoll),
      startBeat(0.f),
      endBeat(0.f),
      endBeatAnchor(0.f)
{
    addAndMakeVisible (shadowL = new ShadowLeftwards());
    addAndMakeVisible (shadowR = new ShadowRightwards());

    //[UserPreSize]
    this->setAlpha(0.f);
    this->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (128, 256);

    //[Constructor]
    //[/Constructor]
}

InsertSpaceHelper::~InsertSpaceHelper()
{
    //[Destructor_pre]
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 0.f, 150, true, 0.f, 0.f);
    //[/Destructor_pre]

    shadowL = nullptr;
    shadowR = nullptr;

    //[Destructor]
    //[/Destructor]
}

void InsertSpaceHelper::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x0bffffff));
    g.fillRect (23, 0, getWidth() - 46, getHeight() - 0);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void InsertSpaceHelper::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shadowL->setBounds (0, 0, 25, getHeight() - 0);
    shadowR->setBounds (getWidth() - 24, 0, 24, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
void InsertSpaceHelper::rebound()
{
    const int xStart = this->roll.getXPositionByBeat(this->getLeftMostBeat()) - 22;
    const int xEnd = this->roll.getXPositionByBeat(this->getRightMostBeat());
    const int xDelta = xEnd - xStart;
    const int newWidth = (xDelta + 22);

    if (this->getParentComponent() != nullptr)
    {
        this->setBounds(this->getParentComponent()->getLocalBounds().withX((xDelta == 0) ? (xStart - 1) : xStart).withWidth(newWidth));
    }
}

void InsertSpaceHelper::setStartBeat(float beat)
{
    this->startBeat = beat;
    this->rebound();
}

void InsertSpaceHelper::setEndBeat(float beat)
{
    this->endBeat = beat;
    this->rebound();
}

float InsertSpaceHelper::getLeftMostBeat() const
{
    return jmin(this->startBeat, this->endBeat);
}

float InsertSpaceHelper::getRightMostBeat() const
{
    return jmax(this->startBeat, this->endBeat);
}

bool InsertSpaceHelper::isInverted() const
{
    return (this->startBeat > this->endBeat);
}

bool InsertSpaceHelper::shouldCheckpoint() const
{
    return this->needsCheckpoint;
}

void InsertSpaceHelper::setNeedsCheckpoint(bool val)
{
    this->needsCheckpoint = val;
}

void InsertSpaceHelper::snapWidth()
{
    this->startBeat = this->endBeat;
    this->rebound();
}

float InsertSpaceHelper::getDragDelta() const
{
    return (this->endBeat - this->endBeatAnchor);
}

void InsertSpaceHelper::resetDragDelta()
{
    this->endBeatAnchor = this->endBeat;
}

void InsertSpaceHelper::fadeIn()
{
    Desktop::getInstance().getAnimator().animateComponent(this, this->getBounds(), 1.f, 150, false, 0.f, 0.f);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="InsertSpaceHelper" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="HybridRoll &amp;parentRoll"
                 variableInitialisers="roll(parentRoll),&#10;startBeat(0.f),&#10;endBeat(0.f),&#10;endBeatAnchor(0.f)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="256">
  <BACKGROUND backgroundColour="0">
    <RECT pos="23 0 46M 0M" fill="solid: bffffff" hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="123c74cf94f9cec1" memberName="shadowL" virtualName=""
             explicitFocusOrder="0" pos="0 0 25 0M" sourceFile="../../Themes/ShadowLeftwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="f606d7e3c14830e" memberName="shadowR" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 24 0M" sourceFile="../../Themes/ShadowRightwards.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

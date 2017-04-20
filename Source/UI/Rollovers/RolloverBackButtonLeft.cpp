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

#include "RolloverBackButtonLeft.h"

//[MiscUserDefs]
#include "TreePanel.h"
//[/MiscUserDefs]

RolloverBackButtonLeft::RolloverBackButtonLeft()
{
    addAndMakeVisible (menuIcon = new IconButton (Icons::close));


    //[UserPreSize]
    this->setInterceptsMouseClicks(true, false);
    //[/UserPreSize]

    setSize (32, 32);

    //[Constructor]
    //[/Constructor]
}

RolloverBackButtonLeft::~RolloverBackButtonLeft()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    menuIcon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RolloverBackButtonLeft::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RolloverBackButtonLeft::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    menuIcon->setBounds (5, 5, getWidth() - 10, getHeight() - 10);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RolloverBackButtonLeft::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->getParentComponent()->postCommandMessage(CommandIDs::HideRollover);
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RolloverBackButtonLeft" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="32" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="menuIcon" virtualName=""
                    explicitFocusOrder="0" pos="5 5 10M 10M" class="IconButton" params="Icons::close"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

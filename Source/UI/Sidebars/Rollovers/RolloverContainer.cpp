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

#include "RolloverContainer.h"

//[MiscUserDefs]
//[/MiscUserDefs]

RolloverContainer::RolloverContainer(Component *newHeaderComponent, Component *newContentComponent)
{
    addAndMakeVisible (background = new PanelBackgroundC());
    addAndMakeVisible (rolloverContent = new Component());

    addAndMakeVisible (headLine = new SeparatorHorizontalReversed());
    addAndMakeVisible (headShadow = new LighterShadowDownwards());
    addAndMakeVisible (gradient1 = new GradientVerticalReversed());
    addAndMakeVisible (rolloverHeader = new Component());


    //[UserPreSize]
    this->rolloverHeader = newHeaderComponent;
    this->addAndMakeVisible(this->rolloverHeader);

    this->rolloverContent = newContentComponent;
    this->addAndMakeVisible(this->rolloverContent);
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    //[/Constructor]
}

RolloverContainer::~RolloverContainer()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    rolloverContent = nullptr;
    headLine = nullptr;
    headShadow = nullptr;
    gradient1 = nullptr;
    rolloverHeader = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RolloverContainer::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RolloverContainer::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    rolloverContent->setBounds (0, 49, getWidth() - 0, getHeight() - 50);
    headLine->setBounds (0, 47, getWidth() - 0, 2);
    headShadow->setBounds (0, 48, getWidth() - 0, 6);
    gradient1->setBounds (-50, 0, getWidth() - -100, 47);
    rolloverHeader->setBounds (0, 0, getWidth() - 0, 48);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RolloverContainer::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    // forward the message to tree panel
    this->getParentComponent()->postCommandMessage(commandId);
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RolloverContainer" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams="Component *newHeaderComponent, Component *newContentComponent"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffffff"/>
  <JUCERCOMP name="" id="19597a6a5daad55d" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="dab1f297d3cc7439" memberName="rolloverContent" virtualName=""
                    explicitFocusOrder="0" pos="0 49 0M 50M" class="Component" params=""/>
  <JUCERCOMP name="" id="28ce45d9e84b729c" memberName="headLine" virtualName=""
             explicitFocusOrder="0" pos="0 47 0M 2" sourceFile="../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1d398dc12e2047bd" memberName="headShadow" virtualName=""
             explicitFocusOrder="0" pos="0 48 0M 6" sourceFile="../Themes/LighterShadowDownwards.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="f09d886c97d1c017" memberName="gradient1" virtualName=""
             explicitFocusOrder="0" pos="-50 0 -100M 47" sourceFile="../Themes/GradientVerticalReversed.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="faec82bf5da2e1" memberName="rolloverHeader" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 48" class="Component" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

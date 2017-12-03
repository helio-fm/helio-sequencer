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

#include "TreeItemMenuButton.h"

//[MiscUserDefs]
#include "TreeItemComponent.h"
#include "IconComponent.h"
#include "Icons.h"
#include "TreeItem.h"
#include "CommandIDs.h"

#if HELIO_DESKTOP
#   define TREE_MENU_ICON_HEIGHT (20)
#elif HELIO_MOBILE
#   define TREE_MENU_ICON_HEIGHT (24)
#endif

//[/MiscUserDefs]

TreeItemMenuButton::TreeItemMenuButton()
    : HighlightedComponent()
{
    addAndMakeVisible (menuIcon = new IconComponent (Icons::right));


    //[UserPreSize]
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (32, 32);

    //[Constructor]
    //[/Constructor]
}

TreeItemMenuButton::~TreeItemMenuButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    menuIcon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TreeItemMenuButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TreeItemMenuButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    menuIcon->setBounds (5, 5, getWidth() - 10, getHeight() - 10);
    //[UserResized] Add your own custom resize handling here..
    menuIcon->setSize(TREE_MENU_ICON_HEIGHT, TREE_MENU_ICON_HEIGHT);
    menuIcon->setCentrePosition(this->getWidth() / 2, this->getHeight() / 2);
    //[/UserResized]
}

void TreeItemMenuButton::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    //[/UserCode_parentHierarchyChanged]
}

void TreeItemMenuButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    //Logger::writeToLog("TreeItemMenuButton::mouseDown");
    this->getParentComponent()->postCommandMessage(CommandIDs::MenuButtonPressed);
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TreeItemMenuButton" template="../../Template"
                 componentName="" parentClasses="public HighlightedComponent"
                 constructorParams="" variableInitialisers="HighlightedComponent()"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="32" initialHeight="32">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="menuIcon" virtualName=""
                    explicitFocusOrder="0" pos="5 5 10M 10M" class="IconComponent"
                    params="Icons::right"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "RootTreeItemPanelCompact.h"

//[MiscUserDefs]
#include "MidiRollCommandPanel.h"
#include "HelioTheme.h"
#include "App.h"
#include "AuthorizationManager.h"
#include "MainLayout.h"
#include "WorkspaceMenu.h"
#include "HelioCallout.h"
#include "TreePanel.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

RootTreeItemPanelCompact::RootTreeItemPanelCompact()
{
    addAndMakeVisible (shade = new ShadeLight());
    addAndMakeVisible (workspaceIcon = new IconComponent (Icons::workspace));


    //[UserPreSize]
    this->shade->setVisible(false);
    this->setInterceptsMouseClicks(true, false);
    //[/UserPreSize]

    setSize (48, 48);

    //[Constructor]
    this->workspaceIcon->setAlpha(0.85f);
    //[/Constructor]
}

RootTreeItemPanelCompact::~RootTreeItemPanelCompact()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    shade = nullptr;
    workspaceIcon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RootTreeItemPanelCompact::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RootTreeItemPanelCompact::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shade->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    workspaceIcon->setBounds (8, (getHeight() / 2) - (32 / 2), 32, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RootTreeItemPanelCompact::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::SelectRootItemPanel)
    {
        this->fader.fadeIn(this->shade, 200);
    }
    else if (commandId == CommandIDs::DeselectRootItemPanel)
    {
        this->fader.fadeOut(this->shade, 200);
    }
    //[/UserCode_handleCommandMessage]
}

void RootTreeItemPanelCompact::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    this->fader.softFadeIn(this->workspaceIcon, 150);
    //[/UserCode_mouseEnter]
}

void RootTreeItemPanelCompact::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    this->fader.softFadeOut(0.75f, this->workspaceIcon, 200);
    //[/UserCode_mouseExit]
}

void RootTreeItemPanelCompact::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->getParentComponent()->postCommandMessage(CommandIDs::RootTreeItemPressed);
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RootTreeItemPanelCompact"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="48"
                 initialHeight="48">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
    <METHOD name="mouseEnter (const MouseEvent&amp; e)"/>
    <METHOD name="mouseExit (const MouseEvent&amp; e)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="1eb024ea37337815" memberName="shade" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/ShadeLight.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="workspaceIcon" virtualName=""
                    explicitFocusOrder="0" pos="8 0Cc 32 32" class="IconComponent"
                    params="Icons::workspace"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

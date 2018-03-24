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
#include "HelioTheme.h"
//[/Headers]

#include "WorkspacePage.h"

//[MiscUserDefs]
#include "HelioCallout.h"
#include "RootTreeItem.h"
#include "MainLayout.h"
#include "AudioSettings.h"
#include "ThemeSettings.h"
#include "OpenGLSettings.h"
#include "LogComponent.h"
#include "ComponentsList.h"
#include "SettingsPage.h"
#include "SerializationKeys.h"
#include "WorkspaceMenu.h"
#include "LogoFader.h"
#include "App.h"
#include "SettingsTreeItem.h"
#include "Workspace.h"
#include "IconComponent.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

WorkspacePage::WorkspacePage(MainLayout &workspaceRef)
    : workspace(workspaceRef)
{
    addAndMakeVisible (background = new PanelBackgroundA());
    addAndMakeVisible (menuButton = new MenuButton());
    addAndMakeVisible (logoImage = new LogoFader (true));

    addAndMakeVisible (shadow = new LightShadowRightwards());
    addAndMakeVisible (settingsButton = new TextButton (String()));
    settingsButton->setButtonText (TRANS("Settings"));
    settingsButton->addListener (this);

    addAndMakeVisible (component = new WorkspaceMenu (&App::Workspace()));

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    this->setOpaque(true);
    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);
    //[/Constructor]
}

WorkspacePage::~WorkspacePage()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    menuButton = nullptr;
    logoImage = nullptr;
    shadow = nullptr;
    settingsButton = nullptr;
    component = nullptr;

    //[Destructor]
    //[/Destructor]
}

void WorkspacePage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void WorkspacePage::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, -127, getWidth() - 0, getHeight() - -128);
    menuButton->setBounds (-200, -200, 128, 128);
    logoImage->setBounds ((getWidth() / 2) - (400 / 2), proportionOfHeight (0.2751f) - (400 / 2), 400, 400);
    shadow->setBounds (0, 0, 5, getHeight() - 0);
    settingsButton->setBounds (-100, -100, 56, 48);
    component->setBounds ((getWidth() / 2) - (450 / 2), (getHeight() / 2) + 40, 450, proportionOfHeight (0.4003f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void WorkspacePage::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == settingsButton)
    {
        //[UserButtonCode_settingsButton] -- add your button handler code here..
        if (TreeItem *settings =
            App::Workspace().getTreeRoot()->findChildOfType<SettingsTreeItem>())
        {
            settings->setSelected(true, true);
        }
        //[/UserButtonCode_settingsButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void WorkspacePage::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    if (this->isVisible())
    {
        #if HELIO_DESKTOP
        this->logoImage->startFade();
        #endif
    }
    //[/UserCode_visibilityChanged]
}

void WorkspacePage::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::MenuButtonPressed)
    {
        auto cs = new WorkspaceMenu(&App::Workspace());
        HelioCallout::emit(cs, this->menuButton);
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="WorkspacePage" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="MainLayout &amp;workspaceRef"
                 variableInitialisers="workspace(workspaceRef)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.660" fixedSize="0"
                 initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <TEXT pos="0Cc 5.5% 552 60" fill="solid: ffffff" hasStroke="0" text="Helio Workstation"
          fontname="Georgia" fontsize="55" kerning="0" bold="1" italic="0"
          justification="36" typefaceStyle="Bold"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="9e61167b79cef28c" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 -127 0M -128M" sourceFile="../../Themes/PanelBackgroundA.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="2ce00deefdf277e6" memberName="menuButton" virtualName=""
             explicitFocusOrder="0" pos="-200 -200 128 128" sourceFile="../../Common/MenuButton.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="ea1b592642055bdc" memberName="logoImage" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 27.512%c 400 400" class="LogoFader"
                    params="true"/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 0 5 0M" sourceFile="../../Themes/LightShadowRightwards.cpp"
             constructorParams=""/>
  <TEXTBUTTON name="" id="ed7621895f01485c" memberName="settingsButton" virtualName=""
              explicitFocusOrder="0" pos="-100 -100 56 48" buttonText="Settings"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="25591a755b533290" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="0Cc 40C 450 40.033%" sourceFile="Menu/WorkspaceMenu.cpp"
             constructorParams="&amp;App::Workspace()"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

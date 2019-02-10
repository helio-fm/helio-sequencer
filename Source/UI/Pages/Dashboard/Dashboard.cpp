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

#include "Dashboard.h"

//[MiscUserDefs]
#include "RootNode.h"
#include "SettingsNode.h"
#include "MainLayout.h"
#include "SerializationKeys.h"
#include "DashboardMenu.h"
#include "LogoFader.h"
#include "Workspace.h"
#include "SessionService.h"
#include "IconComponent.h"
#include "UserProfile.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

Dashboard::Dashboard(MainLayout &workspaceRef)
    : workspace(workspaceRef)
{
    this->skew.reset(new SeparatorVerticalSkew());
    this->addAndMakeVisible(skew.get());
    this->backgroundB.reset(new PanelBackgroundB());
    this->addAndMakeVisible(backgroundB.get());
    this->backgroundA.reset(new PanelBackgroundA());
    this->addAndMakeVisible(backgroundA.get());
    this->logoImage.reset(new LogoFader());
    this->addAndMakeVisible(logoImage.get());

    logoImage->setBounds(40, 32, 280, 280);

    this->projectsList.reset(new DashboardMenu(&App::Workspace()));
    this->addAndMakeVisible(projectsList.get());
    this->openProjectButton.reset(new OpenProjectButton());
    this->addAndMakeVisible(openProjectButton.get());
    openProjectButton->setBounds(400, 60, 271, 32);

    this->createProjectButton.reset(new CreateProjectButton());
    this->addAndMakeVisible(createProjectButton.get());
    createProjectButton->setBounds(400, 20, 271, 32);

    this->separator2.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator2.get());
    separator2->setBounds(264, 104, 488, 3);

    this->loginButton.reset(new LoginButton());
    this->addAndMakeVisible(loginButton.get());
    loginButton->setBounds(400, 120, 272, 32);

    this->userProfile.reset(new UserProfileComponent());
    this->addAndMakeVisible(userProfile.get());
    userProfile->setBounds(400, 120, 272, 32);

    this->updatesInfo.reset(new UpdatesInfoComponent());
    this->addAndMakeVisible(updatesInfo.get());
    updatesInfo->setBounds(88, 352, 184, 128);


    //[UserPreSize]
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);
    this->setOpaque(true);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    this->updateProfileViews();
    App::Workspace().getUserProfile().addChangeListener(this);
    //[/Constructor]
}

Dashboard::~Dashboard()
{
    //[Destructor_pre]
    App::Workspace().getUserProfile().removeChangeListener(this);
    //[/Destructor_pre]

    skew = nullptr;
    backgroundB = nullptr;
    backgroundA = nullptr;
    logoImage = nullptr;
    projectsList = nullptr;
    openProjectButton = nullptr;
    createProjectButton = nullptr;
    separator2 = nullptr;
    loginButton = nullptr;
    userProfile = nullptr;
    updatesInfo = nullptr;

    //[Destructor]
    //[/Destructor]
}

void Dashboard::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Dashboard::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    skew->setBounds(0 + 320, 0, 64, getHeight() - 0);
    backgroundB->setBounds(getWidth() - (getWidth() - 384), 0, getWidth() - 384, getHeight() - 0);
    backgroundA->setBounds(0, 0, 320, getHeight() - 0);
    projectsList->setBounds(getWidth() - 10 - 376, getHeight() - 10 - (getHeight() - 20), 376, getHeight() - 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Dashboard::visibilityChanged()
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


//[MiscUserCode]
void Dashboard::changeListenerCallback(ChangeBroadcaster *source)
{
    // Listens to user profile changes:
    this->updateProfileViews();
}

void Dashboard::updateProfileViews()
{
    const bool loggedIn = App::Workspace().getUserProfile().isLoggedIn();
    this->loginButton->setVisible(!loggedIn);
    this->userProfile->setVisible(loggedIn);
    if (loggedIn)
    {
        this->userProfile->updateProfileInfo();
    }
    this->projectsList->updateListContent();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Dashboard" template="../../../Template"
                 componentName="" parentClasses="public Component, public ChangeListener"
                 constructorParams="MainLayout &amp;workspaceRef" variableInitialisers="workspace(workspaceRef)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.660"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <TEXT pos="0Cc 5.5% 552 60" fill="solid: ffffff" hasStroke="0" text="Helio Workstation"
          fontname="Georgia" fontsize="55.00000000000000000000" kerning="0.00000000000000000000"
          bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="9bde1b4dd587d5fb" memberName="skew" virtualName=""
             explicitFocusOrder="0" pos="0R 0 64 0M" posRelativeX="981ceff5817d7b34"
             sourceFile="../../Themes/SeparatorVerticalSkew.cpp" constructorParams=""/>
  <JUCERCOMP name="" id="9e61167b79cef28c" memberName="backgroundB" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 384M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="981ceff5817d7b34" memberName="backgroundA" virtualName=""
             explicitFocusOrder="0" pos="0 0 320 0M" sourceFile="../../Themes/PanelBackgroundA.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="ea1b592642055bdc" memberName="logoImage" virtualName=""
                    explicitFocusOrder="0" pos="40 32 280 280" class="LogoFader"
                    params=""/>
  <JUCERCOMP name="" id="25591a755b533290" memberName="projectsList" virtualName=""
             explicitFocusOrder="0" pos="10Rr 10Rr 376 20M" sourceFile="Menu/DashboardMenu.cpp"
             constructorParams="&amp;App::Workspace()"/>
  <JUCERCOMP name="" id="13e51011dd762205" memberName="openProjectButton"
             virtualName="" explicitFocusOrder="0" pos="400 60 271 32" sourceFile="Menu/OpenProjectButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="c748db515539334" memberName="createProjectButton"
             virtualName="" explicitFocusOrder="0" pos="400 20 271 32" sourceFile="Menu/CreateProjectButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="26985c577d404f94" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="264 104 488 3" sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="2ed6285515243e89" memberName="loginButton" virtualName=""
             explicitFocusOrder="0" pos="400 120 272 32" sourceFile="Menu/LoginButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="f5d48eba3545f546" memberName="userProfile" virtualName=""
             explicitFocusOrder="0" pos="400 120 272 32" sourceFile="UserProfileComponent.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="2558009f569f191b" memberName="updatesInfo" virtualName=""
             explicitFocusOrder="0" pos="88 352 184 128" sourceFile="UpdatesInfoComponent.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

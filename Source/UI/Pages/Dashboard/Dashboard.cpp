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

#include "Dashboard.h"

//[MiscUserDefs]
#include "MainLayout.h"
#include "SpectralLogo.h"
#include "UserProfileComponent.h"
#include "LoginButton.h"
//[/MiscUserDefs]

Dashboard::Dashboard(MainLayout &workspaceRef)
    : workspace(workspaceRef)
{
    this->backgroundA.reset(new PanelBackgroundA());
    this->addAndMakeVisible(backgroundA.get());
    this->patreonLabel.reset(new Label(String(),
                                              String()));
    this->addAndMakeVisible(patreonLabel.get());
    this->patreonLabel->setFont(Font (16.00f, Font::plain));
    patreonLabel->setJustificationType(Justification::centred);
    patreonLabel->setEditable(false, false, false);

    this->userProfile.reset(new UserProfileComponent());
    this->addAndMakeVisible(userProfile.get());

    this->loginButton.reset(new LoginButton());
    this->addAndMakeVisible(loginButton.get());

    this->backgroundB.reset(new PanelBackgroundB());
    this->addAndMakeVisible(backgroundB.get());
    this->openProjectButton.reset(new OpenProjectButton());
    this->addAndMakeVisible(openProjectButton.get());
    openProjectButton->setBounds(400, 16, 271, 32);

    this->createProjectComboSource.reset(new MobileComboBox::Primer());
    this->addAndMakeVisible(createProjectComboSource.get());

    createProjectComboSource->setBounds(400, 52, 271, 140);

    this->skew.reset(new SeparatorVerticalSkew());
    this->addAndMakeVisible(skew.get());
    this->logo.reset(new SpectralLogo());
    this->addAndMakeVisible(logo.get());

    logo->setBounds(40, 32, 280, 280);

    this->projectsList.reset(new DashboardMenu(&App::Workspace()));
    this->addAndMakeVisible(projectsList.get());

    this->createProjectButton.reset(new CreateProjectButton());
    this->addAndMakeVisible(createProjectButton.get());
    createProjectButton->setBounds(400, 52, 271, 32);

    this->separator2.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator2.get());
    this->updatesInfo.reset(new UpdatesInfoComponent());
    this->addAndMakeVisible(updatesInfo.get());
    updatesInfo->setBounds(78, 352, 204, 172);

    this->patreonButton.reset(new OverlayButton());
    this->addAndMakeVisible(patreonButton.get());


    //[UserPreSize]
    this->setOpaque(true);
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);

    this->patreonLabel->setText(TRANS(I18n::Common::supportProject), dontSendNotification);
    this->patreonLabel->setColour(Label::textColourId,
        findDefaultColour(Label::textColourId).withMultipliedAlpha(0.25f));

    this->patreonButton->onClick = []()
    {
        URL url("https://www.patreon.com/peterrudenko");
        url.launchInDefaultBrowser();
    };
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

    backgroundA = nullptr;
    patreonLabel = nullptr;
    userProfile = nullptr;
    loginButton = nullptr;
    backgroundB = nullptr;
    openProjectButton = nullptr;
    createProjectComboSource = nullptr;
    skew = nullptr;
    logo = nullptr;
    projectsList = nullptr;
    createProjectButton = nullptr;
    separator2 = nullptr;
    updatesInfo = nullptr;
    patreonButton = nullptr;

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

    backgroundA->setBounds(0, 0, 320, getHeight() - 0);
    patreonLabel->setBounds(48, getHeight() - 9 - 32, 264, 32);
    userProfile->setBounds(48, getHeight() - 51 - 56, 264, 56);
    loginButton->setBounds(48, getHeight() - 51 - 56, 264, 56);
    backgroundB->setBounds(getWidth() - (getWidth() - 384), 0, getWidth() - 384, getHeight() - 0);
    skew->setBounds(0 + 320, 0, 64, getHeight() - 0);
    projectsList->setBounds(getWidth() - 10 - 376, getHeight() - 10 - (getHeight() - 20), 376, getHeight() - 20);
    separator2->setBounds(48, getHeight() - 44 - 3, 264, 3);
    patreonButton->setBounds(48, getHeight() - 9 - 32, 264, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
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
  <BACKGROUND backgroundColour="0">
    <TEXT pos="0Cc 5.5% 552 60" fill="solid: ffffff" hasStroke="0" text="Helio Workstation"
          fontname="Georgia" fontsize="55.0" kerning="0.0" bold="1" italic="0"
          justification="36" typefaceStyle="Bold"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="981ceff5817d7b34" memberName="backgroundA" virtualName=""
             explicitFocusOrder="0" pos="0 0 320 0M" sourceFile="../../Themes/PanelBackgroundA.cpp"
             constructorParams=""/>
  <LABEL name="" id="979d32499fd207d" memberName="patreonLabel" virtualName=""
         explicitFocusOrder="0" pos="48 9Rr 264 32" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="" id="f5d48eba3545f546" memberName="userProfile" virtualName=""
                    explicitFocusOrder="0" pos="48 51Rr 264 56" class="UserProfileComponent"
                    params=""/>
  <GENERICCOMPONENT name="" id="2ed6285515243e89" memberName="loginButton" virtualName=""
                    explicitFocusOrder="0" pos="48 51Rr 264 56" class="LoginButton"
                    params=""/>
  <JUCERCOMP name="" id="9e61167b79cef28c" memberName="backgroundB" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 384M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="13e51011dd762205" memberName="openProjectButton"
             virtualName="" explicitFocusOrder="0" pos="400 16 271 32" sourceFile="Menu/OpenProjectButton.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="524df900a9089845" memberName="createProjectComboSource"
                    virtualName="" explicitFocusOrder="0" pos="400 52 271 140" class="MobileComboBox::Primer"
                    params=""/>
  <JUCERCOMP name="" id="9bde1b4dd587d5fb" memberName="skew" virtualName=""
             explicitFocusOrder="0" pos="0R 0 64 0M" posRelativeX="981ceff5817d7b34"
             sourceFile="../../Themes/SeparatorVerticalSkew.cpp" constructorParams=""/>
  <GENERICCOMPONENT name="" id="ea1b592642055bdc" memberName="logo" virtualName=""
                    explicitFocusOrder="0" pos="40 32 280 280" class="SpectralLogo"
                    params=""/>
  <GENERICCOMPONENT name="" id="25591a755b533290" memberName="projectsList" virtualName=""
                    explicitFocusOrder="0" pos="10Rr 10Rr 376 20M" class="DashboardMenu"
                    params="&amp;App::Workspace()"/>
  <JUCERCOMP name="" id="c748db515539334" memberName="createProjectButton"
             virtualName="" explicitFocusOrder="0" pos="400 52 271 32" sourceFile="Menu/CreateProjectButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="26985c577d404f94" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="48 44Rr 264 3" sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="2558009f569f191b" memberName="updatesInfo" virtualName=""
             explicitFocusOrder="0" pos="78 352 204 172" sourceFile="UpdatesInfoComponent.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="80c9e93105d4044" memberName="patreonButton" virtualName=""
                    explicitFocusOrder="0" pos="48 9Rr 264 32" class="OverlayButton"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif




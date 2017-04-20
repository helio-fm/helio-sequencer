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

#include "RootTreeItemPanelDefault.h"

//[MiscUserDefs]
#include "MidiRollCommandPanel.h"
#include "HelioTheme.h"
#include "App.h"
#include "AuthorizationManager.h"
#include "MainLayout.h"
#include "WorkspaceMenu.h"
#include "HelioCallout.h"
#include "TreePanel.h"
#include "TreeItemComponent.h"
#include "RecentFilesList.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

RootTreeItemPanelDefault::RootTreeItemPanelDefault()
{
    addAndMakeVisible (shade = new ShadeLight());
    addAndMakeVisible (workspaceIcon = new IconComponent (Icons::workspace));

    addAndMakeVisible (workspaceNameLabel = new Label (String(),
                                                       TRANS("...")));
    workspaceNameLabel->setFont (Font (17.00f, Font::plain).withTypefaceStyle ("Regular"));
    workspaceNameLabel->setJustificationType (Justification::centredLeft);
    workspaceNameLabel->setEditable (false, false, false);
    workspaceNameLabel->setColour (Label::textColourId, Colour (0xffffe8da));
    workspaceNameLabel->setColour (TextEditor::textColourId, Colours::black);
    workspaceNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (menuButton = new TreeItemMenuButton());

    //[UserPreSize]
    this->shade->setVisible(false);
    this->workspaceNameLabel->setInterceptsMouseClicks(false, false);
    this->setInterceptsMouseClicks(true, false);

    this->workspaceNameLabel->setMinimumHorizontalScale(0.1f);
    this->workspaceNameLabel->setFont(Font(TREE_FONT_SIZE));
    //[/UserPreSize]

    setSize (225, 48);

    //[Constructor]
    this->updateLabels();
    this->workspaceNameLabel->setAlpha(0.75f);
    this->workspaceIcon->setAlpha(0.75f);
    this->menuButton->setAlpha(0.75f);

    App::Helio()->getAuthManager()->addChangeListener(this);
    //[/Constructor]
}

RootTreeItemPanelDefault::~RootTreeItemPanelDefault()
{
    //[Destructor_pre]
    App::Helio()->getAuthManager()->removeChangeListener(this);
    //[/Destructor_pre]

    shade = nullptr;
    workspaceIcon = nullptr;
    workspaceNameLabel = nullptr;
    menuButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RootTreeItemPanelDefault::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RootTreeItemPanelDefault::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shade->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    workspaceIcon->setBounds (10, (getHeight() / 2) - (28 / 2), 28, 28);
    workspaceNameLabel->setBounds (39, (getHeight() / 2) - (50 / 2), getWidth() - 48, 50);
    menuButton->setBounds (getWidth() - -35 - 28, (getHeight() / 2) - (28 / 2), 28, 28);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RootTreeItemPanelDefault::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::MenuButtonPressed)
    {
        Component *menu = new WorkspaceMenu(&App::Workspace());
        HelioCallout::emit(menu, this);
    }
    else if (commandId == CommandIDs::SelectRootItemPanel)
    {
        this->fader.fadeIn(this->shade, 200);
    }
    else if (commandId == CommandIDs::DeselectRootItemPanel)
    {
        this->fader.fadeOut(this->shade, 200);
    }
    else if (commandId == CommandIDs::UpdateRootItemPanel)
    {
        this->updateLabels();
    }
    //[/UserCode_handleCommandMessage]
}

void RootTreeItemPanelDefault::mouseEnter (const MouseEvent& e)
{
    //[UserCode_mouseEnter] -- Add your code here...
    this->fader.softFadeIn(this->workspaceNameLabel, 150);
    this->fader.softFadeIn(this->workspaceIcon, 150);
    //[/UserCode_mouseEnter]
}

void RootTreeItemPanelDefault::mouseExit (const MouseEvent& e)
{
    //[UserCode_mouseExit] -- Add your code here...
    this->fader.softFadeOut(0.75f, this->workspaceNameLabel, 200);
    this->fader.softFadeOut(0.75f, this->workspaceIcon, 200);
    //[/UserCode_mouseExit]
}

void RootTreeItemPanelDefault::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->getParentComponent()->postCommandMessage(CommandIDs::RootTreeItemPressed);
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
void RootTreeItemPanelDefault::changeListenerCallback(ChangeBroadcaster *source)
{
    this->updateLabels();
}

void RootTreeItemPanelDefault::updateLabels()
{
//    AuthorizationManager *authManager = App::Helio()->getAuthManager();
//    const bool loggedIn = (authManager->getAuthorizationState() == AuthorizationManager::LoggedIn);
//    const String login = authManager->getUserLoginOfCurrentSession();
//    this->workspaceNameLabel->setText(loggedIn ? login : SystemStats::getFullUserName(), dontSendNotification);

    // let's make it simple asf.
#if HELIO_DESKTOP
    this->workspaceNameLabel->setText(SystemStats::getFullUserName(), dontSendNotification);
#elif HELIO_MOBILE
    this->workspaceNameLabel->setText("Helio", dontSendNotification);
#endif


//    if (MainLayout *workspace = MainLayout::parentWorkspaceOf(this))
//    {
//        workspace->getRecentFilesList().forceUpdate();
//        const int numProjects = workspace->getRecentFilesList().getNumItems();
//        this->numProjectsLabel->setText(TRANS_PLURAL("{x} projects", numProjects), dontSendNotification);
//    }
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RootTreeItemPanelDefault"
                 template="../../Template" componentName="" parentClasses="public Component, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="225"
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
                    explicitFocusOrder="0" pos="10 0Cc 28 28" class="IconComponent"
                    params="Icons::workspace"/>
  <LABEL name="" id="caeb9c984a0dbed5" memberName="workspaceNameLabel"
         virtualName="" explicitFocusOrder="0" pos="39 0Cc 48M 50" textCol="ffffe8da"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="17" kerning="0" bold="0" italic="0" justification="33"/>
  <JUCERCOMP name="" id="6cc5b3a24299058e" memberName="menuButton" virtualName=""
             explicitFocusOrder="0" pos="-35Rr 0Cc 28 28" sourceFile="../Tree/TreeItemMenuButton.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

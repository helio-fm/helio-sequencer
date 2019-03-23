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

#include "UserProfileComponent.h"

//[MiscUserDefs]
#include "Workspace.h"
#include "UserProfile.h"
//[/MiscUserDefs]

UserProfileComponent::UserProfileComponent()
{
    this->nameLabel.reset(new Label(String(),
                                     TRANS("...")));
    this->addAndMakeVisible(nameLabel.get());
    this->nameLabel->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    nameLabel->setJustificationType(Justification::centredLeft);
    nameLabel->setEditable(false, false, false);

    this->avatar.reset(new IconComponent(Icons::github));
    this->addAndMakeVisible(avatar.get());

    this->separator.reset(new SeparatorVertical());
    this->addAndMakeVisible(separator.get());
    this->clickHandler.reset(new OverlayButton());
    this->addAndMakeVisible(clickHandler.get());


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(256, 32);

    //[Constructor]
    //[/Constructor]
}

UserProfileComponent::~UserProfileComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    nameLabel = nullptr;
    avatar = nullptr;
    separator = nullptr;
    clickHandler = nullptr;

    //[Destructor]
    //[/Destructor]
}

void UserProfileComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(Colours::black.withAlpha(0.35f));
    g.fillRect(this->avatar->getBounds().expanded(2, -1));
    g.fillRect(this->avatar->getBounds().expanded(-1, 2));
    g.setColour(Colours::white.withAlpha(0.25f));
    g.fillRect(this->avatar->getBounds().expanded(1, 0));
    g.fillRect(this->avatar->getBounds().expanded(0, 1));
    //[/UserPaint]
}

void UserProfileComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    nameLabel->setBounds(38, 0, getWidth() - 38, getHeight() - 2);
    avatar->setBounds(10, (getHeight() / 2) - (16 / 2), 16, 16);
    separator->setBounds(34, 4, 4, getHeight() - 8);
    clickHandler->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
void UserProfileComponent::updateProfileInfo()
{
    const auto &userProfile = App::Workspace().getUserProfile();
    if (userProfile.isLoggedIn())
    {
        this->avatar->setIconImage(userProfile.getAvatar());
        this->nameLabel->setText("/" + userProfile.getLogin(), dontSendNotification);
        this->clickHandler->onClick = []() {
            URL(App::Workspace().getUserProfile().getProfileUrl()).launchInDefaultBrowser();
        };
    }
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UserProfileComponent" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="256" initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="d16eb130158ae29c" memberName="nameLabel" virtualName=""
         explicitFocusOrder="0" pos="38 0 38M 2M" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="206f63304f17a28e" memberName="avatar" virtualName=""
                    explicitFocusOrder="0" pos="10 0Cc 16 16" class="IconComponent"
                    params="Icons::github"/>
  <JUCERCOMP name="" id="49a90a98eefa147f" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="34 4 4 8M" sourceFile="../../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="4b99a932dcc449b0" memberName="clickHandler" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="OverlayButton"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

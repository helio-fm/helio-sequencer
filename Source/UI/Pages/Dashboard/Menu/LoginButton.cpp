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

#include "LoginButton.h"

//[MiscUserDefs]
#include "Network.h"
#include "SessionService.h"
#include "UserProfileDto.h"
//[/MiscUserDefs]

LoginButton::LoginButton()
{
    this->avatar.reset(new IconComponent(Icons::github));
    this->addAndMakeVisible(avatar.get());

    this->separator.reset(new SeparatorVertical());
    this->addAndMakeVisible(separator.get());
    this->ctaLabel.reset(new Label(String(),
                                    TRANS("dialog::auth::github")));
    this->addAndMakeVisible(ctaLabel.get());
    this->ctaLabel->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    ctaLabel->setJustificationType(Justification::centredLeft);
    ctaLabel->setEditable(false, false, false);

    this->clickHandler.reset(new OverlayButton());
    this->addAndMakeVisible(clickHandler.get());


    //[UserPreSize]
    this->clickHandler->onClick = [this]() {
        App::Network().getSessionService()->signIn("Github");
    };
    //[/UserPreSize]

    this->setSize(256, 32);

    //[Constructor]
    //[/Constructor]
}

LoginButton::~LoginButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    avatar = nullptr;
    separator = nullptr;
    ctaLabel = nullptr;
    clickHandler = nullptr;

    //[Destructor]
    //[/Destructor]
}

void LoginButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LoginButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    avatar->setBounds(6, (getHeight() / 2) - ((getHeight() - 12) / 2), 24, getHeight() - 12);
    separator->setBounds(34, 4, 4, getHeight() - 8);
    ctaLabel->setBounds(38, 0, getWidth() - 38, getHeight() - 0);
    clickHandler->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LoginButton" template="../../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="256" initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="206f63304f17a28e" memberName="avatar" virtualName=""
                    explicitFocusOrder="0" pos="6 0Cc 24 12M" class="IconComponent"
                    params="Icons::github"/>
  <JUCERCOMP name="" id="49a90a98eefa147f" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="34 4 4 8M" sourceFile="../../../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <LABEL name="" id="becf12dc18ebf08f" memberName="ctaLabel" virtualName=""
         explicitFocusOrder="0" pos="38 0 38M 0M" labelText="dialog::auth::github"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="" id="4b99a932dcc449b0" memberName="clickHandler" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="OverlayButton"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

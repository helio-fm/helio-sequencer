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
//[/MiscUserDefs]

UserProfileComponent::UserProfileComponent()
{
    this->nameLabel.reset(new Label(String(),
                                     TRANS("user name:")));
    this->addAndMakeVisible(nameLabel.get());
    this->nameLabel->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    nameLabel->setJustificationType(Justification::centredLeft);
    nameLabel->setEditable(false, false, false);

    this->linkLabel.reset(new Label(String(),
                                     TRANS("profile link:")));
    this->addAndMakeVisible(linkLabel.get());
    this->linkLabel->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    linkLabel->setJustificationType(Justification::centredLeft);
    linkLabel->setEditable(false, false, false);


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(300, 200);

    //[Constructor]
    //[/Constructor]
}

UserProfileComponent::~UserProfileComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    nameLabel = nullptr;
    linkLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void UserProfileComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void UserProfileComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    nameLabel->setBounds(8, 8, getWidth() - 20, 24);
    linkLabel->setBounds(8, 40, getWidth() - 20, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UserProfileComponent" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="300" initialHeight="200">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="d16eb130158ae29c" memberName="nameLabel" virtualName=""
         explicitFocusOrder="0" pos="8 8 20M 24" labelText="user name:"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <LABEL name="" id="a051719deac4a34d" memberName="linkLabel" virtualName=""
         explicitFocusOrder="0" pos="8 40 20M 24" labelText="profile link:"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

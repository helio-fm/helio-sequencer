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

#include "SettingsListItemSelection.h"

//[MiscUserDefs]
#include "IconComponent.h"
//[/MiscUserDefs]

SettingsListItemSelection::SettingsListItemSelection()
{
    addAndMakeVisible (iconComponent = new IconComponent (Icons::apply));


    //[UserPreSize]
    this->iconComponent->setAlpha(0.6f);
    //[/UserPreSize]

    setSize (256, 32);

    //[Constructor]
    //[/Constructor]
}

SettingsListItemSelection::~SettingsListItemSelection()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    iconComponent = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SettingsListItemSelection::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x23000000));
    g.fillRoundedRectangle (40.0f, 2.0f, static_cast<float> (getWidth() - 45), static_cast<float> (getHeight() - 6), 6.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SettingsListItemSelection::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    iconComponent->setBounds (3, (getHeight() / 2) + -1 - (28 / 2), 28, 28);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SettingsListItemSelection"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="256"
                 initialHeight="32">
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="40 2 45M 6M" cornerSize="6" fill="solid: 23000000" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="935c1acd48db4664" memberName="iconComponent" virtualName=""
                    explicitFocusOrder="0" pos="3 -1Cc 28 28" class="IconComponent"
                    params="Icons::apply"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

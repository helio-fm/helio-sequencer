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

#include "SettingsListItemHighlighter.h"

//[MiscUserDefs]
//[/MiscUserDefs]

SettingsListItemHighlighter::SettingsListItemHighlighter()
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (256, 32);

    //[Constructor]
    //[/Constructor]
}

SettingsListItemHighlighter::~SettingsListItemHighlighter()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void SettingsListItemHighlighter::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colour (0x0b000000));
    g.fillRoundedRectangle (40.0f, 2.0f, static_cast<float> (getWidth() - 45), static_cast<float> (getHeight() - 6), 6.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SettingsListItemHighlighter::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SettingsListItemHighlighter"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="256"
                 initialHeight="32">
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="40 2 45M 6M" cornerSize="6" fill="solid: b000000" hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

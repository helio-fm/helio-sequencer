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

#include "ShadowUpwards.h"

//[MiscUserDefs]
//[/MiscUserDefs]

ShadowUpwards::ShadowUpwards()
{
    addAndMakeVisible (component = new SeparatorHorizontalReversed());

    //[UserPreSize]
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    setSize (600, 40);

    //[Constructor]
    //[/Constructor]
}

ShadowUpwards::~ShadowUpwards()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    component = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ShadowUpwards::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setGradientFill (ColourGradient (Colour (0x25000000),
                                       0.0f, static_cast<float> (getHeight()),
                                       Colour (0x00000000),
                                       0.0f, 0.0f,
                                       false));
    g.fillRect (0, 0, getWidth() - 0, getHeight() - 3);

    g.setGradientFill (ColourGradient (Colour (0x25000000),
                                       0.0f, static_cast<float> (getHeight()),
                                       Colour (0x00000000),
                                       0.0f, static_cast<float> (proportionOfHeight (0.5000f)),
                                       false));
    g.fillRect (0, getHeight() - 3 - proportionOfHeight (0.5000f), getWidth() - 0, proportionOfHeight (0.5000f));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ShadowUpwards::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds (0, getHeight() - 3, getWidth() - 0, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ShadowUpwards" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="40">
  <BACKGROUND backgroundColour="ffffff">
    <RECT pos="0 0 0M 3M" fill="linear: 0 0R, 0 0, 0=25000000, 1=0" hasStroke="0"/>
    <RECT pos="0 3Rr 0M 50%" fill="linear: 0 0R, 0 50%, 0=25000000, 1=0"
          hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="629c455b58f83ee7" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="0 3R 0M 3" sourceFile="SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

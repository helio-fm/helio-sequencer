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

#include "ShadeLight.h"

//[MiscUserDefs]
//[/MiscUserDefs]

ShadeLight::ShadeLight()
{
    addAndMakeVisible (component = new LighterShadowUpwards());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (256, 256);

    //[Constructor]
    this->setInterceptsMouseClicks(false, false);
    //[/Constructor]
}

ShadeLight::~ShadeLight()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    component = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ShadeLight::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0x0cffffff));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ShadeLight::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    component->setBounds (-374, 67, 300, 200);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ShadeLight" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="256" initialHeight="256">
  <BACKGROUND backgroundColour="cffffff"/>
  <JUCERCOMP name="" id="44a93234a43cdb0d" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="-374 67 300 200" sourceFile="LighterShadowUpwards.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

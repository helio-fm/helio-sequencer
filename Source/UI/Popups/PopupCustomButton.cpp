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

#include "PopupCustomButton.h"

//[MiscUserDefs]
//[/MiscUserDefs]

PopupCustomButton::PopupCustomButton(Component *newOwnedComponent)
    : PopupButton(true),
      ownedComponent(newOwnedComponent)
{

    //[UserPreSize]
    this->ownedComponent->setInterceptsMouseClicks(false, false);
    this->addAndMakeVisible(this->ownedComponent);
    //[/UserPreSize]

    setSize (48, 48);

    //[Constructor]
    //[/Constructor]
}

PopupCustomButton::~PopupCustomButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void PopupCustomButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    PopupButton::paint(g);

#if 0
    //[/UserPrePaint]

    g.setGradientFill (ColourGradient (Colour (0x59997cff),
                                       static_cast<float> ((getWidth() / 2)), static_cast<float> (getHeight()),
                                       Colour (0x548f84ff),
                                       static_cast<float> ((getWidth() / 2)), static_cast<float> (getHeight() - -55),
                                       true));
    g.fillEllipse (3.0f, 3.0f, static_cast<float> (getWidth() - 6), static_cast<float> (getHeight() - 6));

    g.setGradientFill (ColourGradient (Colour (0x6effffff),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       Colours::white,
                                       static_cast<float> ((getWidth() / 2)), static_cast<float> (-55),
                                       true));
    g.drawEllipse (3.0f, 3.0f, static_cast<float> (getWidth() - 6), static_cast<float> (getHeight() - 6), 1.500f);

    //[UserPaint] Add your own custom painting code here..
#endif

    //[/UserPaint]
}

void PopupCustomButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    PopupButton::resized();

    this->ownedComponent->
    setTopLeftPosition((this->getWidth() / 2) - (ownedComponent->getWidth() / 2),
                       (this->getHeight() / 2) - (ownedComponent->getHeight() / 2));
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PopupCustomButton" template="../../Template"
                 componentName="" parentClasses="public PopupButton" constructorParams="Component *newOwnedComponent"
                 variableInitialisers="PopupButton(true),&#10;ownedComponent(newOwnedComponent)"
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="48" initialHeight="48">
  <BACKGROUND backgroundColour="1f3677">
    <ELLIPSE pos="3 3 6M 6M" fill=" radial: 0C 0R, 0C -55R, 0=59997cff, 1=548f84ff"
             hasStroke="1" stroke="1.5, mitered, butt" strokeColour=" radial: 0C 0, 0C -55, 0=6effffff, 1=ffffffff"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

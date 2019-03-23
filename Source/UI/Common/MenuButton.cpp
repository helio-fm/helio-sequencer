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

#include "MenuButton.h"

//[MiscUserDefs]
#include "IconComponent.h"
#include "Icons.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

MenuButton::MenuButton()
    : HighlightedComponent()
{

    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(128, 128);

    //[Constructor]
    //[/Constructor]
}

MenuButton::~MenuButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]


    //[Destructor]
    //[/Destructor]
}

void MenuButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 2.0f, y = 2.0f, width = static_cast<float> (getWidth() - 4), height = static_cast<float> (getHeight() - 4);
        Colour strokeColour = Colour (0x0b000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawEllipse (x, y, width, height, 0.500f);
    }

    {
        float x = 3.0f, y = 3.0f, width = static_cast<float> (getWidth() - 6), height = static_cast<float> (getHeight() - 6);
        Colour fillColour = Colour (0x0dffffff);
        Colour strokeColour = Colour (0x27ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillEllipse (x, y, width, height);
        g.setColour (strokeColour);
        g.drawEllipse (x, y, width, height, 0.500f);
    }

    {
        float x = static_cast<float> ((getWidth() / 2) + -10 - (7 / 2)), y = static_cast<float> ((getHeight() / 2) + -30 - (7 / 2)), width = 7.0f, height = 7.0f;
        Colour fillColour = Colour (0x1e000000);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillEllipse (x, y, width, height);
        g.setColour (strokeColour);
        g.drawEllipse (x, y, width, height, 0.100f);
    }

    {
        float x = static_cast<float> ((getWidth() / 2) - (7 / 2)), y = static_cast<float> ((getHeight() / 2) + -30 - (7 / 2)), width = 7.0f, height = 7.0f;
        Colour fillColour = Colour (0x1e000000);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillEllipse (x, y, width, height);
        g.setColour (strokeColour);
        g.drawEllipse (x, y, width, height, 0.100f);
    }

    {
        float x = static_cast<float> ((getWidth() / 2) + 10 - (7 / 2)), y = static_cast<float> ((getHeight() / 2) + -30 - (7 / 2)), width = 7.0f, height = 7.0f;
        Colour fillColour = Colour (0x1e000000);
        Colour strokeColour = Colours::black;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillEllipse (x, y, width, height);
        g.setColour (strokeColour);
        g.drawEllipse (x, y, width, height, 0.100f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MenuButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MenuButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->getParentComponent()->postCommandMessage(CommandIDs::MenuButtonPressed);
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MenuButton" template="../../Template"
                 componentName="" parentClasses="public HighlightedComponent"
                 constructorParams="" variableInitialisers="HighlightedComponent()"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="128" initialHeight="128">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ELLIPSE pos="2 2 4M 4M" fill="solid: 0" hasStroke="1" stroke="0.5, mitered, butt"
             strokeColour="solid: b000000"/>
    <ELLIPSE pos="3 3 6M 6M" fill="solid: dffffff" hasStroke="1" stroke="0.5, mitered, butt"
             strokeColour="solid: 27ffffff"/>
    <ELLIPSE pos="-10Cc -30Cc 7 7" fill="solid: 1e000000" hasStroke="1" stroke="0.1, mitered, butt"
             strokeColour="solid: ff000000"/>
    <ELLIPSE pos="0Cc -30Cc 7 7" fill="solid: 1e000000" hasStroke="1" stroke="0.1, mitered, butt"
             strokeColour="solid: ff000000"/>
    <ELLIPSE pos="10Cc -30Cc 7 7" fill="solid: 1e000000" hasStroke="1" stroke="0.1, mitered, butt"
             strokeColour="solid: ff000000"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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
//[/MiscUserDefs]

LoginButton::LoginButton()
{
    this->component.reset(new IconComponent(Icons::github));
    this->addAndMakeVisible(component.get());
    component->setName ("new component");

    this->component2.reset(new SeparatorVertical());
    this->addAndMakeVisible(component2.get());
    this->ctaLabel.reset(new Label(String(),
                                    TRANS("dialog::auth::github")));
    this->addAndMakeVisible(ctaLabel.get());
    this->ctaLabel->setFont(Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    ctaLabel->setJustificationType(Justification::centredLeft);
    ctaLabel->setEditable(false, false, false);

    this->clickHandler.reset(new TextButton(String()));
    this->addAndMakeVisible(clickHandler.get());
    clickHandler->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnRight | Button::ConnectedOnTop | Button::ConnectedOnBottom);
    clickHandler->addListener(this);
    clickHandler->setColour(TextButton::buttonColourId, Colour (0x00000000));
    clickHandler->setColour(TextButton::buttonOnColourId, Colour (0x14ffffff));


    //[UserPreSize]
    this->clickHandler->setMouseCursor(MouseCursor::PointingHandCursor);
    //[/UserPreSize]

    this->setSize(256, 32);

    //[Constructor]
    //[/Constructor]
}

LoginButton::~LoginButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    component = nullptr;
    component2 = nullptr;
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

    component->setBounds(2, 2, 28, getHeight() - 4);
    component2->setBounds(38, 4, 4, getHeight() - 8);
    ctaLabel->setBounds(42, 0, getWidth() - 42, getHeight() - 0);
    clickHandler->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LoginButton::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == clickHandler.get())
    {
        //[UserButtonCode_clickHandler] -- add your button handler code here..
        //[/UserButtonCode_clickHandler]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LoginButton" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="256" initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="new component" id="206f63304f17a28e" memberName="component"
                    virtualName="" explicitFocusOrder="0" pos="2 2 28 4M" class="IconComponent"
                    params="Icons::github"/>
  <JUCERCOMP name="" id="49a90a98eefa147f" memberName="component2" virtualName=""
             explicitFocusOrder="0" pos="38 4 4 8M" sourceFile="../../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <LABEL name="" id="becf12dc18ebf08f" memberName="ctaLabel" virtualName=""
         explicitFocusOrder="0" pos="42 0 42M 0M" labelText="dialog::auth::github"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="" id="7e8a6c95d463c081" memberName="clickHandler" virtualName=""
              explicitFocusOrder="0" pos="0 0 0M 0M" bgColOff="0" bgColOn="14ffffff"
              buttonText="" connectedEdges="15" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

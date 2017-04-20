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

#include "RolloverHeaderLeft.h"

//[MiscUserDefs]
#include "TreePanel.h"
//[/MiscUserDefs]

RolloverHeaderLeft::RolloverHeaderLeft(const String &headerTitle)
    : title(headerTitle)
{
    addAndMakeVisible (shade = new ShadeLight());
    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("...")));
    titleLabel->setFont (Font (17.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (false, false, false);
    titleLabel->setColour (Label::textColourId, Colours::white);
    titleLabel->setColour (TextEditor::textColourId, Colours::black);
    titleLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (detailsLabel = new Label (String(),
                                                 TRANS("...")));
    detailsLabel->setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular"));
    detailsLabel->setJustificationType (Justification::centredLeft);
    detailsLabel->setEditable (false, false, false);
    detailsLabel->setColour (Label::textColourId, Colour (0x77ffffff));
    detailsLabel->setColour (TextEditor::textColourId, Colours::black);
    detailsLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (backButton = new RolloverBackButtonLeft());

    //[UserPreSize]
    this->setInterceptsMouseClicks(true, false);
    this->detailsLabel->setInterceptsMouseClicks(false, false);
    this->titleLabel->setInterceptsMouseClicks(false, false);
    this->backButton->setInterceptsMouseClicks(false, false);
    //[/UserPreSize]

    setSize (225, 55);

    //[Constructor]
    //[/Constructor]
}

RolloverHeaderLeft::~RolloverHeaderLeft()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    shade = nullptr;
    titleLabel = nullptr;
    detailsLabel = nullptr;
    backButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RolloverHeaderLeft::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RolloverHeaderLeft::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shade->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    titleLabel->setBounds (47, (getHeight() / 2) + -10 - (24 / 2), getWidth() - 88, 24);
    detailsLabel->setBounds (48, (getHeight() / 2) + 10 - (24 / 2), getWidth() - 88, 24);
    backButton->setBounds (15, (getHeight() / 2) - (32 / 2), 32, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RolloverHeaderLeft::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    // forward the message to tree panel
    this->getParentComponent()->postCommandMessage(commandId);
    //[/UserCode_handleCommandMessage]
}

void RolloverHeaderLeft::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    this->getParentComponent()->postCommandMessage(CommandIDs::HideRollover);
    //[/UserCode_mouseDown]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RolloverHeaderLeft" template="../../Template"
                 componentName="" parentClasses="public HighlightedComponent"
                 constructorParams="const String &amp;headerTitle" variableInitialisers="title(headerTitle)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="225" initialHeight="55">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="1eb024ea37337815" memberName="shade" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/ShadeLight.cpp"
             constructorParams=""/>
  <LABEL name="" id="caeb9c984a0dbed5" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="47 -10Cc 88M 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="17" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="daf4204f5a799225" memberName="detailsLabel" virtualName=""
         explicitFocusOrder="0" pos="48 10Cc 88M 24" textCol="77ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="33"/>
  <JUCERCOMP name="" id="6cc5b3a24299058e" memberName="backButton" virtualName=""
             explicitFocusOrder="0" pos="15 0Cc 32 32" sourceFile="RolloverBackButtonLeft.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "FatalErrorScreen.h"

//[MiscUserDefs]
//[/MiscUserDefs]

FatalErrorScreen::FatalErrorScreen()
{
    addAndMakeVisible (bg = new PanelBackgroundA());
    addAndMakeVisible (messageLabel = new Label (String(),
                                                 TRANS("warnings::smallscreen")));
    messageLabel->setFont (Font (Font::getDefaultSerifFontName(), 28.00f, Font::plain).withTypefaceStyle ("Regular"));
    messageLabel->setJustificationType (Justification::centred);
    messageLabel->setEditable (false, false, false);
    messageLabel->setColour (Label::textColourId, Colour (0x6cffffff));
    messageLabel->setColour (TextEditor::textColourId, Colours::black);
    messageLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (separator2 = new SeparatorHorizontalFading());
    addAndMakeVisible (separator1 = new SeparatorHorizontalFading());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    //[/Constructor]
}

FatalErrorScreen::~FatalErrorScreen()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    bg = nullptr;
    messageLabel = nullptr;
    separator2 = nullptr;
    separator1 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void FatalErrorScreen::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FatalErrorScreen::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    messageLabel->setBounds ((getWidth() / 2) - (500 / 2), proportionOfHeight (0.2550f), 500, 88);
    separator2->setBounds ((getWidth() / 2) - (350 / 2), proportionOfHeight (0.2550f) + 88 - -8, 350, 8);
    separator1->setBounds ((getWidth() / 2) - (350 / 2), proportionOfHeight (0.2550f) + 88 - 104, 350, 8);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FatalErrorScreen" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="66a1a33f06322bfb" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundA.cpp"
             constructorParams=""/>
  <LABEL name="" id="49d9df10162c3f76" memberName="messageLabel" virtualName=""
         explicitFocusOrder="0" pos="0Cc 25.496% 500 88" textCol="6cffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="warnings::smallscreen"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="28" kerning="0" bold="0"
         italic="0" justification="36"/>
  <JUCERCOMP name="" id="770155e9a7e2d5a8" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="0Cc -8R 350 8" posRelativeY="49d9df10162c3f76"
             sourceFile="../Themes/SeparatorHorizontalFading.cpp" constructorParams=""/>
  <JUCERCOMP name="" id="737c93f8a235cedb" memberName="separator1" virtualName=""
             explicitFocusOrder="0" pos="0Cc 104R 350 8" posRelativeY="49d9df10162c3f76"
             sourceFile="../Themes/SeparatorHorizontalFading.cpp" constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

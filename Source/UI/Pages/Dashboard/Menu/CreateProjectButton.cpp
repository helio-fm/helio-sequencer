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

#include "CreateProjectButton.h"

//[MiscUserDefs]
#include "Icons.h"
#include "IconComponent.h"
#include "Workspace.h"
//[/MiscUserDefs]

CreateProjectButton::CreateProjectButton()
{
    this->newProjectImage.reset(new IconComponent(Icons::create));
    this->addAndMakeVisible(newProjectImage.get());

    this->newProjectLabel.reset(new Label(String(),
                                           TRANS("menu::workspace::project::create")));
    this->addAndMakeVisible(newProjectLabel.get());
    this->newProjectLabel->setFont(Font (Font::getDefaultSerifFontName(), 18.00f, Font::plain).withTypefaceStyle ("Regular"));
    newProjectLabel->setJustificationType(Justification::centredLeft);
    newProjectLabel->setEditable(false, false, false);

    this->separator.reset(new SeparatorVertical());
    this->addAndMakeVisible(separator.get());
    this->clickHandler.reset(new OverlayButton());
    this->addAndMakeVisible(clickHandler.get());


    //[UserPreSize]
    this->newProjectLabel->setInterceptsMouseClicks(false, false);
    this->clickHandler->onClick = []() {
        App::Workspace().createEmptyProject();
    };
    //[/UserPreSize]

    this->setSize(256, 32);

    //[Constructor]
    //[/Constructor]
}

CreateProjectButton::~CreateProjectButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    newProjectImage = nullptr;
    newProjectLabel = nullptr;
    separator = nullptr;
    clickHandler = nullptr;

    //[Destructor]
    //[/Destructor]
}

void CreateProjectButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    // a hack
#if HELIO_DESKTOP
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
#endif
    //[/UserPaint]
}

void CreateProjectButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    newProjectImage->setBounds(6, (getHeight() / 2) - ((getHeight() - 12) / 2), 24, getHeight() - 12);
    newProjectLabel->setBounds(38, 0, getWidth() - 38, getHeight() - 0);
    separator->setBounds(34, 4, 4, getHeight() - 8);
    clickHandler->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CreateProjectButton" template="../../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="256" initialHeight="32">
  <BACKGROUND backgroundColour="ffffff"/>
  <GENERICCOMPONENT name="" id="79f90a69d0b95011" memberName="newProjectImage" virtualName=""
                    explicitFocusOrder="0" pos="6 0Cc 24 12M" class="IconComponent"
                    params="Icons::create"/>
  <LABEL name="" id="8ebb161d0a976635" memberName="newProjectLabel" virtualName=""
         explicitFocusOrder="0" pos="38 0 38M 0M" labelText="menu::workspace::project::create"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="18.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="33"/>
  <JUCERCOMP name="" id="49a90a98eefa147f" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="34 4 4 8M" sourceFile="../../../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="4b99a932dcc449b0" memberName="clickHandler" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="OverlayButton"
                    params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "SettingsPage.h"

//[MiscUserDefs]
#include "MainLayout.h"
#include "ComponentsList.h"
#include "AudioSettings.h"
#include "ThemeSettings.h"
//[/MiscUserDefs]

SettingsPage::SettingsPage(Component *settingsList)
{
    addAndMakeVisible (background = new PanelBackgroundB());
    addAndMakeVisible (viewport = new Viewport (String()));
    viewport->setScrollBarsShown (true, false);

    addAndMakeVisible (shadow = new LightShadowRightwards());

    //[UserPreSize]
    viewport->setViewedComponent(settingsList, false);
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);
    //[/Constructor]
}

SettingsPage::~SettingsPage()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    viewport = nullptr;
    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SettingsPage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SettingsPage::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    viewport->setBounds (20, 20, getWidth() - 40, getHeight() - 40);
    shadow->setBounds (0, 0, 5, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    this->viewport->getViewedComponent()->
    setSize(this->viewport->getMaximumVisibleWidth(),
            this->viewport->getViewedComponent()->getHeight());
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SettingsPage" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams="Component *settingsList"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <JUCERCOMP name="" id="e130bb0b9ed67f09" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <VIEWPORT name="" id="1df16503f554b532" memberName="viewport" virtualName=""
            explicitFocusOrder="0" pos="20 20 40M 40M" vscroll="1" hscroll="0"
            scrollbarThickness="18" contentType="0" jucerFile="" contentClass=""
            constructorParams=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 0 5 0M" sourceFile="../Themes/LightShadowRightwards.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

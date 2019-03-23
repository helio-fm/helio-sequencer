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
//[/MiscUserDefs]

SettingsPage::SettingsPage(Component *settingsList)
{
    this->background.reset(new PanelBackgroundB());
    this->addAndMakeVisible(background.get());
    this->viewport.reset(new Viewport(String()));
    this->addAndMakeVisible(viewport.get());
    viewport->setScrollBarsShown (true, false);
    viewport->setScrollBarThickness (30);


    //[UserPreSize]
    this->setFocusContainer(true);
    this->setWantsKeyboardFocus(true);
    this->setPaintingIsUnclipped(true);

#if HELIO_DESKTOP
    this->viewport->setScrollBarThickness(2);
#endif

    this->viewport->setViewedComponent(settingsList, false);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    //[/Constructor]
}

SettingsPage::~SettingsPage()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    viewport = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SettingsPage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SettingsPage::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    viewport->setBounds(16, 16, getWidth() - 32, getHeight() - 32);
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

<JUCER_COMPONENT documentType="Component" className="SettingsPage" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="Component *settingsList"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="e130bb0b9ed67f09" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <VIEWPORT name="" id="1df16503f554b532" memberName="viewport" virtualName=""
            explicitFocusOrder="0" pos="16 16 32M 32M" vscroll="1" hscroll="0"
            scrollbarThickness="30" contentType="0" jucerFile="" contentClass=""
            constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

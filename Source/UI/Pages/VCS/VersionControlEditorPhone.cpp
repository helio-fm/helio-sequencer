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

#include "VersionControlEditorPhone.h"
#include "VersionControlEditorPhoneViewportContent.h"

//[MiscUserDefs]
#include "VersionControl.h"
//[/MiscUserDefs]

VersionControlEditorPhone::VersionControlEditorPhone(VersionControl &versionControl)
    : VersionControlEditor(versionControl)
{
    addAndMakeVisible (background = new PanelBackgroundB());
    addAndMakeVisible (shadow = new LightShadowRightwards());
    addAndMakeVisible (viewport = new Viewport (String()));
    viewport->setScrollBarsShown (false, false);
    viewport->setViewedComponent (new VersionControlEditorPhoneViewportContent (versionControl));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    this->viewport->setScrollOnDragEnabled(true);
    //[/Constructor]
}

VersionControlEditorPhone::~VersionControlEditorPhone()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    shadow = nullptr;
    viewport = nullptr;

    //[Destructor]
    //[/Destructor]
}

void VersionControlEditorPhone::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void VersionControlEditorPhone::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    shadow->setBounds (0, 0, 5, getHeight() - 0);
    viewport->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    this->viewport->getViewedComponent()->setBounds(0, 0, this->viewport->getWidth() * 2, this->viewport->getHeight());
    //[/UserResized]
}


//[MiscUserCode]

void VersionControlEditorPhone::updateState()
{
    // VCS or project has changed
    if (VersionControlEditor *contentEditor =
        dynamic_cast<VersionControlEditor *>(this->viewport->getViewedComponent()))
    {
        contentEditor->updateState();
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="VersionControlEditorPhone"
                 template="../../Template" componentName="" parentClasses="public VersionControlEditor"
                 constructorParams="VersionControl &amp;versionControl" variableInitialisers="VersionControlEditor(versionControl)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="9ce6aa2b193dc2e7" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 0 5 0M" sourceFile="../Themes/LightShadowRightwards.cpp"
             constructorParams=""/>
  <VIEWPORT name="" id="45910803fceb488b" memberName="viewport" virtualName=""
            explicitFocusOrder="0" pos="0 0 0M 0M" vscroll="0" hscroll="0"
            scrollbarThickness="18" contentType="1" jucerFile="VersionControlEditorPhoneViewportContent.cpp"
            contentClass="" constructorParams="versionControl"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

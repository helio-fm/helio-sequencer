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

#include "VersionControlEditorDefault.h"

//[MiscUserDefs]
#include "VersionControl.h"
//[/MiscUserDefs]

VersionControlEditorDefault::VersionControlEditorDefault(VersionControl &versionControl)
    : VersionControlEditor(versionControl)
{
    addAndMakeVisible (background = new PanelBackgroundB());
    addAndMakeVisible (stageComponent = new StageComponent (versionControl));
    addAndMakeVisible (historyComponent = new HistoryComponent (versionControl));
    addAndMakeVisible (anchor = new Component());

    addAndMakeVisible (shadow = new LightShadowRightwards());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    //[/Constructor]
}

VersionControlEditorDefault::~VersionControlEditorDefault()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    stageComponent = nullptr;
    historyComponent = nullptr;
    anchor = nullptr;
    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void VersionControlEditorDefault::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void VersionControlEditorDefault::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    stageComponent->setBounds (15, 10, proportionOfWidth (0.5020f) - 15, getHeight() - 20);
    historyComponent->setBounds (0 + proportionOfWidth (0.5020f) - -15, 10, proportionOfWidth (0.5020f) - 30, getHeight() - 20);
    anchor->setBounds (0, 0, proportionOfWidth (0.5020f), 8);
    shadow->setBounds (0, 0, 5, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

void VersionControlEditorDefault::updateState()
{
    // VCS or project has changed
    this->vcs.getHead().rebuildDiffNow();
    this->historyComponent->rebuildRevisionTree();
}

void VersionControlEditorDefault::onStageSelectionChanged()
{
    this->historyComponent->clearSelection();
}

void VersionControlEditorDefault::onHistorySelectionChanged()
{
    this->stageComponent->clearSelection();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="VersionControlEditorDefault"
                 template="../../../Template" componentName="" parentClasses="public VersionControlEditor"
                 constructorParams="VersionControl &amp;versionControl" variableInitialisers="VersionControlEditor(versionControl)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="9ce6aa2b193dc2e7" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="2c6b2bd8a55487b1" memberName="stageComponent" virtualName=""
             explicitFocusOrder="0" pos="15 10 15M 20M" posRelativeW="4ac6bf71d1e1d84f"
             sourceFile="StageComponent.cpp" constructorParams="versionControl"/>
  <JUCERCOMP name="" id="6b8c3baafe7f22f1" memberName="historyComponent" virtualName=""
             explicitFocusOrder="0" pos="-15R 10 30M 20M" posRelativeX="4ac6bf71d1e1d84f"
             posRelativeW="4ac6bf71d1e1d84f" sourceFile="HistoryComponent.cpp"
             constructorParams="versionControl"/>
  <GENERICCOMPONENT name="" id="4ac6bf71d1e1d84f" memberName="anchor" virtualName=""
                    explicitFocusOrder="0" pos="0 0 50.2% 8" class="Component" params=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 0 5 0M" sourceFile="../../Themes/LightShadowRightwards.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

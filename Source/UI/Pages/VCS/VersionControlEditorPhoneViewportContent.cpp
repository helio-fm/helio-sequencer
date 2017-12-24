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

#include "VersionControlEditorPhoneViewportContent.h"

//[MiscUserDefs]
#include "VersionControl.h"
//[/MiscUserDefs]

VersionControlEditorPhoneViewportContent::VersionControlEditorPhoneViewportContent(VersionControl &versionControl)
    : VersionControlEditor(versionControl)
{
    addAndMakeVisible (stageComponent = new StageComponent (versionControl));
    addAndMakeVisible (historyComponent = new HistoryComponent (versionControl));
    addAndMakeVisible (anchor = new Component());

    addAndMakeVisible (slideRightButton = new TextButton (String()));
    slideRightButton->setButtonText (TRANS(">"));
    slideRightButton->addListener (this);

    addAndMakeVisible (slideLeftButton = new TextButton ("new button"));
    slideLeftButton->setButtonText (TRANS("<"));
    slideLeftButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    this->setOpaque(true);

    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);
    //[/Constructor]
}

VersionControlEditorPhoneViewportContent::~VersionControlEditorPhoneViewportContent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    stageComponent = nullptr;
    historyComponent = nullptr;
    anchor = nullptr;
    slideRightButton = nullptr;
    slideLeftButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void VersionControlEditorPhoneViewportContent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void VersionControlEditorPhoneViewportContent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    stageComponent->setBounds (0 + 15, 15, proportionOfWidth (0.5000f) - 75, getHeight() - 25);
    historyComponent->setBounds (0 + proportionOfWidth (0.5000f) - -55, 15, proportionOfWidth (0.5000f) - 75, getHeight() - 25);
    anchor->setBounds (0, 0, proportionOfWidth (0.5000f), 8);
    slideRightButton->setBounds ((0 + 15) + (proportionOfWidth (0.5000f) - 75) - -10, 15 + (getHeight() - 25) / 2 - (128 / 2), 40, 128);
    slideLeftButton->setBounds ((0 + proportionOfWidth (0.5000f) - -55) + -10 - 40, 15 + (getHeight() - 25) / 2 - (128 / 2), 40, 128);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void VersionControlEditorPhoneViewportContent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == slideRightButton)
    {
        //[UserButtonCode_slideRightButton] -- add your button handler code here..
        Component *parent = this->getParentComponent();
        if (Viewport *viewport = dynamic_cast<Viewport *>(this->getParentComponent()->getParentComponent()))
        {
            viewport->setViewPosition(0, 0);
        }
        //[/UserButtonCode_slideRightButton]
    }
    else if (buttonThatWasClicked == slideLeftButton)
    {
        //[UserButtonCode_slideLeftButton] -- add your button handler code here..
        Component *parent = this->getParentComponent();
        if (Viewport *viewport = dynamic_cast<Viewport *>(this->getParentComponent()->getParentComponent()))
        {
            viewport->setViewPosition(-viewport->getWidth(), 0);
        }
        //[/UserButtonCode_slideLeftButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}


//[MiscUserCode]

void VersionControlEditorPhoneViewportContent::updateState()
{
    // VCS or project has changed
    this->vcs.getHead().rebuildDiffNow();
    this->historyComponent->rebuildRevisionTree();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="VersionControlEditorPhoneViewportContent"
                 template="../../Template" componentName="" parentClasses="public VersionControlEditor"
                 constructorParams="VersionControl &amp;versionControl" variableInitialisers="VersionControlEditor(versionControl)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="2c6b2bd8a55487b1" memberName="stageComponent" virtualName=""
             explicitFocusOrder="0" pos="15 15 75M 25M" posRelativeX="4ac6bf71d1e1d84f"
             posRelativeW="4ac6bf71d1e1d84f" sourceFile="StageComponent.cpp"
             constructorParams="versionControl"/>
  <JUCERCOMP name="" id="6b8c3baafe7f22f1" memberName="historyComponent" virtualName=""
             explicitFocusOrder="0" pos="-55R 15 75M 25M" posRelativeX="4ac6bf71d1e1d84f"
             posRelativeW="4ac6bf71d1e1d84f" sourceFile="HistoryComponent.cpp"
             constructorParams="versionControl"/>
  <GENERICCOMPONENT name="" id="4ac6bf71d1e1d84f" memberName="anchor" virtualName=""
                    explicitFocusOrder="0" pos="0 0 50.103% 8" class="Component"
                    params=""/>
  <TEXTBUTTON name="" id="94da8b113cdf3c83" memberName="slideRightButton" virtualName=""
              explicitFocusOrder="0" pos="-10R 0Cc 40 128" posRelativeX="2c6b2bd8a55487b1"
              posRelativeY="2c6b2bd8a55487b1" buttonText="&gt;" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="261e7726ea5bd87a" memberName="slideLeftButton"
              virtualName="" explicitFocusOrder="0" pos="-10r 0Cc 40 128" posRelativeX="6b8c3baafe7f22f1"
              posRelativeY="6b8c3baafe7f22f1" buttonText="&lt;" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

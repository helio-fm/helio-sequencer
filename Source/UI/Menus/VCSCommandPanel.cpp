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

#include "VCSCommandPanel.h"

//[MiscUserDefs]
#include "StageComponent.h"
#include "VersionControl.h"
#include "ProjectTreeItem.h"
//[/MiscUserDefs]

VCSCommandPanel::VCSCommandPanel(ProjectTreeItem &parentProject, VersionControl &versionControl)
    : vcs(versionControl),
      project(parentProject)
{
    addAndMakeVisible (bg = new PanelBackgroundC());
    addAndMakeVisible (stage = new StageComponent (versionControl));

    //[UserPreSize]
    //[/UserPreSize]

    setSize (330, 369);

    //[Constructor]
    this->vcs.getHead().rebuildDiffIfNeeded();

    this->vcs.addChangeListener(this);
    this->project.addChangeListener(this);
    //[/Constructor]
}

VCSCommandPanel::~VCSCommandPanel()
{
    //[Destructor_pre]
    this->project.removeChangeListener(this);
    this->vcs.removeChangeListener(this);
    //[/Destructor_pre]

    bg = nullptr;
    stage = nullptr;

    //[Destructor]
    //[/Destructor]
}

void VCSCommandPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void VCSCommandPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    stage->setBounds (5, 7, getWidth() - 10, getHeight() - 14);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
void VCSCommandPanel::changeListenerCallback(ChangeBroadcaster *source)
{
    // vcs or project has changed
    if (this->isVisible())
    {
        this->vcs.getHead().rebuildDiffNow();
    }
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="VCSCommandPanel" template="../../Template"
                 componentName="" parentClasses="public Component, private ChangeListener"
                 constructorParams="ProjectTreeItem &amp;parentProject, VersionControl &amp;versionControl"
                 variableInitialisers="vcs(versionControl),&#10;project(parentProject)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="330" initialHeight="369">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="274b1411c9ae170b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="539b662d2b873b49" memberName="stage" virtualName=""
             explicitFocusOrder="0" pos="5 7 10M 14M" sourceFile="../Pages/VCS/StageComponent.cpp"
             constructorParams="versionControl"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

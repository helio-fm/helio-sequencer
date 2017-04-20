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

#include "HistoryComponent.h"

//[MiscUserDefs]
#include "App.h"
#include "VersionControl.h"
#include "RevisionTreeComponent.h"
#include "ViewportFitProxyComponent.h"
#include "HelioCallout.h"
#include "CommandIDs.h"

#include "MainLayout.h"
#include "ModalDialogConfirmation.h"
//[/MiscUserDefs]

HistoryComponent::HistoryComponent(VersionControl &owner)
    : vcs(owner)
{
    addAndMakeVisible (panel = new PanelA());
    addAndMakeVisible (revisionViewport = new Viewport());

    addAndMakeVisible (pushButton = new TextButton ("push"));
    pushButton->setButtonText (String());
    pushButton->setConnectedEdges (Button::ConnectedOnTop);
    pushButton->addListener (this);

    addAndMakeVisible (pullButton = new TextButton ("pull"));
    pullButton->setButtonText (String());
    pullButton->setConnectedEdges (Button::ConnectedOnTop);
    pullButton->addListener (this);

    addAndMakeVisible (revisionTreeLabel = new Label (String(),
                                                      TRANS("vcs::history::caption")));
    revisionTreeLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionTreeLabel->setJustificationType (Justification::centredLeft);
    revisionTreeLabel->setEditable (false, false, false);
    revisionTreeLabel->setColour (TextEditor::textColourId, Colours::black);
    revisionTreeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (shadow = new LightShadowDownwards());

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    //[/Constructor]
}

HistoryComponent::~HistoryComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    panel = nullptr;
    revisionViewport = nullptr;
    pushButton = nullptr;
    pullButton = nullptr;
    revisionTreeLabel = nullptr;
    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HistoryComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HistoryComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    panel->setBounds (0, 35, getWidth() - 0, getHeight() - 85);
    revisionViewport->setBounds (1, 36, getWidth() - 2, getHeight() - 87);
    pushButton->setBounds (getWidth() - 8 - 96, 35 + (getHeight() - 85), 96, 48);
    pullButton->setBounds (0 + 8, 35 + (getHeight() - 85), 96, 48);
    revisionTreeLabel->setBounds (0, 14 - (32 / 2), 406, 32);
    shadow->setBounds (0 + 8, 35 + (getHeight() - 85), (getWidth() - 0) - 16, 16);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HistoryComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == pushButton)
    {
        //[UserButtonCode_pushButton] -- add your button handler code here..
        // just emitting a modal ui box.
        this->vcs.getRemote()->push();
        //[/UserButtonCode_pushButton]
    }
    else if (buttonThatWasClicked == pullButton)
    {
        //[UserButtonCode_pullButton] -- add your button handler code here..

        // проверит не просто наличие каких-либо изменений,
        // а то, есть ли изменения айтемов, которые уже присутствуют в индексе
        if (this->vcs.getHead().hasTrackedItemsOnTheStage())
        {
            //this->workspace.showTooltip(TRANS("vcs::history::forcepull::warning"), 3000);

            Component *confirmationDialog =
            new ModalDialogConfirmation(*this,
                                        TRANS("vcs::history::forcepull::confirmation"),
                                        TRANS("vcs::history::forcepull::proceed"),
                                        TRANS("vcs::history::forcepull::cancel"),
                                        CommandIDs::VersionControlForcePull,
                                        CommandIDs::Cancel);

            App::Layout().showModalNonOwnedDialog(confirmationDialog);
        }
        else
        {
            this->vcs.getRemote()->pull();
        }
        //[/UserButtonCode_pullButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void HistoryComponent::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::VersionControlForcePull)
    {
        this->vcs.getRemote()->pull();
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void HistoryComponent::rebuildRevisionTree()
{
    Component *revTree = new RevisionTreeComponent(this->vcs);
    auto alignerProxy = new ViewportFitProxyComponent(*this->revisionViewport, revTree, true); // deletes revTree
    this->revisionViewport->setViewedComponent(alignerProxy, true); // deletes alignerProxy
    alignerProxy->centerTargetToViewport();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HistoryComponent" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams="VersionControl &amp;owner"
                 variableInitialisers="vcs(owner)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <JUCERCOMP name="" id="fa0c0fc3d6eee313" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="0 35 0M 85M" sourceFile="../Themes/PanelA.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="34a64657988c0f04" memberName="revisionViewport" virtualName=""
                    explicitFocusOrder="0" pos="1 36 2M 87M" class="Viewport" params=""/>
  <TEXTBUTTON name="push" id="7855caa7c65c5c11" memberName="pushButton" virtualName=""
              explicitFocusOrder="0" pos="8Rr 0R 96 48" posRelativeY="fa0c0fc3d6eee313"
              buttonText="" connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="pull" id="31cda20020ea520c" memberName="pullButton" virtualName=""
              explicitFocusOrder="0" pos="8 0R 96 48" posRelativeX="fa0c0fc3d6eee313"
              posRelativeY="fa0c0fc3d6eee313" posRelativeW="fe1cf713999527d3"
              posRelativeH="fe1cf713999527d3" buttonText="" connectedEdges="4"
              needsCallback="1" radioGroupId="0"/>
  <LABEL name="" id="158da5e6e58ab3ae" memberName="revisionTreeLabel"
         virtualName="" explicitFocusOrder="0" pos="0 14c 406 32" edTextCol="ff000000"
         edBkgCol="0" labelText="vcs::history::caption" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <JUCERCOMP name="" id="34270fb50cf926d8" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="8 0R 16M 16" posRelativeX="fa0c0fc3d6eee313"
             posRelativeY="fa0c0fc3d6eee313" posRelativeW="fa0c0fc3d6eee313"
             sourceFile="../Themes/LightShadowDownwards.cpp" constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

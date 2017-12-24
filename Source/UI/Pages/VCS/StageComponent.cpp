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

#include "StageComponent.h"

//[MiscUserDefs]
#include "MainLayout.h"
#include "VersionControl.h"
#include "ModalDialogConfirmation.h"
#include "ModalDialogInput.h"
#include "ProgressIndicator.h"

#include "RevisionItemComponent.h"
#include "Head.h"

#include "CommandIDs.h"
#include "App.h"
#include "FailTooltip.h"

using namespace VCS;

#if HELIO_DESKTOP
#    define VCS_STAGE_ROW_HEIGHT (65)
#elif HELIO_MOBILE
#    define VCS_STAGE_ROW_HEIGHT (90)
#endif
//[/MiscUserDefs]

StageComponent::StageComponent(VersionControl &versionControl)
    : vcs(versionControl)
{
    addAndMakeVisible (toggleChangesButton = new TextButton ("toggleOn"));
    toggleChangesButton->setButtonText (String());
    toggleChangesButton->setConnectedEdges (Button::ConnectedOnTop);
    toggleChangesButton->addListener (this);

    addAndMakeVisible (horizontalCenter = new Component());

    addAndMakeVisible (commitButton = new TextButton ("commit"));
    commitButton->setButtonText (String());
    commitButton->setConnectedEdges (Button::ConnectedOnTop);
    commitButton->addListener (this);

    addAndMakeVisible (resetButton = new TextButton ("reset"));
    resetButton->setButtonText (String());
    resetButton->setConnectedEdges (Button::ConnectedOnTop);
    resetButton->addListener (this);

    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("vcs::stage::caption")));
    titleLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centredLeft);
    titleLabel->setEditable (false, false, false);

    addAndMakeVisible (shadow = new LightShadowDownwards());
    addAndMakeVisible (indicator = new ProgressIndicator());

    addAndMakeVisible (panel = new FramePanel());
    addAndMakeVisible (changesList = new ListBox ("", this));


    //[UserPreSize]
    this->indicator->setVisible(false);
    this->indicator->setAlpha(0.5f);

    this->changesList->getViewport()->setScrollBarThickness(2);
    this->changesList->setMultipleSelectionEnabled(true);
    //this->changesList->setColour(ListBox::backgroundColourId, Colours::palevioletred.withAlpha(0.05f));
    this->changesList->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->changesList->setRowHeight(VCS_STAGE_ROW_HEIGHT);
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    this->updateList();
    this->updateToggleButton();

    this->vcs.getHead().addChangeListener(this);
    //[/Constructor]
}

StageComponent::~StageComponent()
{
    //[Destructor_pre]
    this->vcs.getHead().removeChangeListener(this);
    //[/Destructor_pre]

    toggleChangesButton = nullptr;
    horizontalCenter = nullptr;
    commitButton = nullptr;
    resetButton = nullptr;
    titleLabel = nullptr;
    shadow = nullptr;
    indicator = nullptr;
    panel = nullptr;
    changesList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void StageComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void StageComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    toggleChangesButton->setBounds (8, 35 + (getHeight() - 85), 96, 48);
    horizontalCenter->setBounds (0, 0, proportionOfWidth (0.5072f), 8);
    commitButton->setBounds (getWidth() - 8 - 96, 35 + (getHeight() - 85), 96, 48);
    resetButton->setBounds ((getWidth() - 8 - 96) + -5 - 96, 35 + (getHeight() - 85), 96, 48);
    titleLabel->setBounds (0, 14 - (32 / 2), 406, 32);
    shadow->setBounds (8, 35 + (getHeight() - 85), getWidth() - 16, 16);
    indicator->setBounds ((getWidth() / 2) - (32 / 2), (getHeight() / 2) - (32 / 2), 32, 32);
    panel->setBounds (0, 35, getWidth() - 0, getHeight() - 85);
    changesList->setBounds (1, 36, getWidth() - 2, getHeight() - 87);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void StageComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == toggleChangesButton)
    {
        //[UserButtonCode_toggleChangesButton] -- add your button handler code here..
        this->toggleButtonAction();
        //[/UserButtonCode_toggleChangesButton]
    }
    else if (buttonThatWasClicked == commitButton)
    {
        //[UserButtonCode_commitButton] -- add your button handler code here..

        if (this->changesList->getSelectedRows().isEmpty())
        {
            App::Layout().showTooltip(TRANS("vcs::warning::cannotcommit"), 3000);
            return;
        }

        Component *inputDialog =
        new ModalDialogInput(*this,
                             this->commitMessage,
                             TRANS("dialog::vcs::commit::caption"),
                             TRANS("dialog::vcs::commit::proceed"),
                             TRANS("dialog::vcs::commit::cancel"),
                             CommandIDs::VersionControlCommit,
                             CommandIDs::Cancel);

        App::Layout().showModalNonOwnedDialog(inputDialog);

        //[/UserButtonCode_commitButton]
    }
    else if (buttonThatWasClicked == resetButton)
    {
        //[UserButtonCode_resetButton] -- add your button handler code here..

        if (this->changesList->getSelectedRows().isEmpty())
        {
            App::Layout().showTooltip(TRANS("vcs::warning::cannotreset"), 3000);
            return;
        }

        Component *confirmationDialog =
        new ModalDialogConfirmation(*this,
                                    TRANS("dialog::vcs::reset::caption"),
                                    TRANS("dialog::vcs::reset::proceed"),
                                    TRANS("dialog::vcs::reset::cancel"),
                                    CommandIDs::VersionControlReset,
                                    CommandIDs::Cancel);

        App::Layout().showModalNonOwnedDialog(confirmationDialog);

        //[/UserButtonCode_resetButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void StageComponent::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::VersionControlReset)
    {
        this->vcs.resetChanges(this->changesList->getSelectedRows());
    }
    else if (commandId == CommandIDs::VersionControlAmend)
    {
        // not implemented
        //this->vcs.amend(this->changesList->getSelectedRows(), this->commitMessage);
    }
    else if (commandId == CommandIDs::VersionControlCommit)
    {
        this->vcs.commit(this->changesList->getSelectedRows(), this->commitMessage);
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void StageComponent::changeListenerCallback(ChangeBroadcaster *source)
{
    if (Head *head = dynamic_cast<Head *>(source))
    {
        if (head->isRebuildingDiff())
        {
            this->startProgressAnimation();
            this->clearList();
        }
        else
        {
            this->stopProgressAnimation();
            this->updateList();
            this->updateToggleButton();
        }
    }
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *StageComponent::refreshComponentForRow(int rowNumber,
        bool isRowSelected, Component *existingComponentToUpdate)
{
    const ScopedReadLock lock(this->diffLock);

    // juce out-of-range fix
    const int numProps = this->lastDiff.getNumProperties();
    const bool isLastRow = (rowNumber == (numProps - 1));

    if (rowNumber >= numProps) { return existingComponentToUpdate; }

    const Identifier id = this->lastDiff.getPropertyName(rowNumber);
    const var property = this->lastDiff.getProperty(id);

    if (RevisionItem *revRecord = dynamic_cast<RevisionItem *>(property.getObject()))
    {
        if (existingComponentToUpdate != nullptr)
        {
            if (RevisionItemComponent *row = dynamic_cast<RevisionItemComponent *>(existingComponentToUpdate))
            {
                row->updateItemInfo(rowNumber, isLastRow, revRecord);
                return existingComponentToUpdate;
            }
        }
        else
        {
            auto row = new RevisionItemComponent(*this->changesList, this->vcs.getHead());
            row->updateItemInfo(rowNumber, isLastRow, revRecord);
            return row;
        }
    }

    return nullptr;
}

void StageComponent::listBoxItemClicked(int row, const MouseEvent &e)
{
}

void StageComponent::listBoxItemDoubleClicked(int row, const MouseEvent &e)
{
}

int StageComponent::getNumRows()
{
    const ScopedReadLock lock(this->diffLock);
    const int numProps = this->lastDiff.getNumProperties();
    return numProps;
}

void StageComponent::paintListBoxItem(int rowNumber, Graphics &g,
                                      int width, int height, bool rowIsSelected)
{
}


//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void StageComponent::updateToggleButton()
{
    // 4 use cases:
    // 1 - has no quick stash, and has changes - clearly can toggle off, display toggle on button
    // 2 - has quick stash, and no changes - clearly can toggle on, display toggle off button
    // 3 - has no quick stash, and has no changes - don't display toggle button
    // 4 - has quick stash, but also has changes - ? display toggle off button, show alert on press ?

    const bool case1 = !this->vcs.hasQuickStash() && this->vcs.getHead().hasAnythingOnTheStage();
    const bool case2 = this->vcs.hasQuickStash() && !this->vcs.getHead().hasAnythingOnTheStage();
    const bool case3 = !this->vcs.hasQuickStash() && !this->vcs.getHead().hasAnythingOnTheStage();
    const bool case4 = this->vcs.hasQuickStash() && this->vcs.getHead().hasAnythingOnTheStage();

    if (case1)
    {
        this->toggleChangesButton->setEnabled(true);
        //this->fader.fadeIn(this->toggleChangesButton, 100);
        this->toggleChangesButton->setName("toggleOn");
        this->toggleChangesButton->repaint();
    }
    else if (case2)
    {
        this->toggleChangesButton->setEnabled(true);
        //this->fader.fadeIn(this->toggleChangesButton, 100);
        this->toggleChangesButton->setName("toggleOff");
        this->toggleChangesButton->repaint();
    }
    else if (case3)
    {
        this->toggleChangesButton->setEnabled(false);
        //this->fader.fadeOut(this->toggleChangesButton, 100);
    }
    else if (case4)
    {
        this->toggleChangesButton->setEnabled(true);
        //this->fader.fadeIn(this->toggleChangesButton, 100);
        this->toggleChangesButton->setName("toggleOff");
        this->toggleChangesButton->repaint();
    }
}

void StageComponent::toggleButtonAction()
{
    const bool case1 = !this->vcs.hasQuickStash() && this->vcs.getHead().hasAnythingOnTheStage();
    const bool case2 = this->vcs.hasQuickStash() && !this->vcs.getHead().hasAnythingOnTheStage();
    const bool case3 = !this->vcs.hasQuickStash() && !this->vcs.getHead().hasAnythingOnTheStage();
    const bool case4 = this->vcs.hasQuickStash() && this->vcs.getHead().hasAnythingOnTheStage();

    if (case1)
    {
        this->vcs.quickStashAll();
    }
    else if (case2)
    {
        this->vcs.applyQuickStash();
        //this->vcs.getHead().rebuildDiffSynchronously();
    }
    else if (case4)
    {
        App::Helio()->showTooltip(TRANS("vcs::warning::cannotrevert"));
        App::Helio()->showModalComponent(new FailTooltip());
    }
}

void StageComponent::updateList()
{
    const ScopedWriteLock lock(this->diffLock);
    this->lastDiff = this->vcs.getHead().getDiff().createCopy();
    this->changesList->deselectAllRows();
    this->changesList->updateContent();
    this->repaint();
}

void StageComponent::clearList()
{
    const ScopedWriteLock lock(this->diffLock);
    this->lastDiff = ValueTree();
    this->changesList->deselectAllRows();
    this->changesList->updateContent();
    this->repaint();
}

void StageComponent::startProgressAnimation()
{
    this->fader.cancelAllAnimations(false);
    this->indicator->startAnimating();
    this->fader.fadeIn(this->indicator, 200);
}

void StageComponent::stopProgressAnimation()
{
    this->fader.cancelAllAnimations(false);
    this->indicator->stopAnimating();
    this->fader.fadeOut(this->indicator, 200);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="StageComponent" template="../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, public ChangeListener"
                 constructorParams="VersionControl &amp;versionControl" variableInitialisers="vcs(versionControl)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <TEXTBUTTON name="toggleOn" id="d8e2927825fe437e" memberName="toggleChangesButton"
              virtualName="" explicitFocusOrder="0" pos="8 0R 96 48" posRelativeY="83da04584c2ed03b"
              posRelativeW="fe1cf713999527d3" posRelativeH="fe1cf713999527d3"
              buttonText="" connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="" id="4ac6bf71d1e1d84f" memberName="horizontalCenter" virtualName=""
                    explicitFocusOrder="0" pos="0 0 50.719% 8" class="Component"
                    params=""/>
  <TEXTBUTTON name="commit" id="7855caa7c65c5c11" memberName="commitButton"
              virtualName="" explicitFocusOrder="0" pos="8Rr 0R 96 48" posRelativeY="83da04584c2ed03b"
              buttonText="" connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="reset" id="31cda20020ea520c" memberName="resetButton" virtualName=""
              explicitFocusOrder="0" pos="-5r 0R 96 48" posRelativeX="7855caa7c65c5c11"
              posRelativeY="83da04584c2ed03b" posRelativeW="fe1cf713999527d3"
              posRelativeH="fe1cf713999527d3" buttonText="" connectedEdges="4"
              needsCallback="1" radioGroupId="0"/>
  <LABEL name="" id="660583b19bbfaa6b" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="0 14c 406 32" labelText="vcs::stage::caption"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21" kerning="0" bold="0"
         italic="0" justification="33"/>
  <JUCERCOMP name="" id="34270fb50cf926d8" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="8 0R 16M 16" posRelativeX="43f5d7064958b68a"
             posRelativeY="83da04584c2ed03b" sourceFile="../../Themes/LightShadowDownwards.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="92641fd94a728225" memberName="indicator" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 32 32" class="ProgressIndicator"
                    params=""/>
  <JUCERCOMP name="" id="83da04584c2ed03b" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="0 35 0M 85M" sourceFile="../../Themes/FramePanel.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="d017e5395434bb4f" memberName="changesList" virtualName=""
                    explicitFocusOrder="0" pos="1 36 2M 87M" class="ListBox" params="&quot;&quot;, this"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

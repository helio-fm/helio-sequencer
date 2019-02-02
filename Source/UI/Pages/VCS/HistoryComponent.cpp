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
#include "VersionControl.h"
#include "VersionControlEditor.h"
#include "RevisionTreeComponent.h"
#include "ViewportFitProxyComponent.h"
#include "Revision.h"
#include "CommandIDs.h"
#include "ComponentIDs.h"
#include "Icons.h"

#include "MainLayout.h"
#include "ModalDialogConfirmation.h"
#include "VersionControlHistorySelectionMenu.h"

//[/MiscUserDefs]

HistoryComponent::HistoryComponent(VersionControl &owner)
    : vcs(owner)
{
    this->revisionViewport.reset(new Viewport());
    this->addAndMakeVisible(revisionViewport.get());

    this->revisionTreeLabel.reset(new Label(String(),
                                             TRANS("vcs::history::caption")));
    this->addAndMakeVisible(revisionTreeLabel.get());
    this->revisionTreeLabel->setFont(Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    revisionTreeLabel->setJustificationType(Justification::centred);
    revisionTreeLabel->setEditable(false, false, false);

    this->separator3.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator3.get());

    //[UserPreSize]
    this->setComponentID(ComponentIDs::versionControlHistory);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    this->vcs.fetchRevisionsIfNeeded();
    //[/Constructor]
}

HistoryComponent::~HistoryComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    revisionViewport = nullptr;
    revisionTreeLabel = nullptr;
    separator3 = nullptr;

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

    revisionViewport->setBounds(1, 42, getWidth() - 2, getHeight() - 43);
    revisionTreeLabel->setBounds(0, 0, getWidth() - 0, 26);
    separator3->setBounds((getWidth() / 2) - ((getWidth() - 0) / 2), 40, getWidth() - 0, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HistoryComponent::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    switch (commandId)
    {
    case CommandIDs::VersionControlCheckout:
        if (this->vcs.getHead().hasTrackedItemsOnTheStage())
        {
            auto confirmationDialog = ModalDialogConfirmation::Presets::forceCheckout();
            confirmationDialog->onOk = [this]()
            {
                this->vcs.checkout(this->revisionTree->getSelectedRevision());
            };
            App::Layout().showModalComponentUnowned(confirmationDialog.release());
        }
        else
        {
            this->vcs.checkout(this->revisionTree->getSelectedRevision());
        }
        break;
    case CommandIDs::VersionControlPushSelected:
    case CommandIDs::VersionControlPullSelected:
        // from sync thread's perspective, it doesn't make difference if user wants to push or pull a revision,
        // because it will perform the necessary action depending on the information it gets from the api:
        this->vcs.syncRevision(this->revisionTree->getSelectedRevision());
        break;
    default:
        break;
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

void HistoryComponent::clearSelection()
{
    if (this->revisionTree != nullptr)
    {
        this->revisionTree->deselectAll(false);
    }
}

void HistoryComponent::rebuildRevisionTree()
{
    this->revisionTree = new RevisionTreeComponent(this->vcs);
    auto *alignerProxy = new ViewportFitProxyComponent(*this->revisionViewport, this->revisionTree, true); // owns revisionTree
    this->revisionViewport->setViewedComponent(alignerProxy, true); // owns alignerProxy
    alignerProxy->centerTargetToViewport();
}

void HistoryComponent::onRevisionSelectionChanged()
{
    if (this->revisionTree != nullptr &&
        this->revisionTree->getSelectedRevision() != nullptr)
    {
        // Hide existing because selection caption will be always different:
        App::Layout().hideSelectionMenu();
        App::Layout().showSelectionMenu(this);
    }
    else
    {
        App::Layout().hideSelectionMenu();
    }

    if (auto *parent = dynamic_cast<VersionControlEditor *>(this->getParentComponent()))
    {
        parent->onHistorySelectionChanged();
    }
}

//===----------------------------------------------------------------------===//
// HeadlineItemDataSource
//===----------------------------------------------------------------------===//

bool HistoryComponent::hasMenu() const noexcept
{
    return true;
}

ScopedPointer<Component> HistoryComponent::createMenu()
{
    if (this->revisionTree != nullptr)
    {
        return { new VersionControlHistorySelectionMenu(this->revisionTree->getSelectedRevision(), this->vcs) };
    }

    jassertfalse;
    return nullptr;
}

Image HistoryComponent::getIcon() const
{
    return Icons::findByName(Icons::revision, HEADLINE_ICON_SIZE);
}

String HistoryComponent::getName() const
{
    if (this->revisionTree != nullptr &&
        this->revisionTree->getSelectedRevision() != nullptr)
    {
        return this->revisionTree->getSelectedRevision()->getMessage();
    }

    return TRANS("menu::selection::vcs::history");
}

bool HistoryComponent::canBeSelectedAsMenuItem() const
{
    return false;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HistoryComponent" template="../../../Template"
                 componentName="" parentClasses="public Component, public HeadlineItemDataSource"
                 constructorParams="VersionControl &amp;owner" variableInitialisers="vcs(owner)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <GENERICCOMPONENT name="" id="34a64657988c0f04" memberName="revisionViewport" virtualName=""
                    explicitFocusOrder="0" pos="1 42 2M 43M" class="Viewport" params=""/>
  <LABEL name="" id="158da5e6e58ab3ae" memberName="revisionTreeLabel"
         virtualName="" explicitFocusOrder="0" pos="0 0 0M 26" labelText="vcs::history::caption"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="a09914d60dab2768" memberName="separator3" virtualName=""
             explicitFocusOrder="0" pos="0Cc 40 0M 3" sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

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

#include "Common.h"
#include "TreePanel.h"

#include "RootTreeItemPanelDefault.h"
#include "RootTreeItemPanelCompact.h"

#include "App.h"
#include "MainLayout.h"
#include "ModalDialogInput.h"

#include "MidiTrackTreeItem.h"
#include "ProjectTreeItem.h"
#include "UndoStack.h"
#include "MidiTrackActions.h"
#include "ModalDialogInput.h"

#include "LongTapController.h"
#include "MidiTrackTreeItem.h"

#include "RolloverHeaderLeft.h"
#include "RolloverHeaderRight.h"
#include "RolloverContainer.h"

#include "CommandIDs.h"

TreePanel::TreePanel() : root(nullptr), lastRenamedItem(nullptr)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->setOpaque(true);
    this->setBufferedToImage(false);
    
    this->longTapRecognizer = new LongTapController(*this);
    this->addMouseListener(this->longTapRecognizer, true);
}

TreePanel::~TreePanel()
{
    this->removeMouseListener(this->longTapRecognizer);
}

void TreePanel::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::RootTreeItemPressed:
        {
            // root tree item selection logic
            //App::Layout().toggleShowTree();
            this->root->setSelected(true, true);
            break;
        }
            
        case CommandIDs::RenameLayer:
        {
            if (this->lastRenamedItem != nullptr)
            {
                if (ModalDialogInput *modal = 
                    dynamic_cast<ModalDialogInput *>(Component::getCurrentlyModalComponent(0)))
                {
                    return;
                }

                this->renameString = this->lastRenamedItem->getXPath();
                
                Component *inputDialog =
                new ModalDialogInput(*this,
                                     this->renameString,
                                     TRANS("dialog::renamelayer::caption"),
                                     TRANS("dialog::renamelayer::proceed"),
                                     TRANS("dialog::renamelayer::cancel"),
                                     CommandIDs::RenameLayerConfirmed,
                                     CommandIDs::Cancel);
                
                App::Layout().showModalNonOwnedDialog(inputDialog);
            }
            break;
        }
            
        case CommandIDs::RenameLayerConfirmed:
        {
            if (this->lastRenamedItem != nullptr)
            {
                ProjectTreeItem *project = this->lastRenamedItem->getProject();
                const String &layerId = this->lastRenamedItem->getSequence()->getLayerIdAsString();
                
                project->getUndoStack()->beginNewTransaction();
                project->getUndoStack()->perform(new MidiTrackRenameAction(*project, layerId, this->renameString));
                
                // instead of:
                //this->layerItem.onRename(this->renameString);
            }
        }
            break;
            
        case CommandIDs::Cancel:
            break;
            
        case CommandIDs::HideRollover:
            this->dismissCurrentRollover();
            break;
            
        default:
            break;
    }
}



void TreePanel::showRenameLayerDialogAsync(MidiTrackTreeItem *item)
{
    this->lastRenamedItem = item;
    this->postCommandMessage(CommandIDs::RenameLayer);
}

void TreePanel::longTapEvent(const MouseEvent &e)
{
    //    if (this->isCompactMode())
    //    {
    //        return;
    //    }
    //
    //    if (TreeView *tappedTree = e.eventComponent->findParentComponentOfClass<TreeView>())
    //    {
    //        if (TreeItem *tappedItem = dynamic_cast<TreeItem *>(tappedTree->getSelectedItem(0)))
    //        {
    //            PopupMenu::dismissAllActiveMenus();
    //            tappedItem->showPopupMenu();
    //        }
    //    }
}


bool TreePanel::isCompactMode() const
{
    return (this->getWidth() == TREE_COMPACT_WIDTH);
}

//===----------------------------------------------------------------------===//
// The callout replacement
//===----------------------------------------------------------------------===//

void TreePanel::emitRollover(Component *newTargetComponent,
                                    const String &headerTitle)
{
    Component *newHeader = new RolloverHeaderLeft(headerTitle);
    this->currentRollover = new RolloverContainer(newHeader, newTargetComponent);
    //const Rectangle<int> r1(-this->getWidth(), 0, this->getWidth(), this->tree->getBottom());
    const Rectangle<int> r1(this->getWidth(), 0, this->getWidth(), this->getWorkingArea().getBottom());
    const Rectangle<int> r2(0, 0, this->getWidth(), this->getWorkingArea().getBottom());
    
    this->addAndMakeVisible(this->currentRollover);
    this->currentRollover->setBounds(r1);
    this->currentRollover->setAlpha(0.5f);
    this->rolloverFader.animateComponent(this->currentRollover, r2, 1.0, 200, false, 1.0, 0.0);
}

void TreePanel::dismissCurrentRollover()
{
    if (this->currentRollover != nullptr)
    {
        //const Rectangle<int> r1(-this->getWidth(), 0, this->getWidth(), this->tree->getBottom());
        const Rectangle<int> r1(this->getWidth(), 0, this->getWidth(), this->getWorkingArea().getBottom());
        this->rolloverFader.animateComponent(this->currentRollover, r1, 0.0, 200, true, 0.0, 0.0);
        this->currentRollover = nullptr;
    }
}
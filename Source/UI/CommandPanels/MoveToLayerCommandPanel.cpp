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
#include "MoveToLayerCommandPanel.h"
#include "ProjectTreeItem.h"
#include "PianoRollToolbox.h"
#include "PianoTrackTreeItem.h"
#include "PianoRoll.h"
#include "NoteComponent.h"
#include "Icons.h"
#include "TriggersTrackMap.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Instrument.h"
#include "MidiSequence.h"
#include "InternalClipboard.h"
#include "App.h"
#include "CommandIDs.h"

//===----------------------------------------------------------------------===//
// Move-to-layer command panel
//

MoveToLayerCommandPanel::MoveToLayerCommandPanel(PianoRoll &targetRoll,
                                                 ProjectTreeItem &parentProject) :
    roll(targetRoll),
    project(parentProject)
{
    this->initLayersPanel(false);
}

MoveToLayerCommandPanel::~MoveToLayerCommandPanel()
{
}

void MoveToLayerCommandPanel::handleCommandMessage(int commandId)
{
    const Array<PianoTrackTreeItem *> &layerItems =
        this->project.findChildrenOfType<PianoTrackTreeItem>();
    
    if (commandId >= CommandIDs::MoveEventsToLayer &&
        commandId <= (CommandIDs::MoveEventsToLayer + layerItems.size()))
    {
        const int layerIndex = commandId - CommandIDs::MoveEventsToLayer;
        
        const Lasso::GroupedSelections &selections = this->roll.getLassoSelection().getGroupedSelections();
        const int numSelected = this->roll.getLassoSelection().getNumSelected();

		if (NoteComponent *note = dynamic_cast<NoteComponent *>(this->roll.getLassoSelection().getSelectedItem(0)))
		{
			const MidiSequence *layerOfFirstSelected = (numSelected > 0) ? (note->getNote().getLayer()) : nullptr;
			const bool hasMultiLayerSelection = (selections.size() > 1);
			const bool alreadyBelongsTo = hasMultiLayerSelection ? false : (layerItems[layerIndex]->getSequence() == layerOfFirstSelected);

			if (!alreadyBelongsTo)
			{
				//Logger::writeToLog("Moving notes to " + layerItems[layerIndex]->getXPath());
				PianoRollToolbox::moveToLayer(this->roll.getLassoSelection(), layerItems[layerIndex]->getSequence());
				layerItems[layerIndex]->setSelected(false, false, sendNotification);
				layerItems[layerIndex]->setSelected(true, true, sendNotification);
				this->dismiss();
			}
		}

        return;
    }
}

void MoveToLayerCommandPanel::initLayersPanel(bool shouldAddBackButton)
{
    ReferenceCountedArray<CommandItem> cmds;
    
    if (shouldAddBackButton)
    {
        cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));
    }
    
    const Array<PianoTrackTreeItem *> &layers =
        this->project.findChildrenOfType<PianoTrackTreeItem>();
    
    for (int i = 0; i < layers.size(); ++i)
    {
        const Lasso::GroupedSelections &selections = this->roll.getLassoSelection().getGroupedSelections();
        const int numSelected = this->roll.getLassoSelection().getNumSelected();
		const auto &event = this->roll.getLassoSelection().getFirstAs<NoteComponent>()->getNote();
        const MidiSequence *layerOfFirstSelected = (numSelected > 0) ? (event.getSequence()) : nullptr;
        const bool hasMultiLayerSelection = (selections.size() > 1);
        const bool belongsTo = hasMultiLayerSelection ? false : (layers.getUnchecked(i)->getSequence() == layerOfFirstSelected);
        
        const String name(layers.getUnchecked(i)->getXPath());
        const Colour colour(layers.getUnchecked(i)->getColour());
        cmds.add(CommandItem::withParams(belongsTo ? Icons::apply : Icons::layer, CommandIDs::MoveEventsToLayer + i, name)->colouredWith(colour));
    }
    
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void MoveToLayerCommandPanel::dismiss()
{
    this->roll.grabKeyboardFocus();
    this->getParentComponent()->exitModalState(0);
}

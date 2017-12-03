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
#include "PianoRollSelectionCommandPanel.h"
#include "ArpeggiatorsManager.h"
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

CommandPanel::Items createDefaultPanel()
{
    CommandPanel::Items cmds;
    cmds.add(CommandItem::withParams(Icons::copy, CommandIDs::CopyEvents, TRANS("menu::selection::piano::copy")));
    cmds.add(CommandItem::withParams(Icons::cut, CommandIDs::CutEvents, TRANS("menu::selection::piano::cut")));
    // todo cut with shifted time
    //cmds.add(CommandItem::withParams(Icons::cut, CommandIDs::CutEvents, TRANS("menu::selection::piano::cut")));
    //cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::RefactorRemoveOverlaps, TRANS("menu::selection::piano::cleanup")));

    // Refactor ->
    // Move To ->
    // Arpeggiate ->
    // Split ->

    cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteEvents, TRANS("menu::selection::piano::delete")));
    return cmds;
}

CommandPanel::Items createRefactoringPanel()
{
    CommandPanel::Items cmds;
    // TODO
    // Cleanup (remove overlaps)
    // Invert up
    // Invert down
    // Octave up
    // Octave down
    // Double time
    // Half time
    return cmds;
}

CommandPanel::Items createLayersPanel(const Array<PianoTrackTreeItem *> &layers)
{
    CommandPanel::Items cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back"))->withTimer());

    for (int i = 0; i < layers.size(); ++i)
    {
        const String name(layers.getUnchecked(i)->getXPath());
        const Colour colour(layers.getUnchecked(i)->getColour());
        cmds.add(CommandItem::withParams(Icons::layer, CommandIDs::MoveEventsToLayer + i, name)->colouredWith(colour));
    }

    return cmds;
}

CommandPanel::Items createArpsPanel(int selectedArp)
{
    CommandPanel::Items cmds;
    const Array<Arpeggiator> arps = ArpeggiatorsManager::getInstance().getArps();
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back"))->withTimer());

    for (int i = 0; i < arps.size(); ++i)
    {
        const bool isSelectedArp = (i == selectedArp);

        cmds.add(CommandItem::withParams(Icons::group,
            (CommandIDs::ApplyArpeggiator + i),
            arps.getUnchecked(i).getName())->toggled(isSelectedArp));
    }

    return cmds;
}








PianoRollSelectionCommandPanel::PianoRollSelectionCommandPanel(
    PianoRoll &targetRoll,
    ProjectTreeItem &parentProject) :
    roll(targetRoll),
    project(parentProject)
{
    this->initLayersPanel(false);
}

PianoRollSelectionCommandPanel::~PianoRollSelectionCommandPanel()
{
}

void PianoRollSelectionCommandPanel::handleCommandMessage(int commandId)
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
            const MidiSequence *layerOfFirstSelected = (numSelected > 0) ? (note->getNote().getSequence()) : nullptr;
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

void PianoRollSelectionCommandPanel::initLayersPanel(bool shouldAddBackButton)
{
    CommandPanel::Items cmds;
    
    if (shouldAddBackButton)
    {
        cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back"))->withTimer());
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

void PianoRollSelectionCommandPanel::dismiss()
{
    this->getParentComponent()->exitModalState(0);
}

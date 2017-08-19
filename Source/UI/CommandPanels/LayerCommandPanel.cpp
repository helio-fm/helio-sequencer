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
#include "LayerCommandPanel.h"
#include "MidiTrackTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "Icons.h"
#include "CommandIDs.h"

#include "MainLayout.h"
#include "AudioCore.h"
#include "Instrument.h"
#include "MidiSequence.h"
#include "HybridRoll.h"
#include "ProjectTreeItem.h"
#include "ModalDialogInput.h"

#include "MidiSequence.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "MidiTrackActions.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "UndoStack.h"
#include "TreePanel.h"
#include "Workspace.h"
#include "App.h"

LayerCommandPanel::LayerCommandPanel(MidiTrackTreeItem &parentLayer) :
    layerItem(parentLayer)
{
    this->initDefaultCommands();
}

LayerCommandPanel::~LayerCommandPanel()
{
}

void LayerCommandPanel::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::SelectAllEvents:
            
            if (ProjectTreeItem *project = this->layerItem.getProject())
            {
                if (HybridRoll *roll = dynamic_cast<HybridRoll *>(project->getLastFocusedRoll()))
                {
                    roll->selectAll();
                }
            }

            this->exit();
            break;

        case CommandIDs::SelectLayerColour:
            this->initColorSelection();
        break;
            
        case CommandIDs::MuteLayer:
        {
            ProjectTreeItem *project = this->layerItem.getProject();
            const String &layerId = this->layerItem.getSequence()->getLayerIdAsString();
            
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackMuteAction(*project, layerId, true));
            
            // instead of:
            //this->layerItem.getSequence()->setMuted(true);
            //this->layerItem.repaintItem();
            
            this->exit();
        }
            break;
            
        case CommandIDs::UnmuteLayer:
        {
            ProjectTreeItem *project = this->layerItem.getProject();
            const String &layerId = this->layerItem.getSequence()->getLayerIdAsString();
            
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackMuteAction(*project, layerId, false));
            
            // instead of:
            //this->layerItem.getSequence()->setMuted(false);
            //this->layerItem.repaintItem();
            
            this->exit();
        }
            break;

        case CommandIDs::SelectLayerInstrument:
            this->initInstrumentSelection();
            break;

        case CommandIDs::DuplicateLayerTo:
            this->initProjectSelection();
            break;

        case CommandIDs::RenameLayer:
        {
            if (TreePanel *panel = this->layerItem.findParentTreePanel())
            {
                panel->showRenameLayerDialogAsync(&this->layerItem);
                this->exit();
            }
            
            break;
        }

        case CommandIDs::DeleteLayer:
        {
            // instead of this:
            //TreeItem::deleteItem(&this->layerItem);

            ProjectTreeItem *project = this->layerItem.getProject();
            const String &layerId = this->layerItem.getSequence()->getLayerIdAsString();
            
            project->getUndoStack()->beginNewTransaction();
            
            if (dynamic_cast<PianoTrackTreeItem *>(&this->layerItem))
            {
                project->getUndoStack()->perform(new PianoTrackRemoveAction(*project, layerId));
            }
            else if (dynamic_cast<AutomationTrackTreeItem *>(&this->layerItem))
            {
                project->getUndoStack()->perform(new AutomationTrackRemoveAction(*project, layerId));
            }
            
            if (HybridRoll *roll = dynamic_cast<HybridRoll *>(project->getLastFocusedRoll()))
            {
                roll->grabKeyboardFocus();
            }
            
            this->getParentComponent()->exitModalState(0);
            return;
        }
            
        case CommandIDs::Back:
            this->initDefaultCommands();
            break;
    }

    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    
    if (commandId >= CommandIDs::SetLayerInstrument &&
        commandId <= (CommandIDs::SetLayerInstrument + info.size()))
    {
        const int instrumentIndex = commandId - CommandIDs::SetLayerInstrument;
        if (instrumentIndex >= 0 && instrumentIndex < info.size())
        {
            Logger::writeToLog(info[instrumentIndex]->getIdAndHash());
            
            ProjectTreeItem *project = this->layerItem.getProject();
            const String layerId = this->layerItem.getSequence()->getLayerIdAsString();
            const String instrumentId = info[instrumentIndex]->getIdAndHash();

            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackChangeInstrumentAction(*project, layerId, instrumentId));
            
            // instead of:
            //this->layerItem.getSequence()->setInstrumentId(instrumentId);
            
            this->initDefaultCommands();
            return;
        }
    }
    
    
    const StringPairArray colours(CommandPanel::getColoursList());
    
    if (commandId >= CommandIDs::SetLayerColour &&
        commandId <= (CommandIDs::SetLayerColour + colours.size()))
    {
        const int colourIndex = (commandId - CommandIDs::SetLayerColour);
        const String name(colours.getAllKeys()[colourIndex]);
        const Colour colour(Colour::fromString(colours[name]));
        
        if (colour != this->layerItem.getColour())
        {
            ProjectTreeItem *project = this->layerItem.getProject();
            const String layerId = this->layerItem.getSequence()->getLayerIdAsString();
            
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackChangeColourAction(*project, layerId, colour));

            // instead of:
            //this->layerItem.setColour(colour);
            
            this->initDefaultCommands();
            return;
        }
    }
}

void LayerCommandPanel::initDefaultCommands()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::paste, CommandIDs::SelectAllEvents, TRANS("menu::layer::selectall")));
    cmds.add(CommandItem::withParams(Icons::colour, CommandIDs::SelectLayerColour, TRANS("menu::layer::change::colour"))->withSubmenu());
    
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    const int numInstruments = info.size();

    if (numInstruments > 1)
    {
        cmds.add(CommandItem::withParams(Icons::saxophone, CommandIDs::SelectLayerInstrument, TRANS("menu::layer::change::instrument"))->withSubmenu());
    }
    
    cmds.add(CommandItem::withParams(Icons::ellipsis, CommandIDs::RenameLayer, TRANS("menu::layer::rename")));
    
    //MainLayout &workspace = this->layerItem.getWorkspace();
    //Array<ProjectTreeItem *> projects = workspace.getLoadedProjects();
    //const int numProjects = projects.size();
    
    //if (numProjects > 1)
    //{
    //    cmds.add(CommandItem::withParams(Icons::copy, DuplicateTo, TRANS("menu::layer::copytoproject"))->withSubmenu());
    //}

    const bool canBeMuted = (dynamic_cast<PianoTrackTreeItem *>(&this->layerItem) != nullptr);
    
    if (canBeMuted)
    {
        const bool muted = this->layerItem.getSequence()->isMuted();
        
        if (muted)
        {
            cmds.add(CommandItem::withParams(Icons::volumeUp, CommandIDs::UnmuteLayer, TRANS("menu::layer::unmute")));
        }
        else
        {
            cmds.add(CommandItem::withParams(Icons::volumeOff, CommandIDs::MuteLayer, TRANS("menu::layer::mute")));
        }
    }
    
    cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteLayer, TRANS("menu::layer::delete")));
    this->updateContent(cmds, CommandPanel::SlideRight);
}

void LayerCommandPanel::initColorSelection()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));
    
    const StringPairArray colours(CommandPanel::getColoursList());
    
    for (int i = 0; i < colours.getAllKeys().size(); ++i)
    {
        const String name(colours.getAllKeys()[i]);
        const Colour colour(Colour::fromString(colours[name]));
        const bool isSelected = (colour == this->layerItem.getSequence()->getColour());
        cmds.add(CommandItem::withParams(isSelected ? Icons::apply : Icons::colour, CommandIDs::SetLayerColour + i, name)->colouredWith(colour));
    }

    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void LayerCommandPanel::initInstrumentSelection()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));
    
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    const Instrument *selectedInstrument = App::Workspace().getAudioCore().findInstrumentById(this->layerItem.getSequence()->getInstrumentId());
    const bool hasSubmenu = (info.size() > 5);
    
    for (int i = 0; i < info.size(); ++i)
    {
        const bool isTicked = (info[i] == selectedInstrument);
        cmds.add(CommandItem::withParams(isTicked ? Icons::apply : Icons::saxophone, CommandIDs::SetLayerInstrument + i, info[i]->getName()));
    }
    
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void LayerCommandPanel::initProjectSelection()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));

    const ProjectTreeItem *currentProject = this->layerItem.getProject();
    Array<ProjectTreeItem *> projects = App::Workspace().getLoadedProjects();
    
    for (int i = 0; i < projects.size(); ++i)
    {
        const bool isTicked = (projects[i] == currentProject);
        cmds.add(CommandItem::withParams(isTicked ? Icons::apply : Icons::project, CommandIDs::MoveLayerToProject + i, projects[i]->getName()));
    }
    
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void LayerCommandPanel::exit()
{
    if (ProjectTreeItem *project = this->layerItem.getProject())
    {
        if (HybridRoll *roll = dynamic_cast<HybridRoll *>(project->getLastFocusedRoll()))
        {
            roll->grabKeyboardFocus();
        }
    }
    
    this->getParentComponent()->exitModalState(0);
}


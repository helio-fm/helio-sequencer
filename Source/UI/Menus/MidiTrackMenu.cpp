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
#include "MidiTrackMenu.h"
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
#include "Workspace.h"
#include "App.h"

MidiTrackMenu::MidiTrackMenu(MidiTrackTreeItem &parentLayer) :
    trackItem(parentLayer)
{
    this->initDefaultCommands();
}

void MidiTrackMenu::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::SelectTrackColour:
            this->initColorSelection();
        break;
            
        case CommandIDs::MuteTrack:
        {
            ProjectTreeItem *project = this->trackItem.getProject();
            const String &layerId = this->trackItem.getSequence()->getTrackId();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackMuteAction(*project, layerId, true));
            this->dismiss();
        }
            break;
            
        case CommandIDs::UnmuteTrack:
        {
            ProjectTreeItem *project = this->trackItem.getProject();
            const String &layerId = this->trackItem.getSequence()->getTrackId();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackMuteAction(*project, layerId, false));
            this->dismiss();
        }
            break;

        case CommandIDs::SelectTrackInstrument:
            this->initInstrumentSelection();
            break;

        case CommandIDs::DuplicateTrackTo:
            this->initProjectSelection();
            break;

        case CommandIDs::RenameTrack:
        {
            auto inputDialog = ModalDialogInput::Presets::renameTrack(this->trackItem.getXPath());
            inputDialog->onOk = this->trackItem.getRenameCallback();
            App::Layout().showModalComponentUnowned(inputDialog.release());
            this->dismiss();
            break;
        }

        case CommandIDs::DeleteTrack:
        {
            ProjectTreeItem *project = this->trackItem.getProject();
            const String &layerId = this->trackItem.getSequence()->getTrackId();
            
            project->getUndoStack()->beginNewTransaction();
            
            if (dynamic_cast<PianoTrackTreeItem *>(&this->trackItem))
            {
                project->getUndoStack()->perform(new PianoTrackRemoveAction(*project, project, layerId));
            }
            else if (dynamic_cast<AutomationTrackTreeItem *>(&this->trackItem))
            {
                project->getUndoStack()->perform(new AutomationTrackRemoveAction(*project, project, layerId));
            }
            
            this->dismiss();
            return;
        }
            
        case CommandIDs::Back:
            this->initDefaultCommands();
            break;
    }

    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    
    if (commandId >= CommandIDs::SetTrackInstrument &&
        commandId <= (CommandIDs::SetTrackInstrument + info.size()))
    {
        const int instrumentIndex = commandId - CommandIDs::SetTrackInstrument;
        if (instrumentIndex >= 0 && instrumentIndex < info.size())
        {
            Logger::writeToLog(info[instrumentIndex]->getIdAndHash());
            const String instrumentId = info[instrumentIndex]->getIdAndHash();
            this->trackItem.getChangeInstrumentCallback()(instrumentId);
            this->initDefaultCommands();
            return;
        }
    }
    
    const StringPairArray colours(MenuPanel::getColoursList());
    
    if (commandId >= CommandIDs::SetTrackColour &&
        commandId <= (CommandIDs::SetTrackColour + colours.size()))
    {
        const int colourIndex = (commandId - CommandIDs::SetTrackColour);
        const String name(colours.getAllKeys()[colourIndex]);
        const String colour(colours[name]);
        this->trackItem.getChangeColourCallback()(colour);
        this->initDefaultCommands();
        return;
    }
}

void MidiTrackMenu::initDefaultCommands()
{
    MenuPanel::Menu cmds;
    cmds.add(MenuItem::item(Icons::selectAll, CommandIDs::SelectAllEvents, TRANS("menu::track::selectall"))->closesMenu());
    cmds.add(MenuItem::item(Icons::colour, CommandIDs::SelectTrackColour, TRANS("menu::track::change::colour"))->withSubmenu());
    
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    const int numInstruments = info.size();

    if (numInstruments > 1)
    {
        cmds.add(MenuItem::item(Icons::instrument, CommandIDs::SelectTrackInstrument, TRANS("menu::track::change::instrument"))->withSubmenu());
    }
    
    cmds.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack, TRANS("menu::track::rename")));
    
    //MainLayout &workspace = this->trackItem.getWorkspace();
    //Array<ProjectTreeItem *> projects = workspace.getLoadedProjects();
    //const int numProjects = projects.size();
    
    //if (numProjects > 1)
    //{
    //    cmds.add(MenuItem::item(Icons::copy, DuplicateTo, TRANS("menu::track::copytoproject"))->withSubmenu());
    //}

    const bool canBeMuted = (dynamic_cast<PianoTrackTreeItem *>(&this->trackItem) != nullptr);
    
    if (canBeMuted)
    {
        const bool muted = this->trackItem.isTrackMuted();
        
        if (muted)
        {
            cmds.add(MenuItem::item(Icons::unmute, CommandIDs::UnmuteTrack, TRANS("menu::track::unmute")));
        }
        else
        {
            cmds.add(MenuItem::item(Icons::mute, CommandIDs::MuteTrack, TRANS("menu::track::mute")));
        }
    }
    
    cmds.add(MenuItem::item(Icons::remove, CommandIDs::DeleteTrack, TRANS("menu::track::delete")));
    this->updateContent(cmds, MenuPanel::SlideRight);
}

void MidiTrackMenu::initColorSelection()
{
    MenuPanel::Menu cmds;
    cmds.add(MenuItem::item(Icons::back, CommandIDs::Back, TRANS("menu::back"))->withTimer());
    
    const StringPairArray colours(MenuPanel::getColoursList());
    
    for (int i = 0; i < colours.getAllKeys().size(); ++i)
    {
        const String name(colours.getAllKeys()[i]);
        const Colour colour(Colour::fromString(colours[name]));
        const bool isSelected = (colour == this->trackItem.getTrackColour());
        cmds.add(MenuItem::item(isSelected ? Icons::apply : Icons::colour, CommandIDs::SetTrackColour + i, name)->colouredWith(colour));
    }

    this->updateContent(cmds, MenuPanel::SlideLeft);
}

void MidiTrackMenu::initInstrumentSelection()
{
    MenuPanel::Menu cmds;
    cmds.add(MenuItem::item(Icons::back, CommandIDs::Back, TRANS("menu::back"))->withTimer());
    
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    const Instrument *selectedInstrument = App::Workspace().getAudioCore().findInstrumentById(this->trackItem.getTrackInstrumentId());
    const bool hasSubmenu = (info.size() > 5);
    
    for (int i = 0; i < info.size(); ++i)
    {
        const bool isTicked = (info[i] == selectedInstrument);
        cmds.add(MenuItem::item(isTicked ? Icons::apply : Icons::instrument, CommandIDs::SetTrackInstrument + i, info[i]->getName()));
    }
    
    this->updateContent(cmds, MenuPanel::SlideLeft);
}

void MidiTrackMenu::initProjectSelection()
{
    MenuPanel::Menu cmds;
    cmds.add(MenuItem::item(Icons::back, CommandIDs::Back, TRANS("menu::back"))->withTimer());

    const ProjectTreeItem *currentProject = this->trackItem.getProject();
    Array<ProjectTreeItem *> projects = App::Workspace().getLoadedProjects();
    
    for (int i = 0; i < projects.size(); ++i)
    {
        const bool isTicked = (projects[i] == currentProject);
        cmds.add(MenuItem::item(isTicked ? Icons::apply :
            Icons::project, CommandIDs::MoveTrackToProject + i, projects[i]->getName()));
    }
    
    this->updateContent(cmds, MenuPanel::SlideLeft);
}

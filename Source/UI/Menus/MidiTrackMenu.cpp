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
#include "MidiTrackNode.h"
#include "PianoTrackNode.h"
#include "Icons.h"
#include "CommandIDs.h"

#include "MainLayout.h"
#include "AudioCore.h"
#include "Instrument.h"
#include "MidiSequence.h"
#include "HybridRoll.h"
#include "ProjectNode.h"
#include "ModalDialogInput.h"

#include "MidiSequence.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "MidiTrackActions.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "UndoStack.h"
#include "Workspace.h"

MidiTrackMenu::MidiTrackMenu(MidiTrackNode &parentLayer) :
    trackItem(parentLayer)
{
    this->initDefaultMenu();
}

void MidiTrackMenu::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        // TODO move to another command processor
        case CommandIDs::MuteTrack:
        {
            ProjectNode *project = this->trackItem.getProject();
            const String &trackId = this->trackItem.getSequence()->getTrackId();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackMuteAction(*project, trackId, true));
            this->dismiss();
        }
            break;
            
        case CommandIDs::UnmuteTrack:
        {
            ProjectNode *project = this->trackItem.getProject();
            const String &trackId = this->trackItem.getSequence()->getTrackId();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackMuteAction(*project, trackId, false));
            this->dismiss();
        }
            break;

        case CommandIDs::DeleteTrack:
        {
            ProjectNode *project = this->trackItem.getProject();
            project->checkpoint();
            project->removeTrack(this->trackItem);
            this->dismiss();
            return;
        }
    }
}

void MidiTrackMenu::initDefaultMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::selectAll, CommandIDs::SelectAllEvents, TRANS("menu::track::selectall"))->closesMenu());
    menu.add(MenuItem::item(Icons::colour, TRANS("menu::track::change::colour"))->withSubmenu()->withAction([this]()
    {
        this->initColorSelectionMenu();
    }));
    
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    const int numInstruments = info.size();
    if (numInstruments > 1)
    {
        menu.add(MenuItem::item(Icons::instrument, TRANS("menu::track::change::instrument"))->withSubmenu()->withAction([this]()
        {
            this->initInstrumentSelectionMenu();
        }));
    }
    
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack,
        TRANS("menu::track::rename"))->closesMenu());
    
    const bool canBeMuted = (dynamic_cast<PianoTrackNode *>(&this->trackItem) != nullptr);
    if (canBeMuted)
    {
        const bool muted = this->trackItem.isTrackMuted();
        
        if (muted)
        {
            menu.add(MenuItem::item(Icons::unmute, CommandIDs::UnmuteTrack, TRANS("menu::track::unmute")));
        }
        else
        {
            menu.add(MenuItem::item(Icons::mute, CommandIDs::MuteTrack, TRANS("menu::track::mute")));
        }
    }
    
    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteTrack, TRANS("menu::track::delete")));
    this->updateContent(menu, MenuPanel::SlideRight);
}

void MidiTrackMenu::initColorSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->initDefaultMenu();
    }));
    
    const StringPairArray colours(MenuPanel::getColoursList());
    for (int i = 0; i < colours.getAllKeys().size(); ++i)
    {
        const String name(colours.getAllKeys()[i]);
        const String colourString(colours[name]);
        const Colour colour(Colour::fromString(colourString));
        const bool isSelected = (colour == this->trackItem.getTrackColour());
        menu.add(MenuItem::item(isSelected ? Icons::apply : Icons::colour, name)->
            colouredWith(colour)->withAction([this, colourString]()
        {
            this->trackItem.getChangeColourCallback()(colourString);
            this->initDefaultMenu();
        }));
    }

    this->updateContent(menu, MenuPanel::SlideLeft);
}

void MidiTrackMenu::initInstrumentSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->initDefaultMenu();
    }));
    
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    const Instrument *selectedInstrument = App::Workspace().getAudioCore().findInstrumentById(this->trackItem.getTrackInstrumentId());
    const bool hasSubmenu = (info.size() > 5);
    
    for (int i = 0; i < info.size(); ++i)
    {
        const bool isTicked = (info[i] == selectedInstrument);
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::instrument, info[i]->getName())->withAction([this, instrument = info[i]]()
        {
            DBG(instrument->getIdAndHash());
            const String instrumentId = instrument->getIdAndHash();
            this->trackItem.getChangeInstrumentCallback()(instrumentId);
            this->initDefaultMenu();
            return;
        }));
    }
    
    this->updateContent(menu, MenuPanel::SlideLeft);
}

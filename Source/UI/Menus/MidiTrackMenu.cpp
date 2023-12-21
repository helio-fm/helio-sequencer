/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "MidiTrackMenu.h"
#include "MidiTrackNode.h"

#include "MainLayout.h"
#include "AudioCore.h"
#include "MidiSequence.h"
#include "RollBase.h"

#include "Workspace.h"

MidiTrackMenu::MidiTrackMenu(WeakReference<MidiTrack> track, WeakReference<UndoStack> undoStack) :
    track(track),
    undoStack(undoStack)
{
    this->initDefaultMenu();
}

void MidiTrackMenu::initDefaultMenu()
{
    MenuPanel::Menu menu;

#if PLATFORM_MOBILE
    menu.add(MenuItem::item(Icons::selectAll, CommandIDs::SelectAllEvents,
        TRANS(I18n::Menu::trackSelectall))->closesMenu());
#endif

    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack,
        TRANS(I18n::Menu::trackRename))->closesMenu());

    const auto hasTs = this->track->hasTimeSignatureOverride();
    menu.add(MenuItem::item(Icons::meter, CommandIDs::SetTrackTimeSignature,
        hasTs ? TRANS(I18n::Menu::timeSignatureChange) : TRANS(I18n::Menu::timeSignatureAdd))->
        closesMenu());

    menu.add(MenuItem::item(Icons::copy, CommandIDs::DuplicateTrack,
        TRANS(I18n::Menu::trackDuplicate))->closesMenu());

#if PLATFORM_MOBILE
    menu.add(MenuItem::item(Icons::remove,
        CommandIDs::DeleteTrack, TRANS(I18n::Menu::trackDelete)));
#endif

    menu.add(MenuItem::item(Icons::list, TRANS(I18n::Menu::trackChangeChannel))->
        withSubmenu()->withAction([this]()
        {
            this->initChannelSelectionMenu();
        }));

    const auto instruments = App::Workspace().getAudioCore().getInstrumentsExceptInternal();
    menu.add(MenuItem::item(Icons::instrument, TRANS(I18n::Menu::trackChangeInstrument))->
        disabledIf(instruments.isEmpty())->withSubmenu()->withAction([this]()
    {
        this->initInstrumentSelectionMenu();
    }));

    const auto trackInstrumentId = this->track->getTrackInstrumentId();
    for (const auto *instrument : instruments)
    {
        // well, the track can have an instrument which has no window; but here,
        // in the menu constructor, it's too expensive to check if this is the case,
        // so we'll only enable this menu item, if the track has a valid instrument assigned:
        if (instrument->getIdAndHash() == trackInstrumentId)
        {
            const auto editInstrumentCaption = instrument->getName() + ": " + TRANS(I18n::Menu::instrumentShowWindow);
            menu.add(MenuItem::item(Icons::instrument, CommandIDs::EditCurrentInstrument, editInstrumentCaption)->
                disabledIf(trackInstrumentId.isEmpty() || !instrument->isValid())->
                closesMenu());
            break;
        }
    }

    this->updateContent(menu, MenuPanel::SlideRight);
}

void MidiTrackMenu::initChannelSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->initDefaultMenu();
    }));

    for (int channel = 1; channel <= Globals::numChannels; ++channel)
    {
        const bool isTicked = this->track->getTrackChannel() == channel;
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::ellipsis, String(channel))->
            disabledIf(isTicked)->
            withAction([this, channel]()
            {
                this->undoStack->beginNewTransaction();
                this->track->setTrackChannel(channel, true, sendNotification);
                this->initDefaultMenu();
            }));
    }

    this->updateContent(menu, MenuPanel::SlideLeft);
}

void MidiTrackMenu::initInstrumentSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->initDefaultMenu();
    }));
    
    const auto &audioCore = App::Workspace().getAudioCore();
    const auto *selectedInstrument = audioCore.findInstrumentById(this->track->getTrackInstrumentId());

    for (const auto *instrument : audioCore.getInstrumentsExceptInternal())
    {
        const bool isTicked = instrument == selectedInstrument;
        const String instrumentId = instrument->getIdAndHash();
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::instrument, instrument->getName())->
            disabledIf(!instrument->isValid() || isTicked)->
            withAction([this, instrumentId]()
        {
            this->undoStack->beginNewTransaction();
            this->track->setTrackInstrumentId(instrumentId, true, sendNotification);
            this->initDefaultMenu();
        }));
    }
    
    this->updateContent(menu, MenuPanel::SlideLeft);
}

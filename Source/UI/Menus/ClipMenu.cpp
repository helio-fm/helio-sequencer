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
#include "ClipMenu.h"
#include "MidiTrackNode.h"

#include "MainLayout.h"
#include "AudioCore.h"
#include "MidiSequence.h"
#include "RollBase.h"

#include "Workspace.h"

ClipMenu::ClipMenu(const Clip &clip, WeakReference<UndoStack> undoStack) :
    clip(clip),
    undoStack(undoStack)
{
    jassert(clip.isValid());
    this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
}

MenuPanel::Menu ClipMenu::makeDefaultMenu()
{
    MenuPanel::Menu menu;

    // big part of this menu duplicates the menu code
    // in PatternRollSelectionMenu with minor differences

    jassert(clip.isValid());
    const auto *track = this->clip.getPattern()->getTrack();

#if PLATFORM_MOBILE
    menu.add(MenuItem::item(Icons::selectAll, CommandIDs::SelectAllEvents,
        TRANS(I18n::Menu::trackSelectall))->closesMenu());
#endif

    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack,
        TRANS(I18n::Menu::trackRename))->closesMenu());

    const auto hasTs = track->hasTimeSignatureOverride();
    menu.add(MenuItem::item(Icons::meter, CommandIDs::SetTrackTimeSignature,
        hasTs ? TRANS(I18n::Menu::timeSignatureChange) : TRANS(I18n::Menu::timeSignatureAdd))->
        closesMenu());

    const auto muteAction = this->clip.isMuted() ?
        TRANS(I18n::Menu::unmute) : TRANS(I18n::Menu::mute);
    menu.add(MenuItem::item(Icons::mute,
        CommandIDs::ToggleMuteClips, muteAction)->closesMenu());

    const auto soloAction = this->clip.isSoloed() ?
        TRANS(I18n::Menu::unsolo) : TRANS(I18n::Menu::solo);
    menu.add(MenuItem::item(Icons::unmute,
        CommandIDs::ToggleSoloClips, soloAction)->
        disabledIf(!this->clip.canBeSoloed())->
        closesMenu());

    menu.add(MenuItem::item(Icons::copy, CommandIDs::DuplicateTrack,
        TRANS(I18n::Menu::trackDuplicate))->closesMenu());

#if PLATFORM_MOBILE
    menu.add(MenuItem::item(Icons::remove,
        CommandIDs::DeleteTrack, TRANS(I18n::Menu::trackDelete)));
#endif

    menu.add(MenuItem::item(Icons::reprise,
        CommandIDs::ToggleLoopOverSelection,
        TRANS(I18n::CommandPalette::toggleLoopOverSelection))->closesMenu());

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesQuantizeTo))->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->makeQuantizationMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::list, TRANS(I18n::Menu::trackChangeChannel))->
        withSubmenu()->withAction([this]()
    {
        this->updateContent(this->makeChannelSelectionMenu(), MenuPanel::SlideLeft);
    }));

    const auto instruments = App::Workspace().getAudioCore().getInstrumentsExceptInternal();
    menu.add(MenuItem::item(Icons::instrument, TRANS(I18n::Menu::trackChangeInstrument))->
        disabledIf(instruments.isEmpty())->withSubmenu()->withAction([this]()
    {
        this->updateContent(this->makeInstrumentSelectionMenu(), MenuPanel::SlideLeft);
    }));

    const auto trackInstrumentId = track->getTrackInstrumentId();
    for (const auto *instrument : instruments)
    {
        if (instrument->getIdAndHash() == trackInstrumentId)
        {
            if (auto mainNode = instrument->findMainPluginNode())
            {
                const auto editInstrumentCaption = instrument->getName() + ": " + TRANS(I18n::Menu::instrumentShowWindow);
                menu.add(MenuItem::item(Icons::instrument, CommandIDs::EditCurrentInstrument, editInstrumentCaption)->
                    disabledIf(!instrument->isValid())->
                    closesMenu());
            }

            break;
        }
    }

    return menu;
}

MenuPanel::Menu ClipMenu::makeQuantizationMenu()
{
    MenuPanel::Menu menu;

    using namespace I18n::Menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));

#define CLIP_QUANTIZE_ITEM(cmd) \
    MenuItem::item(Icons::ellipsis, cmd, \
        TRANS(CommandIDs::getTranslationKeyFor(cmd)))->closesMenu()

    menu.add(CLIP_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_1));
    menu.add(CLIP_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_2));
    menu.add(CLIP_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_4));
    menu.add(CLIP_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_8));
    menu.add(CLIP_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_16));
    menu.add(CLIP_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_32));

#undef CLIP_QUANTIZE_ITEM

    return menu;
}

MenuPanel::Menu ClipMenu::makeChannelSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));

    jassert(clip.isValid());
    auto *track = this->clip.getPattern()->getTrack();

    for (int channel = 1; channel <= Globals::numChannels; ++channel)
    {
        const bool isTicked = track->getTrackChannel() == channel;
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::ellipsis, String(channel))->
            disabledIf(isTicked)->
            withAction([this, track, channel]()
        {
            this->undoStack->beginNewTransaction();
            track->setTrackChannel(channel, true, sendNotification);
            this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
        }));
    }

    return menu;
}

MenuPanel::Menu ClipMenu::makeInstrumentSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));
    
    jassert(clip.isValid());
    auto *track = this->clip.getPattern()->getTrack();

    const auto &audioCore = App::Workspace().getAudioCore();
    const auto *selectedInstrument = audioCore.findInstrumentById(track->getTrackInstrumentId());

    for (const auto *instrument : audioCore.getInstrumentsExceptInternal())
    {
        const bool isTicked = instrument == selectedInstrument;
        const String instrumentId = instrument->getIdAndHash();
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::instrument, instrument->getName())->
            disabledIf(!instrument->isValid() || isTicked)->
            withAction([this, track, instrumentId]()
        {
            this->undoStack->beginNewTransaction();
            track->setTrackInstrumentId(instrumentId, true, sendNotification);
            this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
        }));
    }
    
    return menu;
}

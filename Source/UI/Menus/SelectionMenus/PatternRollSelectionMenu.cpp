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
#include "PatternRollSelectionMenu.h"

#include "ClipComponent.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "Lasso.h"

#include "PatternOperations.h"
#include "SequencerOperations.h"

#include "ProjectNode.h"
#include "UndoStack.h"
#include "PianoTrackNode.h"
#include "AutomationSequence.h"
#include "PianoSequence.h"

#include "TempoDialog.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "CommandIDs.h"

PatternRollSelectionMenu::PatternRollSelectionMenu(WeakReference<Lasso> lasso) :
    lasso(lasso)
{
    if (lasso->getNumSelected() > 0)
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }
}

static bool canRenamePatternSelection(WeakReference<Lasso> lasso)
{
    const auto trackId = lasso->getFirstAs<ClipComponent>()->getClip().getTrackId();
    for (int i = 0; i < lasso->getNumSelected(); ++i)
    {
        if (lasso->getItemAs<ClipComponent>(i)->getClip().getTrackId() != trackId)
        {
            return false;
        }
    }
    return true;
}

static bool canTriggerSoloForPatternSelection(WeakReference<Lasso> lasso)
{
    for (int i = 0; i < lasso->getNumSelected(); ++i)
    {
        const auto &clip = lasso->getItemAs<ClipComponent>(i)->getClip();
        if (clip.isSoloed() || clip.canBeSoloed())
        {
            return true;
        }
    }
    return false;
}

MenuPanel::Menu PatternRollSelectionMenu::createDefaultMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::zoomToFit, CommandIDs::ZoomEntireClip,
        TRANS(I18n::Menu::Selection::clipsEdit))->
        disabledIf(lasso->getNumSelected() == 0)->
        closesMenu());
    
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack,
        TRANS(I18n::Menu::trackRename))->
        disabledIf(!canRenamePatternSelection(this->lasso))->
        closesMenu());

    if (this->lasso->getNumSelected() == 1)
    {
        auto *track = this->lasso->getFirstAs<ClipComponent>()->getClip().getPattern()->getTrack();

        const auto tsActionlabel = track->hasTimeSignatureOverride() ?
            TRANS(I18n::Menu::timeSignatureChange) :
            TRANS(I18n::Menu::timeSignatureAdd);

        menu.add(MenuItem::item(Icons::meter,
            CommandIDs::SetTrackTimeSignature, tsActionlabel)->closesMenu());

        if (track->isTempoTrack())
        {
            menu.add(MenuItem::item(Icons::automationTrack,
                CommandIDs::TrackSetOneTempo, TRANS(I18n::Menu::setOneTempo))->closesMenu());
        }
    }

#if PLATFORM_DESKTOP
    if (this->lasso->getNumSelected() > 1)
#endif
    {
        bool hasAtLeastOnePianoTrack = false;
        for (int i = 0; i < this->lasso->getNumSelected(); ++i)
        {
            auto *track = this->lasso->getItemAs<ClipComponent>(i)->getClip().getPattern()->getTrack();
            hasAtLeastOnePianoTrack = hasAtLeastOnePianoTrack || (nullptr != dynamic_cast<PianoTrackNode *>(track));
        }

        if (hasAtLeastOnePianoTrack)
        {
            menu.add(MenuItem::item(Icons::up, CommandIDs::ClipTransposeUp,
                TRANS(I18n::Menu::Selection::clipsTransposeUp)));

            menu.add(MenuItem::item(Icons::down, CommandIDs::ClipTransposeDown,
                TRANS(I18n::Menu::Selection::clipsTransposeDown)));
        }
    }

    const auto muteAction = PatternOperations::lassoContainsMutedClip(*this->lasso.get()) ?
        TRANS(I18n::Menu::unmute) : TRANS(I18n::Menu::mute);

    const auto canSolo = canTriggerSoloForPatternSelection(this->lasso);
    const auto soloAction = PatternOperations::lassoContainsSoloedClip(*this->lasso.get()) ?
        TRANS(I18n::Menu::unsolo) : TRANS(I18n::Menu::solo);

    menu.add(MenuItem::item(Icons::mute,
        CommandIDs::ToggleMuteClips, muteAction)->closesMenu());

    menu.add(MenuItem::item(Icons::unmute,
        CommandIDs::ToggleSoloClips, soloAction)->
        disabledIf(!canSolo)->
        closesMenu());

    const auto canDuplicate = this->lasso->getNumSelected() == 1;

    menu.add(MenuItem::item(Icons::copy, CommandIDs::DuplicateTrack,
        TRANS(I18n::Menu::trackDuplicate))->disabledIf(!canDuplicate)->closesMenu());

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteClips,
        TRANS(I18n::Menu::delete_))->closesMenu());

    menu.add(MenuItem::item(Icons::reprise,
        CommandIDs::ToggleLoopOverSelection,
        TRANS(I18n::CommandPalette::toggleLoopOverSelection))->closesMenu());

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesQuantizeTo))->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->createQuantizationMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::list, TRANS(I18n::Menu::trackChangeChannel))->
        withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createChannelSelectionMenu(), MenuPanel::SlideLeft);
    }));

    const auto instruments = App::Workspace().getAudioCore().getInstrumentsExceptInternal();
    menu.add(MenuItem::item(Icons::instrument, TRANS(I18n::Menu::trackChangeInstrument))->
        disabledIf(instruments.isEmpty())->withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createInstrumentSelectionMenu(), MenuPanel::SlideLeft);
    }));

    const auto selectionInstrumentId = PatternOperations::getSelectedInstrumentId(*this->lasso.get());
    for (const auto *i : instruments)
    {
        if (i->getIdAndHash() == selectionInstrumentId)
        {
            const auto editInstrumentCaption = i->getName() + ": " + TRANS(I18n::Menu::instrumentShowWindow);
            menu.add(MenuItem::item(Icons::instrument, CommandIDs::EditCurrentInstrument,
                editInstrumentCaption)->disabledIf(selectionInstrumentId.isEmpty())->closesMenu());
            break;
        }
    }

    return menu;
}

MenuPanel::Menu PatternRollSelectionMenu::createQuantizationMenu()
{
    MenuPanel::Menu menu;

    using namespace I18n::Menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
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

MenuPanel::Menu PatternRollSelectionMenu::createChannelSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    Array<MidiTrack *> uniqueTracks;
    FlatHashSet<int> uniqueChannels;
    for (int i = 0; i < this->lasso->getNumSelected(); ++i)
    {
        const auto &clip = this->lasso->getItemAs<ClipComponent>(i)->getClip();
        uniqueTracks.addIfNotAlreadyThere(clip.getPattern()->getTrack());
        uniqueChannels.insert(clip.getPattern()->getTrack()->getTrackChannel());
    }

    for (int channel = 1; channel <= Globals::numChannels; ++channel)
    {
        const bool isTicked = uniqueChannels.size() == 1 && uniqueChannels.contains(channel);
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::ellipsis, String(channel))->
            disabledIf(isTicked)->
            withAction([this, channel, uniqueTracks]()
        {
            bool haveCheckpoint = false;
            for (auto *track : uniqueTracks)
            {
                if (channel != track->getTrackChannel())
                {
                    auto *trackNode = dynamic_cast<MidiTrackNode *>(track);
                    jassert(trackNode != nullptr);

                    auto *project = trackNode->getProject();

                    if (!haveCheckpoint)
                    {
                        project->checkpoint();
                        haveCheckpoint = true;
                    }

                    track->setTrackChannel(channel, true, sendNotification);
                }
            }

            this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
        }));
    }

    return menu;
}

MenuPanel::Menu PatternRollSelectionMenu::createInstrumentSelectionMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    // get all unique track nodes in the selection;
    // if they all have the same instrument assigned, it should be selected;
    // perform one undo action for them all

    const auto &audioCore = App::Workspace().getAudioCore();
    const auto instruments = audioCore.getInstrumentsExceptInternal();

    Array<MidiTrack *> uniqueTracks;
    StringArray uniqueInstrumentIds;
    for (int i = 0; i < this->lasso->getNumSelected(); ++i)
    {
        const auto &clip = this->lasso->getItemAs<ClipComponent>(i)->getClip();
        const auto instrumentId = clip.getPattern()->getTrack()->getTrackInstrumentId();
        uniqueInstrumentIds.addIfNotAlreadyThere(instrumentId);
        uniqueTracks.addIfNotAlreadyThere(clip.getPattern()->getTrack());
    }

    const Instrument *singleInstrumentIfAny =
        (uniqueInstrumentIds.size() == 1) ?
        audioCore.findInstrumentById(uniqueInstrumentIds.getReference(0)) :
        nullptr;

    for (const auto *instrument : instruments)
    {
        const bool isTicked = (instrument == singleInstrumentIfAny);
        const auto instrumentId = instrument->getIdAndHash();
        menu.add(MenuItem::item(isTicked ? Icons::apply : Icons::instrument,
            instrument->getName())->disabledIf(isTicked)->
            withAction([this, instrumentId, uniqueTracks]()
        {
            //DBG(instrumentId);
            bool haveCheckpoint = false;
            for (auto *track : uniqueTracks)
            {
                if (instrumentId != track->getTrackInstrumentId())
                {
                    auto *trackNode = dynamic_cast<MidiTrackNode *>(track);
                    jassert(trackNode != nullptr);

                    auto *project = trackNode->getProject();

                    if (!haveCheckpoint)
                    {
                        project->checkpoint();
                        haveCheckpoint = true;
                    }

                    track->setTrackInstrumentId(instrumentId, true, sendNotification);
                }
            }

            this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
        }));
    }

    return menu;
}

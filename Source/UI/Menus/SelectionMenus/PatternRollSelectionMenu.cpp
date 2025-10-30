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

#include "ArpeggiationSequenceModifier.h"
#include "RefactoringSequenceModifier.h"
#include "TuningSequenceModifier.h"

#include "TempoDialog.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "CommandIDs.h"

PatternRollSelectionMenu::PatternRollSelectionMenu(WeakReference<Lasso> lasso) :
    ClipModifiersMenu(lasso->getFirstAs<ClipComponent>()->getClip(),
        lasso->getFirstAs<ClipComponent>()->getClip().getPattern()->getUndoStack()),
    lasso(lasso)
{
    jassert(lasso->getNumSelected() > 0);
    this->updateContent(this->makeDefaultMenu(), MenuPanel::Fading);
}

static bool canRenamePatternSelection(WeakReference<Lasso> lasso) noexcept
{
    if (lasso->getNumSelected() == 0)
    {
        jassertfalse;
        return false;
    }

    const auto firstTrackId = lasso->getFirstAs<ClipComponent>()->getClip().getTrackId();
    for (int i = 0; i < lasso->getNumSelected(); ++i)
    {
        if (lasso->getItemAs<ClipComponent>(i)->getClip().getTrackId() != firstTrackId)
        {
            return false; // selection has multiple tracks
        }
    }

    return true;
}

static bool canTriggerSoloForPatternSelection(WeakReference<Lasso> lasso) noexcept
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

static bool canMakeUnique(WeakReference<Lasso> lasso) noexcept
{
    if (lasso->getNumSelected() != 1)
    {
        return false;
    }

    return lasso->getFirstAs<ClipComponent>()->getClip().getPattern()->size() > 1;
}

MenuPanel::Menu PatternRollSelectionMenu::makeDefaultMenu() noexcept
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
        const auto &clip = this->lasso->getFirstAs<ClipComponent>()->getClip();
        const auto *track = clip.getPattern()->getTrack();

        const auto tsActionlabel = track->hasTimeSignatureOverride() ?
            TRANS(I18n::Menu::timeSignatureChange) :
            TRANS(I18n::Menu::timeSignatureAdd);

        menu.add(MenuItem::item(Icons::meter,
            CommandIDs::SetTrackTimeSignature, tsActionlabel)->closesMenu());

        if (track->isTempoTrack())
        {
            menu.add(MenuItem::item(Icons::automationTrack,
                CommandIDs::TrackSetOneTempo, TRANS(I18n::Menu::setOneTempo))->closesMenu());

            menu.add(MenuItem::item(Icons::automationTrack,
                CommandIDs::TempoUp1Bpm, TRANS(I18n::Menu::tempoUp1Bpm))->closesMenu());

            menu.add(MenuItem::item(Icons::automationTrack,
                CommandIDs::TempoDown1Bpm, TRANS(I18n::Menu::tempoDown1Bpm))->closesMenu());
        }
    }

#if PLATFORM_DESKTOP
    if (this->lasso->getNumSelected() > 1)
#elif PLATFORM_MOBILE
    if (this->lasso->getNumSelected() > 0)
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
                TRANS(I18n::Menu::Selection::transposeUp)));

            menu.add(MenuItem::item(Icons::down, CommandIDs::ClipTransposeDown,
                TRANS(I18n::Menu::Selection::transposeDown)));
        }
    }

    const auto muteAction = PatternOperations::lassoContainsMutedClip(*this->lasso.get()) ?
        TRANS(I18n::Menu::unmute) : TRANS(I18n::Menu::mute);

    const auto canSolo = canTriggerSoloForPatternSelection(this->lasso);
    const auto soloAction = PatternOperations::lassoContainsSoloedClip(*this->lasso.get()) ?
        TRANS(I18n::Menu::unsolo) : TRANS(I18n::Menu::solo);

    menu.add(MenuItem::item(Icons::mute,
        CommandIDs::ToggleMuteClips, muteAction)->closesMenu());

    menu.add(MenuItem::item(Icons::volumeUp,
        CommandIDs::ToggleSoloClips, soloAction)->
        disabledIf(!canSolo)->
        closesMenu());

    if (this->lasso->getNumSelected() == 1)
    {
        menu.add(MenuItem::item(Icons::copy, CommandIDs::DuplicateTrack,
            TRANS(I18n::Menu::trackDuplicate))->closesMenu());
    }

    if (canMakeUnique(this->lasso))
    {
        menu.add(MenuItem::item(Icons::paste, CommandIDs::InstanceToUniqueTrack,
            TRANS(I18n::Menu::trackMakeUnique))->closesMenu());
    }

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteClips,
        TRANS(I18n::Menu::delete_))->closesMenu());

    menu.add(MenuItem::item(Icons::reprise,
        CommandIDs::ToggleLoopOverSelection,
        TRANS(I18n::CommandPalette::toggleLoopOverSelection))->closesMenu());

    if (this->lasso->getNumSelected() == 1)
    {
        const auto &clip = this->lasso->getFirstAs<ClipComponent>()->getClip();
        menu.add(MenuItem::item(Icons::arpeggiate,
            clip.hasModifiers() ? TRANS(I18n::Menu::Modifiers::edit) : TRANS(I18n::Menu::Modifiers::add))->
            withSubmenu()->withAction([this]()
            {
                this->updateContent(this->makeModifiersMenu([this]()
                {
                    this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
                }), MenuPanel::SlideLeft);
            }));
    }

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

    const auto selectionInstrumentId = PatternOperations::getSelectedInstrumentId(*this->lasso.get());
    for (const auto *instrument : instruments)
    {
        if (instrument->getIdAndHash() == selectionInstrumentId)
        {
            if (auto mainNode = instrument->findFirstMidiReceiver())
            {
                // not checking mainNode->getProcessor()->hasEditor() because it may hang for a few seconds
                const auto editInstrumentCaption = instrument->getName() + ": " + TRANS(I18n::Menu::instrumentShowWindow);
                menu.add(MenuItem::item(Icons::instrument, CommandIDs::EditCurrentInstrument, editInstrumentCaption)->
                    disabledIf(!instrument->isValid())->closesMenu());
            }

            break;
        }
    }

    return menu;
}

MenuPanel::Menu PatternRollSelectionMenu::makeQuantizationMenu() noexcept
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

MenuPanel::Menu PatternRollSelectionMenu::makeChannelSelectionMenu() noexcept
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
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
        const bool isCurrent = uniqueChannels.size() == 1 && uniqueChannels.contains(channel);
        menu.add(MenuItem::item(Icons::list, String(channel))->
            disabledIf(isCurrent)->
            markedAsCurrentIf(isCurrent)->
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

                this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
            }));
    }

    return menu;
}

MenuPanel::Menu PatternRollSelectionMenu::makeInstrumentSelectionMenu() noexcept
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));

    // get all unique track nodes in the selection;
    // if they all have the same instrument assigned, it should be selected;
    // perform one undo action for them all

    const auto &audioCore = App::Workspace().getAudioCore();
    const auto instruments = audioCore.getInstrumentsExceptInternal();

    Array<WeakReference<MidiTrack>> uniqueTracks;
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
        const bool isCurrent = (instrument == singleInstrumentIfAny);
        const auto instrumentId = instrument->getIdAndHash();
        menu.add(MenuItem::item(Icons::instrument, instrument->getName())->
            disabledIf(isCurrent)->
            markedAsCurrentIf(isCurrent)->
            withAction([this, instrumentId, uniqueTracks]()
            {
                //DBG(instrumentId);
                bool haveCheckpoint = false;
                for (auto &track : uniqueTracks)
                {
                    auto *trackNode = dynamic_cast<MidiTrackNode *>(track.get());
                    if (trackNode == nullptr)
                    {
                        jassertfalse;
                        continue;
                    }

                    if (instrumentId != track->getTrackInstrumentId())
                    {
                        auto *project = trackNode->getProject();

                        if (!haveCheckpoint)
                        {
                            project->checkpoint();
                            haveCheckpoint = true;
                        }

                        track->setTrackInstrumentId(instrumentId, true, sendNotification);
                    }
                }

                this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
            }));
    }

    return menu;
}

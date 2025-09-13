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
#include "Pattern.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "MidiSequence.h"
#include "RollBase.h"
#include "PatternOperations.h"
#include "ArpeggiationSequenceModifier.h"
#include "Workspace.h"

ClipMenu::ClipMenu(const Clip &clip, WeakReference<UndoStack> undoStack) :
    ClipModifiersMenu(clip, undoStack),
    clip(clip),
    undoStack(undoStack)
{
    jassert(clip.isValid());
    this->updateContent(this->makeDefaultMenu(), MenuPanel::Fading);
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
    menu.add(MenuItem::item(Icons::volumeUp,
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

    menu.add(MenuItem::item(Icons::arpeggiate,
        this->clip.hasModifiers() ? TRANS(I18n::Menu::Modifiers::edit) : TRANS(I18n::Menu::Modifiers::add))->
        withSubmenu()->withAction([this]()
        {
            this->updateContent(this->makeModifiersMenu([this]()
                {
                    this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
                }), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::refactor,
        TRANS(I18n::Menu::Selection::notesRefactor))->
        withSubmenu()->
        withAction([this]()
        {
            this->updateContent(this->makeRefactoringMenu(), MenuPanel::SlideLeft);
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
            if (auto mainNode = instrument->findFirstMidiReceiver())
            {
                // not checking mainNode->getProcessor()->hasEditor() because it may hang for a few seconds
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

// tech debt warning: mostly duplicates PianoRollSelectionMenu::makeRefactoringMenu
MenuPanel::Menu ClipMenu::makeRefactoringMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));
    
    menu.add(MenuItem::item(Icons::inversion, CommandIDs::MelodicInversion,
        TRANS(I18n::Menu::Refactor::melodicInversion))->closesMenu());

    menu.add(MenuItem::item(Icons::retrograde, CommandIDs::Retrograde,
        TRANS(I18n::Menu::Refactor::retrograde))->closesMenu());

    menu.add(MenuItem::item(Icons::cleanup, CommandIDs::CleanupOverlaps,
        TRANS(I18n::Menu::Refactor::cleanup))->closesMenu());

    menu.add(MenuItem::item(Icons::inverseUp, CommandIDs::InvertChordUp,
        TRANS(I18n::Menu::Refactor::inverseUp))->closesMenu());

    menu.add(MenuItem::item(Icons::inverseDown, CommandIDs::InvertChordDown,
        TRANS(I18n::Menu::Refactor::inverseDown))->closesMenu());

    menu.add(MenuItem::item(Icons::up, CommandIDs::TransposeScaleKeyUp,
        TRANS(I18n::Menu::Refactor::inScaleTransposeUp))->closesMenu());

    menu.add(MenuItem::item(Icons::down, CommandIDs::TransposeScaleKeyDown,
        TRANS(I18n::Menu::Refactor::inScaleTransposeDown))->closesMenu());

    menu.add(MenuItem::item(Icons::snap, CommandIDs::AlignToScale,
        TRANS(I18n::Menu::Refactor::alignToScale))->closesMenu());

    menu.add(MenuItem::item(Icons::legato, CommandIDs::MakeLegato,
        TRANS(I18n::Menu::Refactor::legato))->closesMenu());

    menu.add(MenuItem::item(Icons::staccato, CommandIDs::MakeStaccato,
        TRANS(I18n::Menu::Refactor::staccato))->closesMenu());

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesQuantizeTo))->
        withSubmenu()->
        withAction([this]()
        {
            this->updateContent(this->makeQuantizationMenu(), MenuPanel::SlideLeft);
        }));

    return menu;
}

MenuPanel::Menu ClipMenu::makeQuantizationMenu()
{
    MenuPanel::Menu menu;

    using namespace I18n::Menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeRefactoringMenu(), MenuPanel::SlideRight);
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
        const bool isCurrent = track->getTrackChannel() == channel;
        menu.add(MenuItem::item(Icons::list, String(channel))->
            disabledIf(isCurrent)->
            markedAsCurrentIf(isCurrent)->
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
        const bool isCurrent = instrument == selectedInstrument;
        const String instrumentId = instrument->getIdAndHash();
        menu.add(MenuItem::item(Icons::instrument, instrument->getName())->
            disabledIf(!instrument->isValid() || isCurrent)->
            markedAsCurrentIf(isCurrent)->
            withAction([this, track, instrumentId]()
            {
                this->undoStack->beginNewTransaction();
                track->setTrackInstrumentId(instrumentId, true, sendNotification);
                this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
            }));
    }
    
    return menu;
}


//===----------------------------------------------------------------------===//
// Parametric modifiers menus
//===----------------------------------------------------------------------===//

ClipModifiersMenu::ClipModifiersMenu(const Clip &clip, WeakReference<UndoStack> undoStack) :
    clip(clip),
    undoStack(undoStack) {}

MenuPanel::Menu ClipModifiersMenu::makeModifiersMenu(const MenuItem::Callback &goBackToParent)
{
    if (this->clip.hasModifiers())
    {
        return this->makeEditModifiersMenu(goBackToParent);
    }

    return this->makeAddModifiersMenu(goBackToParent);
}

MenuPanel::Menu ClipModifiersMenu::makeEditModifiersMenu(const MenuItem::Callback &goBackToParent)
{
    MenuPanel::Menu menu;
    if (goBackToParent)
    {
        menu.add(MenuItem::item(Icons::back,
            TRANS(I18n::Menu::back))->withAction(goBackToParent));
    }

    const auto goBackToMe = [this, goBackToParent]()
    {
        this->updateContent(this->makeEditModifiersMenu(goBackToParent), MenuPanel::SlideRight);
    };

    for (const auto &modifier : this->clip.getModifiers())
    {
        menu.add(MenuItem::item(modifier->getIconId(), modifier->getDescription())->
            withButton(true, modifier->isEnabled() ? Icons::toggleOn : Icons::toggleOff, [this, goBackToParent, modifier]()
            {
                this->undoStack->beginNewTransaction();
                this->clip.getPattern()->change(this->clip,
                    this->clip.withUpdatedModifier(modifier,
                        modifier->withEnabledFlag(!modifier->isEnabled())), true);
                this->updateContent(this->makeEditModifiersMenu(goBackToParent), MenuPanel::None);
            })->
            withButton(modifier != this->clip.getModifiers().getFirst(), Icons::up, [this, goBackToParent, modifier]()
            {
                this->undoStack->beginNewTransaction();
                this->clip.getPattern()->change(this->clip,
                    this->clip.withShiftedModifier(modifier, -1), true);
                this->updateContent(this->makeEditModifiersMenu(goBackToParent), MenuPanel::None);
            })->
            withButton(modifier != this->clip.getModifiers().getLast(), Icons::down, [this, goBackToParent, modifier]()
            {
                this->undoStack->beginNewTransaction();
                this->clip.getPattern()->change(this->clip,
                    this->clip.withShiftedModifier(modifier, 1), true);
                this->updateContent(this->makeEditModifiersMenu(goBackToParent), MenuPanel::None);
            })->
            withButton(true, Icons::close, [this, goBackToParent, modifier]()
            {
                this->undoStack->beginNewTransaction();
                this->clip.getPattern()->change(this->clip,
                    this->clip.withRemovedModifier(modifier), true);
                this->updateContent(this->makeEditModifiersMenu(goBackToParent), MenuPanel::None);
            })->
            disabledIf(!modifier->isEnabled())->
            withSubmenuIf(modifier->hasParameters())->
            withActionIf(modifier->hasParameters(), [this, goBackToMe, modifier]()
            {
                if (dynamic_cast<ArpeggiationSequenceModifier *>(modifier.get()))
                {
                    this->updateContent(this->makeModifiersArpsMenu(goBackToMe, goBackToMe, modifier), MenuPanel::SlideLeft);
                }
                else if (auto *refactoringMod = dynamic_cast<RefactoringSequenceModifier *>(modifier.get()))
                {
                    this->updateContent(this->makeModifiersRefactoringStepsMenu(goBackToMe, goBackToMe,
                        modifier, refactoringMod->getType(), modifier->getIconId(),
                        RefactoringSequenceModifier::getParametersChoice(refactoringMod->getType())),
                        MenuPanel::SlideLeft);
                }
#if DEBUG
                else
                {
                    jassertfalse;
                }
#endif
            }));
    }

    menu.add(MenuItem::item(Icons::create, TRANS(I18n::Menu::Modifiers::add))->
        withSubmenu()->withAction([this, goBackToMe]()
        {
            this->updateContent(this->makeAddModifiersMenu(goBackToMe), MenuPanel::SlideLeft);
        }));

    const auto shouldMute = clip.hasEnabledModifiers();
    menu.add(MenuItem::item(shouldMute ? Icons::mute : Icons::volumeUp,
        shouldMute ? TRANS(I18n::Menu::Modifiers::disableAll) : TRANS(I18n::Menu::Modifiers::enableAll))->
        withHotkeyText(CommandIDs::ToggleMuteModifiers)->
        disabledIf(!this->clip.hasModifiers())->
        withAction([this, goBackToParent]()
        {
            PatternOperations::toggleMuteModifiersStack(clip, true);
            this->updateContent(this->makeEditModifiersMenu(goBackToParent), MenuPanel::None);
        }));
    
    menu.add(MenuItem::item(Icons::apply, TRANS(I18n::Menu::Modifiers::applyAll))->
        disabledIf(!this->clip.hasEnabledModifiers())->
        closesMenu()->
        withAction([this, goBackToParent]()
        {
            PatternOperations::applyModifiersStack(clip, true);
        }));

    menu.add(MenuItem::item(Icons::close, TRANS(I18n::Menu::Modifiers::deleteAll))->
        disabledIf(!this->clip.hasModifiers())->
        closesMenu()->
        withAction([goBackToParent, this]()
        {
            this->undoStack->beginNewTransaction();
            this->clip.getPattern()->change(this->clip, this->clip.withModifiers({}), true);
        }));

    return menu;
}

MenuPanel::Menu ClipModifiersMenu::makeAddModifiersMenu(const MenuItem::Callback &goBackToParent)
{
    MenuPanel::Menu menu;
    if (goBackToParent)
    {
        menu.add(MenuItem::item(Icons::back,
            TRANS(I18n::Menu::back))->withAction(goBackToParent));
    }

    const auto goBackToMe = [this, goBackToParent]()
    {
        this->updateContent(this->makeAddModifiersMenu(goBackToParent), MenuPanel::SlideRight);
    };

    menu.add(MenuItem::item(Icons::arpeggiate, TRANS(I18n::Menu::Selection::notesArpeggiate))->
        disabledIf(App::Config().getArpeggiators()->isEmpty())->
        withSubmenu()->withAction([this, goBackToParent, goBackToMe]()
        {
            this->updateContent(this->makeModifiersArpsMenu(goBackToMe, goBackToParent, {}), MenuPanel::SlideLeft);
        }));

    for (const auto type : RefactoringSequenceModifier::allTypes)
    {
        const auto iconId = RefactoringSequenceModifier::getIconId(type);
        const auto hasParameters = RefactoringSequenceModifier::hasParameters(type);
        menu.add(MenuItem::item(iconId, RefactoringSequenceModifier::getDescription(type))->
            withSubmenuIf(hasParameters)->
            withAction([this, goBackToParent, goBackToMe, hasParameters, type, iconId]()
            {
                if (hasParameters)
                {
                    this->updateContent(this->makeModifiersRefactoringStepsMenu(goBackToMe, goBackToParent, {},
                        type, iconId, RefactoringSequenceModifier::getParametersChoice(type)),
                        MenuPanel::SlideLeft);
                }
                else
                {
                    this->undoStack->beginNewTransaction();
                    SequenceModifier::Ptr mod(new RefactoringSequenceModifier(type));
                    this->clip.getPattern()->change(this->clip, this->clip.withAppendedModifier(mod), true);
                    if (goBackToParent != nullptr)
                    {
                        goBackToParent();
                    }
                }
            }));
    }

    return menu;
}

MenuPanel::Menu ClipModifiersMenu::makeModifiersArpsMenu(const MenuItem::Callback &goBackToParent,
    const MenuItem::Callback &onAdd, SequenceModifier::Ptr updatedModifier)
{
    MenuPanel::Menu menu;
    if (goBackToParent)
    {
        menu.add(MenuItem::item(Icons::back,
            TRANS(I18n::Menu::back))->withAction(goBackToParent));
    }

    const auto goBackToMe = [this, goBackToParent, onAdd, updatedModifier]()
    {
        this->updateContent(this->makeModifiersArpsMenu(
            goBackToParent, onAdd, updatedModifier), MenuPanel::SlideRight);
    };

    const auto arps = App::Config().getArpeggiators()->getAll();
    for (int i = 0; i < arps.size(); ++i)
    {
        Arpeggiator::Ptr arp = arps[i];
        menu.add(MenuItem::item(Icons::arpeggiate, arp->getName())->
            withSubmenu()->withAction([this, goBackToMe, onAdd, updatedModifier, arp]()
            {
                this->updateContent(this->makeModifiersArpsSpeedMenu(goBackToMe, onAdd,
                    updatedModifier, arp, { 0.25, 0.5f, 1.f, 2.f, 4.f }), MenuPanel::SlideLeft);
            }));
    }

    return menu;
}

MenuPanel::Menu ClipModifiersMenu::makeModifiersArpsSpeedMenu(
    const MenuItem::Callback &goBackToParent, const MenuItem::Callback &onAdd,
    SequenceModifier::Ptr updatedModifier,
    const Arpeggiator::Ptr arp, const Array<float> &speedValues)
{
    MenuPanel::Menu menu;
    if (goBackToParent)
    {
        menu.add(MenuItem::item(Icons::back,
            TRANS(I18n::Menu::back))->withAction(goBackToParent));
    }

    for (const auto &speed : speedValues)
    {
        menu.add(MenuItem::item(Icons::arpeggiate, "x" + String(speed))->
            withAction([this, goBackToParent, onAdd, updatedModifier, arp, speed]()
            {
                this->undoStack->beginNewTransaction();

                SequenceModifier::Ptr mod(new ArpeggiationSequenceModifier(arp, speed));

                if (updatedModifier == nullptr)
                {
                    this->clip.getPattern()->change(this->clip,
                        this->clip.withAppendedModifier(mod), true);
                }
                else
                {
                    this->clip.getPattern()->change(this->clip,
                        this->clip.withUpdatedModifier(updatedModifier, mod), true);
                }

                if (onAdd != nullptr)
                {
                    onAdd();
                }
            }));
    }

    return menu;
}

MenuPanel::Menu ClipModifiersMenu::makeModifiersRefactoringStepsMenu(
    const MenuItem::Callback &goBackToParent, const MenuItem::Callback &onAdd,
    SequenceModifier::Ptr updatedModifier, RefactoringSequenceModifier::Type type,
    Icons::Id iconId, const Array<RefactoringSequenceModifier::Parameter> &parameters)
{
    MenuPanel::Menu menu;
    if (goBackToParent)
    {
        menu.add(MenuItem::item(Icons::back,
            TRANS(I18n::Menu::back))->withAction(goBackToParent));
    }

    for (const auto &parameter : parameters)
    {
        menu.add(MenuItem::item(iconId, parameter.name)->
            withAction([this, goBackToParent, onAdd, updatedModifier, type, parameter]()
            {
                this->undoStack->beginNewTransaction();

                SequenceModifier::Ptr mod(new RefactoringSequenceModifier(type, parameter.value));

                if (updatedModifier == nullptr)
                {
                    this->clip.getPattern()->change(this->clip,
                        this->clip.withAppendedModifier(mod), true);
                }
                else
                {
                    this->clip.getPattern()->change(this->clip,
                        this->clip.withUpdatedModifier(updatedModifier, mod), true);
                }

                if (onAdd != nullptr)
                {
                    onAdd();
                }
            }));
    }

    return menu;
}

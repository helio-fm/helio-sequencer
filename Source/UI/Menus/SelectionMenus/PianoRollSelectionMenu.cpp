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
#include "PianoRollSelectionMenu.h"

#include "Lasso.h"
#include "SequencerOperations.h"
#include "NoteComponent.h"
#include "PianoRoll.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "PianoTrackNode.h"
#include "ProjectTimeline.h"

#include "CommandIDs.h"
#include "MainLayout.h"
#include "Config.h"

MenuPanel::Menu PianoRollSelectionMenu::makeDefaultMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents,
        TRANS(I18n::Menu::copy))->closesMenu());

    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS(I18n::Menu::cut))->closesMenu());

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteEvents,
        TRANS(I18n::Menu::delete_))->closesMenu());

    menu.add(MenuItem::item(Icons::reprise,
        CommandIDs::ToggleLoopOverSelection,
        TRANS(I18n::CommandPalette::toggleLoopOverSelection))->closesMenu());
    
    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesDivisions))->
        withSubmenu()->
        withAction([this]()
        {
            this->updateContent(this->makeTupletsMenu(), MenuPanel::SlideLeft);
        }));

    jassert(this->lasso->getNumSelected() > 0);
    const bool hasSingleNote = this->lasso->getNumSelected() == 1;

    menu.add(MenuItem::item(Icons::arpeggiate,
        TRANS(I18n::Menu::Selection::notesArpeggiate))->
        disabledIf(hasSingleNote)->
        withSubmenu()->
        withAction([this]()
        {
            this->updateContent(this->makeArpsMenu(), MenuPanel::SlideLeft);
        }));
    
    menu.add(MenuItem::item(Icons::refactor,
        TRANS(I18n::Menu::Selection::notesRefactor))->
        withSubmenu()->
        withAction([this]()
        {
            this->updateContent(this->makeRefactoringMenu(), MenuPanel::SlideLeft);
        }));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::makeMoveToTrackMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeRefactoringMenu(), MenuPanel::SlideRight);
    }));

    if (this->lasso->getNumSelected() == 0)
    {
        jassertfalse;
        return menu;
    }

    jassert(this->lasso->getNumSelected() > 0);

    const auto *sourceTrack = this->lasso->getFirstAs<NoteComponent>()->getNote().getSequence()->getTrack();

    for (auto *targetTrack : this->getProject().findChildrenOfType<PianoTrackNode>())
    {
        if (targetTrack == sourceTrack)
        {
            continue;
        }

        float outDistance = 0.f;
        auto &closestClip = SequencerOperations::findClosestClip(*this->lasso.get(), targetTrack, outDistance);

        menu.add(MenuItem::item(Icons::pianoTrack, targetTrack->getTrackName())->
            withColour(targetTrack->getTrackColour())->
            withWeight(outDistance)->
            closesMenu()->
            withAction([this, &closestClip]()
            {
                auto &selection = *this->lasso.get();
                auto &project = this->getProject();
                auto &roll = this->roll; // save for later when "this" is not longer a valid object
                const auto newNotes = SequencerOperations::moveSelection(selection, closestClip, true);
                project.setEditableScope(closestClip, false); // likely dismisses the menu
                roll.selectEvents(newNotes, true);
            }));
    }

    static MenuItem::SortByWeight sortByWeight;
    menu.sort(sortByWeight);
    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::makeRefactoringMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));
    
    // in some cases transforming a single note makes no sense
    jassert(this->lasso->getNumSelected() > 0);
    const bool hasSingleNote = this->lasso->getNumSelected() == 1;

    menu.add(MenuItem::item(Icons::inversion, CommandIDs::MelodicInversion,
        TRANS(I18n::Menu::Refactor::melodicInversion))->disabledIf(hasSingleNote)->closesMenu());

    menu.add(MenuItem::item(Icons::retrograde, CommandIDs::Retrograde,
        TRANS(I18n::Menu::Refactor::retrograde))->disabledIf(hasSingleNote)->closesMenu());

    menu.add(MenuItem::item(Icons::cleanup, CommandIDs::CleanupOverlaps,
        TRANS(I18n::Menu::Refactor::cleanup))->disabledIf(hasSingleNote)->closesMenu());

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
        TRANS(I18n::Menu::Refactor::legato))->disabledIf(hasSingleNote)->closesMenu());

    menu.add(MenuItem::item(Icons::staccato, CommandIDs::MakeStaccato,
        TRANS(I18n::Menu::Refactor::staccato))->closesMenu());

    menu.add(MenuItem::item(Icons::cut, CommandIDs::NewTrackFromSelection,
        TRANS(I18n::Menu::Selection::notesToTrack))->closesMenu());

    const bool nowhereToMove = this->getProject().findChildrenOfType<PianoTrackNode>().size() < 2;

    menu.add(MenuItem::item(Icons::cut,
        TRANS(I18n::Menu::Selection::notesMoveTo))->
        disabledIf(nowhereToMove)->withSubmenu()->withAction([this]()
        {
            this->updateContent(this->makeMoveToTrackMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::arpeggiate,
        TRANS(I18n::Menu::Selection::notesRescale))->
        disabledIf(hasSingleNote)->
        withSubmenu()->
        withAction([this]()
        {
            this->updateContent(this->makeRescalingMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesQuantizeTo))->
        withSubmenu()->
        withAction([this]()
        {
            this->updateContent(this->makeQuantizationMenu(), MenuPanel::SlideLeft);
        }));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::makeRescalingMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeRefactoringMenu(), MenuPanel::SlideRight);
    }));

    const auto &scales = App::Config().getScales()->getAll();
    for (int i = 0; i < scales.size(); ++i)
    {
        if (scales.getUnchecked(i)->getBasePeriod() !=
            this->getProject().getProjectInfo()->getTemperament()->getPeriodSize())
        {
            continue;
        }

        menu.add(MenuItem::item(Icons::arpeggiate,
            scales.getUnchecked(i)->getLocalizedName())->
            closesMenu()->
            withAction([this, i]()
            {
                if (this->lasso->getNumSelected() < 1)
                {
                    jassertfalse;
                    return;
                }

                const auto &scales = App::Config().getScales()->getAll();
                const auto &clip = this->lasso->getFirstAs<NoteComponent>()->getClip();

                SequencerOperations::rescale(*this->lasso.get(), clip,
                    this->getProject().getTimeline()->getKeySignaturesSequence(), scales[i],
                    true, true);
            }));
    }

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::makeQuantizationMenu()
{
    MenuPanel::Menu menu;

    using namespace I18n::Menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeRefactoringMenu(), MenuPanel::SlideRight);
    }));

#define EVENTS_QUANTIZE_ITEM(cmd) \
    MenuItem::item(Icons::ellipsis, cmd, \
        TRANS(CommandIDs::getTranslationKeyFor(cmd)))->closesMenu()
    
    menu.add(EVENTS_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_1));
    menu.add(EVENTS_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_2));
    menu.add(EVENTS_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_4));
    menu.add(EVENTS_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_8));
    menu.add(EVENTS_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_16));
    menu.add(EVENTS_QUANTIZE_ITEM(CommandIDs::QuantizeTo1_32));

#undef EVENTS_QUANTIZE_ITEM

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::makeTupletsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet1, TRANS(I18n::Menu::tuplet1))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet2, TRANS(I18n::Menu::tuplet2))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet3, TRANS(I18n::Menu::tuplet3))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet4, TRANS(I18n::Menu::tuplet4))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet5, TRANS(I18n::Menu::tuplet5))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet6, TRANS(I18n::Menu::tuplet6))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet7, TRANS(I18n::Menu::tuplet7))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet8, TRANS(I18n::Menu::tuplet8))->closesMenu());
    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::Tuplet9, TRANS(I18n::Menu::tuplet9))->closesMenu());

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::makeArpsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::create,
        CommandIDs::CreateArpeggiatorFromSelection,
        TRANS(I18n::Menu::arpeggiatorsCreate))->closesMenu());

    const auto arps = App::Config().getArpeggiators()->getAll();
    for (int i = 0; i < arps.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::arpeggiate, arps.getUnchecked(i)->getName())->
            closesMenu()->withAction([this, i]()
            {
                if (this->lasso->getNumSelected() < 3)
                {
                    jassertfalse;
                    return;
                }

                const auto arps = App::Config().getArpeggiators()->getAll();
                const auto &clip = this->lasso->getFirstAs<NoteComponent>()->getClip();

                SequencerOperations::arpeggiate(*this->lasso.get(), clip,
                    arps[i],
                    this->getProject().getProjectInfo()->getTemperament(),
                    this->getProject().getTimeline()->getKeySignaturesSequence(),
                    this->getProject().getTimeline()->getTimeSignaturesAggregator(),
                    1.0f, 0.0f, false, false,
                    true, true);
            }));
    }

    return menu;
}

PianoRollSelectionMenu::PianoRollSelectionMenu(PianoRoll &roll, WeakReference<Lasso> lasso) :
    roll(roll),
    lasso(lasso)
{
    this->updateContent(this->makeDefaultMenu(), MenuPanel::SlideRight);
}

ProjectNode &PianoRollSelectionMenu::getProject()
{
    return this->roll.getProject();
}

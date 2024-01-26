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

#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "PianoTrackNode.h"
#include "ProjectTimeline.h"

#include "CommandIDs.h"
#include "MainLayout.h"
#include "Config.h"

MenuPanel::Menu PianoRollSelectionMenu::createDefaultPanel()
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

    menu.add(MenuItem::item(Icons::refactor,
        TRANS(I18n::Menu::Selection::notesRefactor))->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->createRefactoringPanel(), MenuPanel::SlideLeft);
    }));

    const bool canArpeggiate = (this->lasso->getNumSelected() > 1) && (this->harmonicContextScale != nullptr);

    menu.add(MenuItem::item(Icons::arpeggiate,
        TRANS(I18n::Menu::Selection::notesArpeggiate))->
        disabledIf(!canArpeggiate)->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->createArpsPanel(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesDivisions))->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->createTupletsPanel(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesQuantizeTo))->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->createQuantizationPanel(), MenuPanel::SlideLeft);
    }));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createMoveToTrackPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createRefactoringPanel(), MenuPanel::SlideRight);
    }));


    if (this->lasso->getNumSelected() == 0)
    {
        jassertfalse;
        return menu;
    }

    jassert(this->lasso->getNumSelected() > 0);

    const auto *sourceTrack = this->lasso->getFirstAs<NoteComponent>()->getNote().getSequence()->getTrack();

    for (auto *targetTrack : this->project.findChildrenOfType<PianoTrackNode>())
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
            SequencerOperations::moveSelection(selection, closestClip, true);
            this->project.setEditableScope(closestClip, false);
        }));
    }

    static MenuItem::SortByWeight sortByWeight;
    menu.sort(sortByWeight);
    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createRefactoringPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));
    
    const bool canRefactor = this->lasso->getNumSelected() > 1;

    menu.add(MenuItem::item(Icons::inverseUp, CommandIDs::InvertChordUp,
        TRANS(I18n::Menu::refactoringInverseUp))->disabledIf(!canRefactor)->closesMenu());

    menu.add(MenuItem::item(Icons::inverseDown, CommandIDs::InvertChordDown,
        TRANS(I18n::Menu::refactoringInverseDown))->disabledIf(!canRefactor)->closesMenu());

    menu.add(MenuItem::item(Icons::up, CommandIDs::TransposeScaleKeyUp,
        TRANS(I18n::Menu::refactoringInScaleTransposeUp))->disabledIf(!canRefactor)->closesMenu());

    menu.add(MenuItem::item(Icons::down, CommandIDs::TransposeScaleKeyDown,
        TRANS(I18n::Menu::refactoringInScaleTransposeDown))->disabledIf(!canRefactor)->closesMenu());

    // todo icons for refactoring commands:
    menu.add(MenuItem::item(Icons::refactor, CommandIDs::MelodicInversion,
        TRANS(I18n::Menu::refactoringMelodicInversion))->disabledIf(!canRefactor)->closesMenu());

    menu.add(MenuItem::item(Icons::refactor, CommandIDs::Retrograde,
        TRANS(I18n::Menu::refactoringRetrograde))->disabledIf(!canRefactor)->closesMenu());

    menu.add(MenuItem::item(Icons::refactor, CommandIDs::CleanupOverlaps,
        TRANS(I18n::Menu::refactoringCleanup))->disabledIf(!canRefactor)->closesMenu());

    menu.add(MenuItem::item(Icons::cut, CommandIDs::NewTrackFromSelection,
        TRANS(I18n::Menu::Selection::notesToTrack))->closesMenu());

    const bool nowhereToMove = this->project.findChildrenOfType<PianoTrackNode>().size() < 2;

    menu.add(MenuItem::item(Icons::cut,
        TRANS(I18n::Menu::Selection::notesMoveTo))->
        disabledIf(nowhereToMove)->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->createMoveToTrackPanel(), MenuPanel::SlideLeft);
    }));

    const bool canRescale = this->harmonicContextScale != nullptr;

    menu.add(MenuItem::item(Icons::arpeggiate, // todo new icon for this
        TRANS(I18n::Menu::Selection::notesRescale))->
        disabledIf(!canRescale)->
        withSubmenu()->
        withAction([this]()
    {
        this->updateContent(this->createScalesPanel(), MenuPanel::SlideLeft);
    }));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createScalesPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createRefactoringPanel(), MenuPanel::SlideRight);
    }));

    const auto &scales = App::Config().getScales()->getAll();
    for (int i = 0; i < scales.size(); ++i)
    {
        if (scales.getUnchecked(i)->getBasePeriod() !=
            this->project.getProjectInfo()->getTemperament()->getPeriodSize())
        {
            continue;
        }

        menu.add(MenuItem::item(Icons::arpeggiate,
            scales.getUnchecked(i)->getLocalizedName())->
            closesMenu()->
            withAction([this, i]()
        {
            if (this->harmonicContextScale == nullptr)
            {
                jassertfalse;
                return;
            }

            const auto &scales = App::Config().getScales()->getAll();
            SequencerOperations::rescale(*this->lasso.get(), this->harmonicContextKey,
                this->harmonicContextScale, scales[i], true);
        }));
    }

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createQuantizationPanel()
{
    MenuPanel::Menu menu;

    using namespace I18n::Menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
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

MenuPanel::Menu PianoRollSelectionMenu::createTupletsPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
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

MenuPanel::Menu PianoRollSelectionMenu::createArpsPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::create,
        CommandIDs::CreateArpeggiatorFromSelection,
        TRANS(I18n::Menu::arpeggiatorsCreate))->closesMenu());

    const auto arps = App::Config().getArpeggiators()->getAll();
    for (int i = 0; i < arps.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::arpeggiate,
            arps.getUnchecked(i)->getName())->
            closesMenu()->
            withAction([this, i]()
        {
            if (this->lasso->getNumSelected() < 3)
            {
                jassertfalse;
                return;
            }

            const auto arps = App::Config().getArpeggiators()->getAll();

            auto *harmonicContext = dynamic_cast<KeySignaturesSequence *>(
                this->project.getTimeline()->getKeySignatures()->getSequence());

            SequencerOperations::arpeggiate(*this->lasso.get(), arps[i],
                this->project.getProjectInfo()->getTemperament(),
                harmonicContext,
                this->project.getTimeline()->getTimeSignaturesAggregator(),
                1.0f, 0.0f, false, false, true);
        }));
    }

    return menu;
}

PianoRollSelectionMenu::PianoRollSelectionMenu(ProjectNode &project, WeakReference<Lasso> lasso) :
    project(project),
    lasso(lasso)
{
    if (this->lasso->getNumSelected() > 0)
    {
        const Clip &clip = this->lasso->getFirstAs<NoteComponent>()->getClip();
        if (!SequencerOperations::findHarmonicContext(*this->lasso.get(), clip,
            this->project.getTimeline()->getKeySignatures(),
            this->harmonicContextScale, this->harmonicContextKey, this->harmonicContextKeyName))
        {
            DBG("Warning: harmonic context could not be detected");
        }
    }

    this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
}

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
#include "PianoRollSelectionMenu.h"

#include "Lasso.h"
#include "SequencerOperations.h"

#include "Arpeggiator.h"
#include "NoteComponent.h"

#include "ProjectTimeline.h"
#include "MidiTrack.h"

#include "CommandIDs.h"
#include "Icons.h"

#include "MainLayout.h"
#include "ModalDialogInput.h"
#include "Config.h"

MenuPanel::Menu PianoRollSelectionMenu::createDefaultPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents,
        TRANS(I18n::Menu::Selection::notesCopy))->closesMenu());

    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS(I18n::Menu::Selection::notesCut))->closesMenu());

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteEvents,
        TRANS(I18n::Menu::Selection::notesDelete))->closesMenu());

    menu.add(MenuItem::item(Icons::refactor,
        TRANS(I18n::Menu::Selection::notesRefactor))->
        withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createRefactoringPanel(), MenuPanel::SlideLeft);
    }));

    const bool canArpeggiate = (this->lasso->getNumSelected() > 1) && (this->harmonicContextScale != nullptr);

    menu.add(MenuItem::item(Icons::arpeggiate,
        TRANS(I18n::Menu::Selection::notesArpeggiate))->
        disabledIf(!canArpeggiate)->
        withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createArpsPanel(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::ellipsis,
        TRANS(I18n::Menu::Selection::notesDivisions))->
        withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createTupletsPanel(), MenuPanel::SlideLeft);
    }));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createRefactoringPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));
    
    menu.add(MenuItem::item(Icons::cut, CommandIDs::NewTrackFromSelection,
        TRANS(I18n::Menu::Selection::notesToTrack))->closesMenu());

    const bool canInvert = this->lasso->getNumSelected() > 1;

    menu.add(MenuItem::item(Icons::inverseUp, CommandIDs::InvertChordUp,
        TRANS(I18n::Menu::refactoringInverseUp))->disabledIf(!canInvert)->closesMenu());

    menu.add(MenuItem::item(Icons::inverseDown, CommandIDs::InvertChordDown,
        TRANS(I18n::Menu::refactoringInverseDown))->disabledIf(!canInvert)->closesMenu());

    // TODO
    // Cleanup
    // Not implemented:
    // Double time
    // Half time

    const bool canRescale = (this->harmonicContextScale != nullptr);

    menu.add(MenuItem::item(Icons::arpeggiate, // todo new icon for this
        TRANS(I18n::Menu::Selection::notesRescale))->
        disabledIf(!canRescale)->withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createScalesPanel(), MenuPanel::SlideLeft);
    }));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createScalesPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createRefactoringPanel(), MenuPanel::SlideRight);
    }));

    const auto &scales = App::Config().getScales()->getAll();
    for (int i = 0; i < scales.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::arpeggiate, scales.getUnchecked(i)->getLocalizedName())->withAction([this, i]()
        {
            if (this->harmonicContextScale == nullptr)
            {
                jassertfalse;
                this->dismiss();
                return;
            }

            const auto &scales = App::Config().getScales()->getAll();
            SequencerOperations::rescale(*this->lasso, this->harmonicContextKey,
                this->harmonicContextScale, scales[i], true);

            this->dismiss();
            return;
        }));
    }

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createTupletsPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withTimer()->withAction([this]()
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

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::create, CommandIDs::CreateArpeggiatorFromSelection,
        TRANS(I18n::Menu::arpeggiatorsCreate))->closesMenu());

    const auto arps = App::Config().getArpeggiators()->getAll();
    for (int i = 0; i < arps.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::arpeggiate, arps.getUnchecked(i)->getName())->withAction([this, i]()
        {
            if (this->lasso->getNumSelected() < 2 || this->harmonicContextScale == nullptr)
            {
                jassertfalse;
                this->dismiss();
                return;
            }

            const auto arps = App::Config().getArpeggiators()->getAll();
            SequencerOperations::arpeggiate(*this->lasso, this->harmonicContextScale,
                this->harmonicContextKey, arps[i], 1.0f, 0.0f, false, false, true);

            this->dismiss();
            return;
        }));
    }

    return menu;
}

PianoRollSelectionMenu::PianoRollSelectionMenu(WeakReference<Lasso> lasso, WeakReference<MidiTrack> keySignatures) :
    lasso(lasso)
{
    if (this->lasso->getNumSelected() > 0)
    {
        const Clip &clip = this->lasso->getFirstAs<NoteComponent>()->getClip();
        if (!SequencerOperations::findHarmonicContext(*this->lasso, clip, keySignatures,
            this->harmonicContextScale, this->harmonicContextKey))
        {
            DBG("Warning: harmonic context could not be detected");
        }
    }

    this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
}

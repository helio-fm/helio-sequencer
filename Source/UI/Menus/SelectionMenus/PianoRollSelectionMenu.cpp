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

#include "ScalesManager.h"
#include "ArpeggiatorsManager.h"
#include "Arpeggiator.h"
#include "NoteComponent.h"

#include "ProjectTreeItem.h"
#include "ProjectTimeline.h"
#include "MidiTrack.h"

#include "CommandIDs.h"
#include "Icons.h"

#include "App.h"
#include "MainLayout.h"
#include "ModalDialogInput.h"

MenuPanel::Menu PianoRollSelectionMenu::createDefaultPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents,
        TRANS("menu::selection::notes::copy"))->closesMenu());

    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::selection::notes::cut"))->closesMenu());

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteEvents,
        TRANS("menu::selection::notes::delete"))->closesMenu());

    menu.add(MenuItem::item(Icons::refactor,
        TRANS("menu::selection::notes::refactor"))->
        withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createRefactoringPanel(), MenuPanel::SlideLeft);
    }));

    const bool canArpeggiate = (this->lasso->getNumSelected() > 1) && (this->harmonicContextScale != nullptr);

    menu.add(MenuItem::item(Icons::arpeggiate,
        TRANS("menu::selection::notes::arpeggiate"))->
        disabledIf(!canArpeggiate)->
        withSubmenu()->withAction([this]()
    {
        this->updateContent(this->createArpsPanel(), MenuPanel::SlideLeft);
    }));

    // TODO
    //menu.add(MenuItem::item(Icons::arpeggiate,
    //    TRANS("menu::selection::notes::divisions"))->
    //    withSubmenu()->withAction([this]()
    //{
    //    this->updateContent(this->createTimeDivisionsPanel(), MenuPanel::SlideLeft);
    //}));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createRefactoringPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));
    
    menu.add(MenuItem::item(Icons::cut, CommandIDs::NewTrackFromSelection,
        TRANS("menu::selection::notes::totrack"))->closesMenu());

    // TODO
    // Cleanup
    // Invert up, Invert down
    // Period up, Period down - really worths mentioning here? only for mobile versions?

    // Not implemented:
    // Double time
    // Half time

    //menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents, TRANS("menu::selection::piano::cut")));
    //menu.add(MenuItem::item(Icons::trash, CommandIDs::RefactorRemoveOverlaps, TRANS("menu::selection::piano::cleanup")));

    const bool canRescale = (this->harmonicContextScale != nullptr);

    menu.add(MenuItem::item(Icons::arpeggiate, // todo new icon for this
        TRANS("menu::selection::notes::rescale"))->disabledIf(!canRescale)->withTimer()->withAction([this]()
    {
        this->updateContent(this->createScalesPanel(), MenuPanel::SlideLeft);
    }));

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createScalesPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createRefactoringPanel(), MenuPanel::SlideRight);
    }));

    const auto &scales = ScalesManager::getInstance().getScales();
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

            const auto &scales = ScalesManager::getInstance().getScales();
            SequencerOperations::rescale(*this->lasso, this->harmonicContextScale, scales[i], true);
            this->dismiss();
            return;
        }));
    }

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createTimeDivisionsPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::divisions::merge"))->closesMenu());
    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::divisions::tuplet"))->closesMenu());
    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::divisions::triplet"))->closesMenu());
    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::divisions::quadruplet"))->closesMenu());
    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::divisions::quintuplet"))->closesMenu());

    return menu;
}

MenuPanel::Menu PianoRollSelectionMenu::createArpsPanel()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));

    menu.add(MenuItem::item(Icons::create, CommandIDs::CreateArpeggiatorFromSelection,
        TRANS("menu::arpeggiators::create"))->closesMenu());

    const auto arps = ArpeggiatorsManager::getInstance().getArps();
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

            const auto arps = ArpeggiatorsManager::getInstance().getArps();
            SequencerOperations::arpeggiate(*this->lasso, this->harmonicContextScale,
                this->harmonicContextKey, arps[i], false, false, true);

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

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

    menu.add(MenuItem::item(Icons::arpeggiate,
        TRANS("menu::selection::notes::arpeggiate"))->
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
        TRANS("menu::selection::totrack"))->closesMenu());

    // TODO
    // Cleanup
    // Invert up
    // Invert down
    // Period up
    // Period down
    // Double time
    // Half time

    //menu.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents, TRANS("menu::selection::piano::cut")));
    //menu.add(MenuItem::item(Icons::trash, CommandIDs::RefactorRemoveOverlaps, TRANS("menu::selection::piano::cleanup")));

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
    MenuPanel::Menu cmds;

    cmds.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
    }));

    cmds.add(MenuItem::item(Icons::create,
        CommandIDs::CreateArpeggiatorFromSelection, TRANS("menu::arpeggiators::create")));

    const auto arps = ArpeggiatorsManager::getInstance().getArps();
    for (int i = 0; i < arps.size(); ++i)
    {
        cmds.add(MenuItem::item(Icons::arpeggiate, arps.getUnchecked(i)->getName())->withAction([this, i]()
        {
            if (this->lasso->getNumSelected() < 2)
            {
                App::Layout().showTooltip(TRANS("menu::arpeggiators::error"));
                this->dismiss();
                return;
            }

            Note::Key rootKey = -1;
            Scale::Ptr scale = nullptr;

            const auto keySignatures = this->project.getTimeline()->getKeySignatures();
            if (!SequencerOperations::findHarmonicContext(*this->lasso, keySignatures, scale, rootKey))
            {
                App::Layout().showTooltip(TRANS("menu::arpeggiators::error"));
                this->dismiss();
                return;
            }

            const auto arps = ArpeggiatorsManager::getInstance().getArps();
            SequencerOperations::arpeggiate(*this->lasso, scale, rootKey, arps[i], false, false, true);
            this->dismiss();
            return;
        }));
    }

    return cmds;
}

PianoRollSelectionMenu::PianoRollSelectionMenu(WeakReference<Lasso> lasso, const ProjectTreeItem &project) :
    lasso(lasso),
    project(project)
{
    this->updateContent(this->createDefaultPanel(), MenuPanel::SlideRight);
}

void PianoRollSelectionMenu::handleCommandMessage(int commandId)
{
    jassert(this->lasso != nullptr);
    
    // FIXME: move this into PianoRoll and assign a hotkey
    if (commandId == CommandIDs::CreateArpeggiatorFromSelection)
    {
        // A scenario from user's point:
        // 1. User creates a sequence of notes strictly within a certain scale,
        // 2. User selects `create arpeggiator from sequence`
        // 3. App checks that the entire sequence is within single scale and adds arp model
        // 4. User select a number of chords and clicks `arpeggiate`

        if (this->lasso->getNumSelected() < 2)
        {
            App::Layout().showTooltip(TRANS("menu::arpeggiators::error"));
            this->dismiss();
            return;
        }

        const auto keySignatures = this->project.getTimeline()->getKeySignatures();

        Note::Key rootKey = -1;
        Scale::Ptr scale = nullptr;
        if (!SequencerOperations::findHarmonicContext(*this->lasso, keySignatures, scale, rootKey))
        {
            App::Layout().showTooltip(TRANS("menu::arpeggiators::error"));
            this->dismiss();
            return;
        }

        Array<Note> selectedNotes;
        for (int i = 0; i < this->lasso->getNumSelected(); ++i)
        {
            const auto nc = static_cast<NoteComponent *>(this->lasso->getSelectedItem(i));
            selectedNotes.add(nc->getNote());
        }

        auto newArpDialog = ModalDialogInput::Presets::newArpeggiator();
        newArpDialog->onOk = [scale, rootKey, selectedNotes](const String &name)
        {
            Arpeggiator::Ptr arp(new Arpeggiator(name, scale, selectedNotes, rootKey));
            App::Helio().getResourceManagerFor(Serialization::Resources::arpeggiators).updateUserResource(arp);
        };

        App::Layout().showModalComponentUnowned(newArpDialog.release());
        this->dismiss();
        return;
    }
}

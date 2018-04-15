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

static CommandPanel::Items createDefaultPanel()
{
    CommandPanel::Items cmds;

    cmds.add(CommandItem::withParams(Icons::group, CommandIDs::ArpeggiateNotes,
        TRANS("menu::selection::notes::arpeggiate"))->withSubmenu()->withTimer());

    cmds.add(CommandItem::withParams(Icons::group, CommandIDs::RefactorNotes,
        TRANS("menu::selection::notes::refactor"))->withSubmenu()->withTimer());

    cmds.add(CommandItem::withParams(Icons::group, CommandIDs::BatchTweakNotes,
        TRANS("menu::selection::notes::batch"))->withSubmenu()->withTimer());

    cmds.add(CommandItem::withParams(Icons::copy, CommandIDs::CopyEvents,
        TRANS("menu::selection::notes::copy")));

    cmds.add(CommandItem::withParams(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::selection::notes::cut")));

    cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteEvents,
        TRANS("menu::selection::notes::delete")));

    return cmds;
}

static CommandPanel::Items createRefactoringPanel()
{
    CommandPanel::Items cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back,
        TRANS("menu::back"))->withTimer());

    // TODO
    // Cleanup
    // Invert up
    // Invert down
    // Period up
    // Period down
    // Double time
    // Half time

    //cmds.add(CommandItem::withParams(Icons::cut, CommandIDs::CutEvents, TRANS("menu::selection::piano::cut")));
    //cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::RefactorRemoveOverlaps, TRANS("menu::selection::piano::cleanup")));

    return cmds;
}

static CommandPanel::Items createBatchTweakPanel()
{
    CommandPanel::Items cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back,
        TRANS("menu::back"))->withTimer());

    // TODO
    // Volume randomize
    // Volume slow down

    return cmds;
}

static CommandPanel::Items createArpsPanel()
{
    CommandPanel::Items cmds;

    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back,
        TRANS("menu::back"))->withTimer());

    cmds.add(CommandItem::withParams(Icons::group,
        CommandIDs::CreateArpeggiatorFromSelection, TRANS("menu::arpeggiators::create")));

    const auto arps = ArpeggiatorsManager::getInstance().getArps();
    for (int i = 0; i < arps.size(); ++i)
    {
        cmds.add(CommandItem::withParams(Icons::group,
            (CommandIDs::ApplyArpeggiator + i),
            arps.getUnchecked(i)->getName()));
    }

    return cmds;
}

PianoRollSelectionMenu::PianoRollSelectionMenu(WeakReference<Lasso> lasso, const ProjectTreeItem &project) :
    lasso(lasso),
    project(project)
{
    this->updateContent(createDefaultPanel(), CommandPanel::SlideRight);
}

void PianoRollSelectionMenu::handleCommandMessage(int commandId)
{
    jassert(this->lasso != nullptr);
    const Lasso &selectionReference = *this->lasso;

    if (commandId == CommandIDs::Back)
    {
        this->updateContent(createDefaultPanel(), CommandPanel::SlideRight);
        return;
    }
    else if (commandId == CommandIDs::CopyEvents)
    {
        SequencerOperations::copyToClipboard(App::Clipboard(), selectionReference);
        this->dismiss();
        return;
    }
    else if (commandId == CommandIDs::CutEvents)
    {
        SequencerOperations::copyToClipboard(App::Clipboard(), selectionReference);
        SequencerOperations::deleteSelection(selectionReference);
        this->dismiss();
        return;
    }
    else if (commandId == CommandIDs::DeleteEvents)
    {
        SequencerOperations::deleteSelection(selectionReference);
        this->dismiss();
        return;
    }
    else if (commandId == CommandIDs::CreateArpeggiatorFromSelection)
    {
        // A scenario from user's point:
        // 1. User creates a sequence of notes strictly within a certain scale,
        // 2. User selects `create arpeggiator from sequence`
        // 3. App checks that the entire sequence is within single scale and adds arp model
        // 4. User select a number of chords and clicks `arpeggiate`

        if (this->lasso->getNumSelected() < 2)
        {
            // TODO show error
            this->dismiss();
            return;
        }

        const auto keySignatures = this->project.getTimeline()->getKeySignatures();

        Note::Key rootKey = -1;
        Scale::Ptr scale = nullptr;
        if (!SequencerOperations::findHarmonicContext(*this->lasso, keySignatures, scale, rootKey))
        {
            // TODO error
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
    else if (commandId == CommandIDs::ArpeggiateNotes)
    {
        this->updateContent(createArpsPanel(), CommandPanel::SlideLeft);
        return;
    }
    else if (commandId == CommandIDs::RefactorNotes)
    {
        this->updateContent(createRefactoringPanel(), CommandPanel::SlideLeft);
        return;
    }
    else if (commandId == CommandIDs::BatchTweakNotes)
    {
        this->updateContent(createBatchTweakPanel(), CommandPanel::SlideLeft);
        return;
    }

    const int numArps = ArpeggiatorsManager::getInstance().size();
    if (commandId >= CommandIDs::ApplyArpeggiator &&
        commandId < (CommandIDs::ApplyArpeggiator + numArps))
    {
        if (this->lasso->getNumSelected() < 2)
        {
            // TODO show error
            this->dismiss();
            return;
        }

        Note::Key rootKey = -1;
        Scale::Ptr scale = nullptr;

        const auto keySignatures = this->project.getTimeline()->getKeySignatures();
        if (!SequencerOperations::findHarmonicContext(*this->lasso, keySignatures, scale, rootKey))
        {
            // TODO error
            this->dismiss();
            return;
        }

        const int arpIndex = commandId - CommandIDs::ApplyArpeggiator;
        const auto arps = ArpeggiatorsManager::getInstance().getArps();
        SequencerOperations::arpeggiate(*this->lasso, scale, rootKey, arps[arpIndex], false, false, true);
        this->dismiss();
        return;
    }   
}

void PianoRollSelectionMenu::dismiss() const
{
    if (Component *parent = this->getParentComponent())
    {
        parent->exitModalState(0);
    }
}

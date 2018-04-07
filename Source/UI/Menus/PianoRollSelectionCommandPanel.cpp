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
#include "PianoRollSelectionCommandPanel.h"
#include "ArpeggiatorsManager.h"
#include "SequencerOperations.h"
#include "CommandIDs.h"
#include "Icons.h"
#include "App.h"

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

static CommandPanel::Items createArpsPanel(int selectedArp)
{
    CommandPanel::Items cmds;
    const auto arps = ArpeggiatorsManager::getInstance().getArps();
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back,
        TRANS("menu::back"))->withTimer());

    for (int i = 0; i < arps.size(); ++i)
    {
        const bool isSelectedArp = (i == selectedArp);

        cmds.add(CommandItem::withParams(Icons::group,
            (CommandIDs::ApplyArpeggiator + i),
            arps.getUnchecked(i)->getName())->toggled(isSelectedArp));
    }

    return cmds;
}

PianoRollSelectionCommandPanel::PianoRollSelectionCommandPanel(WeakReference<Lasso> lasso) :
    lasso(lasso)
{
    this->updateContent(createDefaultPanel(), CommandPanel::SlideRight);
}

void PianoRollSelectionCommandPanel::handleCommandMessage(int commandId)
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
        SequencerOperations::deleteSelection(*this->lasso);
        this->dismiss();
        return;
    }
    else if (commandId == CommandIDs::DeleteEvents)
    {
        SequencerOperations::deleteSelection(selectionReference);
        this->dismiss();
        return;
    }
    else if (commandId == CommandIDs::ArpeggiateNotes)
    {
        this->updateContent(createArpsPanel(0), CommandPanel::SlideLeft);
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
}

void PianoRollSelectionCommandPanel::dismiss() const
{
    if (Component *parent = this->getParentComponent())
    {
        parent->exitModalState(0);
    }
}

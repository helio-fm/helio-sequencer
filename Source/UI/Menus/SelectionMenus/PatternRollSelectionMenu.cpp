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
#include "PatternRollSelectionMenu.h"
#include "App.h"
#include "Lasso.h"
#include "CommandIDs.h"
#include "Icons.h"

static CommandPanel::Items createDefaultPanel()
{
    CommandPanel::Items cmds;

    cmds.add(CommandItem::withParams(Icons::copy, CommandIDs::CopyEvents,
        TRANS("menu::selection::clips::copy")));

    cmds.add(CommandItem::withParams(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::selection::clips::cut")));

    cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteEvents,
        TRANS("menu::selection::clips::delete")));

    return cmds;
}

PatternRollSelectionMenu::PatternRollSelectionMenu(WeakReference<Lasso> lasso) :
    lasso(lasso)
{
    this->updateContent(createDefaultPanel(), CommandPanel::SlideLeft);
}

void PatternRollSelectionMenu::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::CopyEvents)
    {
        return;
    }
}

void PatternRollSelectionMenu::dismiss() const
{
    if (Component *parent = this->getParentComponent())
    {
        parent->exitModalState(0);
    }
}

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

static MenuPanel::Menu createDefaultPanel()
{
    MenuPanel::Menu cmds;

    cmds.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents,
        TRANS("menu::selection::clips::copy")));

    cmds.add(MenuItem::item(Icons::cut, CommandIDs::CutEvents,
        TRANS("menu::selection::clips::cut")));

    cmds.add(MenuItem::item(Icons::remove, CommandIDs::DeleteEvents,
        TRANS("menu::selection::clips::delete")));

    return cmds;
}

PatternRollSelectionMenu::PatternRollSelectionMenu(WeakReference<Lasso> lasso) :
    lasso(lasso)
{
    this->updateContent(createDefaultPanel(), MenuPanel::SlideRight);
}

void PatternRollSelectionMenu::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::CopyEvents)
    {
        return;
    }
}


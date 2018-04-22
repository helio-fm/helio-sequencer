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
#include "VersionControlHistorySelectionMenu.h"
#include "VersionControl.h"
#include "CommandIDs.h"
#include "Icons.h"

static MenuPanel::Menu createDefaultPanel()
{
    MenuPanel::Menu cmds;

    cmds.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents,
        TRANS("menu::selection::history::checkout")));

    // TODO
    //cmds.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents,
    //    TRANS("menu::selection::history::merge")));

    return cmds;
}

VersionControlHistorySelectionMenu::VersionControlHistorySelectionMenu(ValueTree revision, VersionControl &vcs) :
    revision(revision),
    vcs(vcs)
{
    // TODO replace with RevisionComponent
    this->updateContent(createDefaultPanel(), MenuPanel::SlideRight);
}

void VersionControlHistorySelectionMenu::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::Back)
    {
        this->updateContent(createDefaultPanel(), MenuPanel::SlideRight);
        return;
    }
    else if (commandId == CommandIDs::CopyEvents)
    {
        this->dismiss();
        return;
    }
}

void VersionControlHistorySelectionMenu::dismiss() const
{
    if (Component *parent = this->getParentComponent())
    {
        parent->exitModalState(0);
    }
}

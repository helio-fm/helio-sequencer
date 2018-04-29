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
#include "VersionControlStageSelectionMenu.h"
#include "VersionControl.h"
#include "CommandIDs.h"
#include "Icons.h"

VersionControlStageSelectionMenu::VersionControlStageSelectionMenu(const SparseSet<int> &selectedChanges, VersionControl &vcs) :
    selectedChanges(selectedChanges),
    vcs(vcs)
{
    MenuPanel::Menu cmds;

    cmds.add(MenuItem::item(Icons::commit, TRANS("menu::selection::vcs::commit"))->withAction([this]()
    {
        this->dismiss();
    }));

    cmds.add(MenuItem::item(Icons::reset, TRANS("menu::selection::vcs::reset"))->withAction([this]()
    {
        this->dismiss();
    }));

    // TODO add named stashes in addition to the default one
    //cmds.add(MenuItem::item(Icons::stash, CommandIDs::DeleteEvents,
    //    TRANS("menu::selection::vcs::stash")));

    cmds.add(MenuItem::item(Icons::selectAll, CommandIDs::VersionControlSelectAll,
        TRANS("menu::selection::vcs::selectall")));

    cmds.add(MenuItem::item(Icons::selectNone, CommandIDs::VersionControlSelectNone,
        TRANS("menu::selection::vcs::selectnone")));

    this->updateContent(cmds, MenuPanel::SlideRight);
}

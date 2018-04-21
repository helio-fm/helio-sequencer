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
#include "VersionControlMenu.h"

#include "StageComponent.h"
#include "VersionControl.h"
#include "ProjectTreeItem.h"

#include "CommandPanel.h"
#include "CommandIDs.h"
#include "Icons.h"

static CommandPanel::Items createDefaultPanel()
{
    CommandPanel::Items cmds;

    // TODO
    // if default stash is full and no changes present: "toggle on"
    // if default stash is empty and there are changes on stage: "toggle on"
    // else, disabled? or shows error messages?

    cmds.add(CommandItem::withParams(Icons::toggleOn, CommandIDs::VersionControlToggleQuickStash,
        TRANS("menu::vcs::togglechanges")));

    cmds.add(CommandItem::withParams(Icons::commit, CommandIDs::VersionControlCommitAll,
        TRANS("menu::vcs::commitall")));

    cmds.add(CommandItem::withParams(Icons::reset, CommandIDs::VersionControlResetAll,
        TRANS("menu::vcs::resetall")));

    cmds.add(CommandItem::withParams(Icons::pull, CommandIDs::VersionControlPull,
        TRANS("menu::vcs::pull")));

    cmds.add(CommandItem::withParams(Icons::push, CommandIDs::VersionControlPush,
        TRANS("menu::vcs::push")));

    // TODO when stashes are ready
    //cmds.add(CommandItem::withParams(Icons::copy, CommandIDs::CopyEvents,
    //    TRANS("menu::vcs::pop"))->withSubmenu()->withTimer());

    return cmds;
}

VersionControlMenu::VersionControlMenu(ProjectTreeItem &parentProject, VersionControl &versionControl)
    : vcs(versionControl),
      project(parentProject)
{
    this->updateContent(createDefaultPanel(), CommandPanel::SlideRight);
}

void VersionControlMenu::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::Back)
    {
        this->updateContent(createDefaultPanel(), CommandPanel::SlideRight);
        return;
    }
    else if (commandId == CommandIDs::CopyEvents)
    {
        this->dismiss();
        return;
    }
}

void VersionControlMenu::dismiss() const
{
    if (Component *parent = this->getParentComponent())
    {
        parent->exitModalState(0);
    }
}

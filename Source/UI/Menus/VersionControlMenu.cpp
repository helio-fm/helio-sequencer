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

#include "Workspace.h"
#include "UserProfile.h"

#include "MenuPanel.h"
#include "CommandIDs.h"
#include "Icons.h"

VersionControlMenu::VersionControlMenu(VersionControl &vcs)
{
    MenuPanel::Menu menu;

    const bool noStashNoChanges = !vcs.hasQuickStash() && !vcs.getHead().hasAnythingOnTheStage();
    const Icons::Id stashIcon = vcs.hasQuickStash() ? Icons::toggleOff : Icons::toggleOn;
    const String stashMessage = vcs.hasQuickStash() ?
        TRANS("menu::vcs::changes::show") : TRANS("menu::vcs::changes::hide");

    menu.add(MenuItem::item(stashIcon,
        CommandIDs::VersionControlToggleQuickStash,
        stashMessage)->disabledIf(noStashNoChanges)->closesMenu());

    menu.add(MenuItem::item(Icons::commit,
        CommandIDs::VersionControlCommitAll,
        TRANS("menu::vcs::commitall"))->closesMenu());

    menu.add(MenuItem::item(Icons::reset,
        CommandIDs::VersionControlResetAll,
        TRANS("menu::vcs::resetall"))->closesMenu());

    const bool loggedIn = App::Workspace().getUserProfile().isLoggedIn();

    menu.add(MenuItem::item(Icons::push,
        CommandIDs::VersionControlSyncAll,
        TRANS("menu::vcs::syncall"))->disabledIf(!loggedIn)->closesMenu());

    // TODO when stashes are ready
    //menu.add(MenuItem::item(Icons::stash, CommandIDs::VersionControlPopStash,
    //    TRANS("menu::vcs::stash"))->withSubmenu());

    this->updateContent(menu, MenuPanel::SlideRight);
}

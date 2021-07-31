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

#include "VersionControl.h"
#include "Workspace.h"
#include "Network.h"
#include "SessionService.h"
#include "CommandIDs.h"

VersionControlMenu::VersionControlMenu(VersionControl &vcs)
{
    MenuPanel::Menu menu;

    const bool noStashNoChanges = !vcs.hasQuickStash() && !vcs.getHead().hasAnythingOnTheStage();
    const Icons::Id stashIcon = vcs.hasQuickStash() ? Icons::toggleOff : Icons::toggleOn;
    const String stashMessage = vcs.hasQuickStash() ?
        TRANS(I18n::Menu::vcsChangesShow) : TRANS(I18n::Menu::vcsChangesHide);

    menu.add(MenuItem::item(stashIcon,
        CommandIDs::VersionControlToggleQuickStash,
        stashMessage)->disabledIf(noStashNoChanges)->closesMenu());

    menu.add(MenuItem::item(Icons::commit,
        CommandIDs::VersionControlCommitAll,
        TRANS(I18n::Menu::vcsCommitAll))->closesMenu());

    menu.add(MenuItem::item(Icons::reset,
        CommandIDs::VersionControlResetAll,
        TRANS(I18n::Menu::vcsResetAll))->closesMenu());

#if !NO_NETWORK

    const bool loggedIn = App::Workspace().getUserProfile().isLoggedIn();

    // show username & pic somewhere?
    // userProfile.getAvatar()
    // "/" + userProfile.getLogin()
    // URL(userProfile.getProfileUrl()).launchInDefaultBrowser();

    menu.add(MenuItem::item(Icons::push,
        CommandIDs::VersionControlSyncAll,
        TRANS(I18n::Menu::vcsSyncAll))->disabledIf(!loggedIn)->closesMenu());

    if (!loggedIn)
    {
        menu.add(MenuItem::item(Icons::github,
            TRANS(I18n::Dialog::authGithub))->
            disabledIf(loggedIn)->closesMenu()->withAction([]()
        {
            App::Network().getSessionService()->signIn("Github");
        }));
    }

#endif

    // TODO when stashes are ready
    //menu.add(MenuItem::item(Icons::stash, CommandIDs::VersionControlPopStash,
    //    TRANS(I18n::Menu::vcsStash))->withSubmenu());

    this->updateContent(menu, MenuPanel::SlideRight);
}

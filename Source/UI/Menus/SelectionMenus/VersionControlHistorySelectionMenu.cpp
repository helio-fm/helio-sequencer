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
#include "RevisionTooltipComponent.h"
#include "CommandIDs.h"

static MenuPanel::Menu createDefaultPanel(VCS::Revision::Ptr revision, VersionControl &vcs)
{
    MenuPanel::Menu menu;

    const auto syncState = vcs.getRevisionSyncState(revision);
    const bool needsPush = (syncState == VCS::Revision::NoSync);
    const bool needsPull = (syncState == VCS::Revision::ShallowCopy);

    menu.add(MenuItem::item(Icons::versionControl, CommandIDs::VersionControlCheckout,
        TRANS(I18n::Menu::Selection::vcsCheckout))->disabledIf(needsPull)->closesMenu());

    menu.add(MenuItem::item(Icons::push, CommandIDs::VersionControlPushSelected,
        TRANS(I18n::Menu::Selection::vcsPush))->disabledIf(!needsPush)->closesMenu());

    menu.add(MenuItem::item(Icons::pull, CommandIDs::VersionControlPullSelected,
        TRANS(I18n::Menu::Selection::vcsPull))->disabledIf(!needsPull)->closesMenu());

    return menu;
}

VersionControlHistorySelectionMenu::VersionControlHistorySelectionMenu(VCS::Revision::Ptr revision, VersionControl &vcs) :
    revision(revision),
    vcs(vcs)
{
    UniquePointer<Component> content;
    if (!revision->isShallowCopy())
    {
        content = make<RevisionTooltipComponent>(revision);
    }

    this->updateContent(createDefaultPanel(revision, vcs),
        MenuPanel::SlideRight, true, content.release());
}

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
#include "Icons.h"

static MenuPanel::Menu createDefaultPanel(VCS::Revision::Ptr revision, VersionControl &vcs)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::versionControl, CommandIDs::VersionControlCheckout,
        TRANS("menu::selection::vcs::checkout"))->closesMenu());

    const auto syncState = vcs.getRevisionSyncState(revision);
    const bool needsPush = (syncState == VCS::Revision::NoSync);
    const bool needsPull = (syncState == VCS::Revision::ShallowCopy);

    menu.add(MenuItem::item(Icons::push, CommandIDs::VersionControlPushSelected,
        TRANS("menu::selection::vcs::push"))->disabledIf(!needsPush)->closesMenu());

    menu.add(MenuItem::item(Icons::pull, CommandIDs::VersionControlPullSelected,
        TRANS("menu::selection::vcs::pull"))->disabledIf(!needsPull)->closesMenu());

    return menu;
}

VersionControlHistorySelectionMenu::VersionControlHistorySelectionMenu(VCS::Revision::Ptr revision, VersionControl &vcs) :
    revision(revision),
    vcs(vcs)
{
    ScopedPointer<Component> content;
    if (!revision->isShallowCopy())
    {
        content.reset(new RevisionTooltipComponent(revision));
    }

    this->updateContent(createDefaultPanel(revision, vcs), MenuPanel::SlideRight, true, content.release());
}

/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "VersionControlHistorySelectionMenu.h"
#include "VersionControl.h"
#include "RevisionTooltipComponent.h"
#include "CommandIDs.h"

static MenuPanel::Menu createDefaultPanel(VCS::Revision::Ptr revision, VersionControl &vcs)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::versionControl, CommandIDs::VersionControlCheckout,
        TRANS(I18n::Menu::Selection::vcsCheckout))->closesMenu());

    return menu;
}

VersionControlHistorySelectionMenu::VersionControlHistorySelectionMenu(VCS::Revision::Ptr revision, VersionControl &vcs) :
    revision(revision),
    vcs(vcs)
{
    auto content = make<RevisionTooltipComponent>(revision);
    this->updateContent(createDefaultPanel(revision, vcs),
        MenuPanel::SlideRight, true, -1, content.release());
}

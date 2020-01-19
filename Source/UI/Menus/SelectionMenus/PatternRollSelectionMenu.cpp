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
#include "PatternOperations.h"
#include "ClipComponent.h"
#include "Lasso.h"
#include "CommandIDs.h"
#include "Icons.h"

PatternRollSelectionMenu::PatternRollSelectionMenu(WeakReference<Lasso> lasso) :
    lasso(lasso)
{
    if (lasso->getNumSelected() > 0)
    {
        this->initDefaultMenu();
    }
}

static bool canRenamePatternSelection(WeakReference<Lasso> lasso)
{
    const String trackId = lasso->getFirstAs<ClipComponent>()->getClip().getTrackId();
    for (int i = 0; i < lasso->getNumSelected(); ++i)
    {
        if (lasso->getItemAs<ClipComponent>(i)->getClip().getTrackId() != trackId)
        {
            return false;
        }
    }
    return true;
}

void PatternRollSelectionMenu::initDefaultMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::zoomTool, CommandIDs::ZoomEntireClip,
        TRANS(I18n::Menu::Selection::clipsEdit))->
        disabledIf(lasso->getNumSelected() == 0)->closesMenu());

    menu.add(MenuItem::item(Icons::up, CommandIDs::ClipTransposeUp,
        TRANS(I18n::Menu::Selection::clipsTransposeUp)));

    menu.add(MenuItem::item(Icons::down, CommandIDs::ClipTransposeDown,
        TRANS(I18n::Menu::Selection::clipsTransposeDown)));

    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack,
        TRANS(I18n::Menu::trackRename))->disabledIf(!canRenamePatternSelection(this->lasso))->closesMenu());

    const auto muteAction = PatternOperations::lassoContainsMutedClip(*this->lasso) ?
        TRANS(I18n::Menu::unmute) : TRANS(I18n::Menu::mute);

    const auto soloAction = PatternOperations::lassoContainsSoloedClip(*this->lasso) ?
        TRANS(I18n::Menu::unsolo) : TRANS(I18n::Menu::solo);

    // TODO icons
    menu.add(MenuItem::item(Icons::mute, CommandIDs::ToggleMuteClips, muteAction)->closesMenu());

    menu.add(MenuItem::item(Icons::unmute, CommandIDs::ToggleSoloClips, soloAction)->closesMenu());

    const auto canDuplicate = this->lasso->getNumSelected() == 1;

    menu.add(MenuItem::item(Icons::copy, CommandIDs::DuplicateTrack,
        TRANS(I18n::Menu::trackDuplicate))->disabledIf(!canDuplicate)->closesMenu());

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteClips,
        TRANS(I18n::Menu::Selection::clipsDelete))->closesMenu());

    this->updateContent(menu, MenuPanel::SlideRight);
}

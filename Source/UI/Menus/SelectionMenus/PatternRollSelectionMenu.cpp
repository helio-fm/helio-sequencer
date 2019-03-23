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

static bool canRename(WeakReference<Lasso> lasso)
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
        TRANS("menu::selection::clips::edit"))->
        disabledIf(lasso->getNumSelected() == 0)->closesMenu());

    menu.add(MenuItem::item(Icons::up, CommandIDs::ClipTransposeUp,
        TRANS("menu::selection::clips::transpose::up")));

    menu.add(MenuItem::item(Icons::down, CommandIDs::ClipTransposeDown,
        TRANS("menu::selection::clips::transpose::down")));

    menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::RenameTrack,
        TRANS("menu::track::rename"))->disabledIf(!canRename(this->lasso))->closesMenu());

    menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyClips,
        TRANS("menu::selection::clips::copy"))->closesMenu());

    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutClips,
        TRANS("menu::selection::clips::cut"))->closesMenu());

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteClips,
        TRANS("menu::selection::clips::delete"))->closesMenu());

    this->updateContent(menu, MenuPanel::SlideRight);
}


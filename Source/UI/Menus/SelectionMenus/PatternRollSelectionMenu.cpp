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
#include "App.h"
#include "Lasso.h"
#include "CommandIDs.h"
#include "Icons.h"

PatternRollSelectionMenu::PatternRollSelectionMenu(WeakReference<Lasso> lasso) :
    lasso(lasso)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::zoomIn, CommandIDs::EditClip,
        TRANS("menu::selection::clips::edit"))->
        disabledIf(lasso->getNumSelected() == 0)->closesMenu());

    menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyClips,
        TRANS("menu::selection::clips::copy"))->closesMenu());

    menu.add(MenuItem::item(Icons::cut, CommandIDs::CutClips,
        TRANS("menu::selection::clips::cut"))->closesMenu());

    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteClips,
        TRANS("menu::selection::clips::delete"))->closesMenu());

    this->updateContent(menu, MenuPanel::SlideRight);
}

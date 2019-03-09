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
#include "PatternsMenu.h"
#include "CommandIDs.h"

PatternsMenu::PatternsMenu(PatternEditorNode &parentNode) :
    node(parentNode)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::annotation,
        CommandIDs::PatternsGroupByName,
        TRANS("menu::track::selectall"))->closesMenu());
    
    menu.add(MenuItem::item(Icons::colour,
        CommandIDs::PatternsGroupByColour,
        TRANS("menu::track::selectall"))->closesMenu());

    menu.add(MenuItem::item(Icons::instrument,
        CommandIDs::PatternsGroupByInstrument,
        TRANS("menu::track::selectall"))->closesMenu());

    this->updateContent(menu, MenuPanel::SlideRight);
}

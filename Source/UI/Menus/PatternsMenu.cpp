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
#include "ProjectNode.h"
#include "MidiTrack.h"
#include "CommandIDs.h"

PatternsMenu::PatternsMenu(PatternEditorNode &parentNode)
{
    MenuPanel::Menu menu;

    auto grouping = MidiTrack::Grouping::GroupByName;
    if (auto *project = parentNode.findParentOfType<ProjectNode>())
    {
        grouping = project->getTrackGroupingMode();
    }

    menu.add(MenuItem::item(Icons::annotation,
        CommandIDs::PatternsGroupByName,
        TRANS(I18n::Menu::groupByName))->
        disabledIf(grouping == MidiTrack::Grouping::GroupByName)->
        closesMenu());
    
    menu.add(MenuItem::item(Icons::colour,
        CommandIDs::PatternsGroupByColour,
        TRANS(I18n::Menu::groupByColour))->
        disabledIf(grouping == MidiTrack::Grouping::GroupByColour)->
        closesMenu());

    menu.add(MenuItem::item(Icons::instrument,
        CommandIDs::PatternsGroupByInstrument,
        TRANS(I18n::Menu::groupByInstrument))->
        disabledIf(grouping == MidiTrack::Grouping::GroupByInstrument)->
        closesMenu());

    menu.add(MenuItem::item(Icons::list,
        CommandIDs::PatternsGroupById,
        TRANS(I18n::Menu::groupByNone))->
        disabledIf(grouping == MidiTrack::Grouping::GroupByNameId)->
        closesMenu());

    this->updateContent(menu, MenuPanel::SlideRight);
}

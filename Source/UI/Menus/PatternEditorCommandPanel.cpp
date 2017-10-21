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
#include "PatternEditorCommandPanel.h"
#include "ProjectTreeItem.h"
#include "PatternRoll.h"
#include "Icons.h"
#include "App.h"
#include "CommandIDs.h"

PatternEditorCommandPanel::PatternEditorCommandPanel(
    PatternRoll &targetRoll,
    ProjectTreeItem &parentProject) :
    roll(targetRoll),
    project(parentProject)
{
    this->initMainPanel();
}

PatternEditorCommandPanel::~PatternEditorCommandPanel()
{
}

void PatternEditorCommandPanel::handleCommandMessage(int commandId)
{
}

void PatternEditorCommandPanel::initMainPanel()
{
    ReferenceCountedArray<CommandItem> cmds;
    //cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back"))->withTimer());
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void PatternEditorCommandPanel::dismiss()
{
    if (Component *parent = this->getParentComponent())
    {
        parent->exitModalState(0);
    }
}

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
#include "TransientTreeItems.h"
#include "PianoRollSelectionCommandPanel.h"
#include "PatternRollSelectionCommandPanel.h"
#include "CommandPanel.h"
#include "CommandIDs.h"
#include "Icons.h"
#include "App.h"
#include "Workspace.h"

//===----------------------------------------------------------------------===//
// PianoRoll Selection Menu
//===----------------------------------------------------------------------===//

Image PianoRollSelectionTreeItem::getIcon() const
{
    return Icons::findByName(Icons::empty, TREE_LARGE_ICON_HEIGHT);
}

String PianoRollSelectionTreeItem::getCaption() const
{
    return TRANS("tree::selection::piano");
}

Component *PianoRollSelectionTreeItem::createItemMenu()
{
    return nullptr; // new PianoRollSelectionCommandPanel(*this, CommandPanel::SlideRight);
}

//===----------------------------------------------------------------------===//
// PatternRoll Selection Menu
//===----------------------------------------------------------------------===//

Image PatternRollSelectionTreeItem::getIcon() const
{
    return Icons::findByName(Icons::empty, TREE_LARGE_ICON_HEIGHT);
}

String PatternRollSelectionTreeItem::getCaption() const
{
    return TRANS("tree::selection::pattern");
}

Component *PatternRollSelectionTreeItem::createItemMenu()
{
    return nullptr; // new PatternRollSelectionCommandPanel(*this, CommandPanel::SlideRight);
}

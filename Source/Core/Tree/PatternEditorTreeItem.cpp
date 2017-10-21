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
#include "PatternEditorTreeItem.h"
#include "PatternEditorCommandPanel.h"
#include "TreeItemChildrenSerializer.h"
#include "Icons.h"
#include "App.h"
#include "MainLayout.h"
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"

PatternEditorTreeItem::PatternEditorTreeItem() :
    TreeItem("Patterns", Serialization::Core::patternSet)
{
}

PatternEditorTreeItem::~PatternEditorTreeItem()
{
}

Colour PatternEditorTreeItem::getColour() const
{
    return Colour(0xff818dff);
}

Image PatternEditorTreeItem::getIcon() const
{
    return Icons::findByName(Icons::stack, TREE_ICON_HEIGHT);
}

void PatternEditorTreeItem::showPage()
{
    if (ProjectTreeItem *parentProject =
        this->findParentOfType<ProjectTreeItem>())
    {
        parentProject->showPatternEditor(this);
    }
}

void PatternEditorTreeItem::recreatePage()
{
}

String PatternEditorTreeItem::getName() const
{
    return TRANS("tree::patterns");
}

String PatternEditorTreeItem::getStatsString() const
{
    // TODO
    return String::empty;
}


//===----------------------------------------------------------------------===//
// Popup
//===----------------------------------------------------------------------===//

ScopedPointer<Component> PatternEditorTreeItem::createItemMenu()
{
    //TODO

    //if (ProjectTreeItem *parentProject =
    //    this->findParentOfType<ProjectTreeItem>())
    //{
    //    return new PatternEditorCommandPanel(*parentProject);
    //}

    return nullptr;
}


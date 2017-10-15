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
    TreeItem("Patterns")
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


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *PatternEditorTreeItem::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::treeItem);
    xml->setAttribute("type", Serialization::Core::patternSet);

    TreeItemChildrenSerializer::serializeChildren(*this, *xml);

    return xml;
}

void PatternEditorTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    const String& type = xml.getStringAttribute("type");
    if (type != Serialization::Core::patternSet) { return; }

    TreeItemChildrenSerializer::deserializeChildren(*this, xml);
}

void PatternEditorTreeItem::reset()
{
    TreeItem::reset();
}

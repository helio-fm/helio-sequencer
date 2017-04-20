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
#include "LayerTreeItemActions.h"
#include "ProjectTreeItem.h"
#include "LayerTreeItem.h"
#include "TreeItem.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

LayerTreeItemRenameAction::LayerTreeItemRenameAction(ProjectTreeItem &parentProject,
                                                     String targetLayerId,
                                                     String newXPath) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    xPathAfter(std::move(newXPath))
{
}

bool LayerTreeItemRenameAction::perform()
{
    if (LayerTreeItem *treeItem =
        this->project.findChildByLayerId<LayerTreeItem>(this->layerId))
    {
        this->xPathBefore = treeItem->getXPath();
        treeItem->onRename(this->xPathAfter);
        return true;
    }
    
    return false;
}

bool LayerTreeItemRenameAction::undo()
{
    if (LayerTreeItem *treeItem =
        this->project.findChildByLayerId<LayerTreeItem>(this->layerId))
    {
        treeItem->onRename(this->xPathBefore);
        return true;
    }
    
    return false;
}

int LayerTreeItemRenameAction::getSizeInUnits()
{
    return this->xPathBefore.length() + this->xPathAfter.length();
}

XmlElement *LayerTreeItemRenameAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::layerTreeItemRenameAction);
    xml->setAttribute(Serialization::Undo::xPathBefore, this->xPathBefore);
    xml->setAttribute(Serialization::Undo::xPathAfter, this->xPathAfter);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    return xml;
}

void LayerTreeItemRenameAction::deserialize(const XmlElement &xml)
{
    this->xPathBefore = xml.getStringAttribute(Serialization::Undo::xPathBefore);
    this->xPathAfter = xml.getStringAttribute(Serialization::Undo::xPathAfter);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void LayerTreeItemRenameAction::reset()
{
    this->xPathBefore.clear();
    this->xPathAfter.clear();
    this->layerId.clear();
}

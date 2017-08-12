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
#include "MidiTrackTreeItem.h"
#include "TreeItem.h"

//===----------------------------------------------------------------------===//
// Rename/Move
//===----------------------------------------------------------------------===//

MidiTrackRenameAction::MidiTrackRenameAction(ProjectTreeItem &parentProject,
                                                     String targetLayerId,
                                                     String newXPath) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    xPathAfter(std::move(newXPath))
{
}

bool MidiTrackRenameAction::perform()
{
    if (MidiTrackTreeItem *treeItem =
        this->project.findChildByLayerId<MidiTrackTreeItem>(this->layerId))
    {
        this->xPathBefore = treeItem->getXPath();
        treeItem->onRename(this->xPathAfter);
        return true;
    }
    
    return false;
}

bool MidiTrackRenameAction::undo()
{
    if (MidiTrackTreeItem *treeItem =
        this->project.findChildByLayerId<MidiTrackTreeItem>(this->layerId))
    {
        treeItem->onRename(this->xPathBefore);
        return true;
    }
    
    return false;
}

int MidiTrackRenameAction::getSizeInUnits()
{
    return this->xPathBefore.length() + this->xPathAfter.length();
}

XmlElement *MidiTrackRenameAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::layerTreeItemRenameAction);
    xml->setAttribute(Serialization::Undo::xPathBefore, this->xPathBefore);
    xml->setAttribute(Serialization::Undo::xPathAfter, this->xPathAfter);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    return xml;
}

void MidiTrackRenameAction::deserialize(const XmlElement &xml)
{
    this->xPathBefore = xml.getStringAttribute(Serialization::Undo::xPathBefore);
    this->xPathAfter = xml.getStringAttribute(Serialization::Undo::xPathAfter);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
}

void MidiTrackRenameAction::reset()
{
    this->xPathBefore.clear();
    this->xPathAfter.clear();
    this->layerId.clear();
}

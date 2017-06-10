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
#include "AutoLayerTreeItemActions.h"
#include "ProjectTreeItem.h"
#include "AutomationLayerTreeItem.h"
#include "TreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AutoLayerTreeItemInsertAction::AutoLayerTreeItemInsertAction(ProjectTreeItem &parentProject,
                                                             String targetSerializedState,
                                                             String targetXPath) :
    UndoAction(parentProject),
    serializedState(std::move(targetSerializedState)),
    xPath(std::move(targetXPath))
{
}

bool AutoLayerTreeItemInsertAction::perform()
{
    MidiLayerTreeItem *layer = new AutomationLayerTreeItem("empty");
    this->project.addChildTreeItem(layer);
    
    ScopedPointer<XmlElement> layerState = XmlDocument::parse(this->serializedState);
    layer->deserialize(*layerState);
    
    this->layerId = layer->getLayer()->getLayerIdAsString();
    layer->onRename(this->xPath);
    
    return true;
}

bool AutoLayerTreeItemInsertAction::undo()
{
    if (AutomationLayerTreeItem *treeItem =
        this->project.findChildByLayerId<AutomationLayerTreeItem>(this->layerId))
    {
        // here the item state should be the same as when it was created
        // so don't serialize anything again
        return TreeItem::deleteItem(treeItem);
    }
    
    return false;
}

int AutoLayerTreeItemInsertAction::getSizeInUnits()
{
    return this->xPath.length();
}

XmlElement *AutoLayerTreeItemInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::autoLayerTreeItemInsertAction);
    xml->setAttribute(Serialization::Undo::xPath, this->xPath);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(XmlDocument::parse(this->serializedState));
    return xml;
}

void AutoLayerTreeItemInsertAction::deserialize(const XmlElement &xml)
{
    this->xPath = xml.getStringAttribute(Serialization::Undo::xPath);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->serializedState = xml.getFirstChildElement()->createDocument("");
}

void AutoLayerTreeItemInsertAction::reset()
{
    this->xPath.clear();
    this->layerId.clear();
    this->serializedState.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AutoLayerTreeItemRemoveAction::AutoLayerTreeItemRemoveAction(ProjectTreeItem &parentProject,
                                                             String targetLayerId) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    numEvents(0)
{
}

bool AutoLayerTreeItemRemoveAction::perform()
{
    if (AutomationLayerTreeItem *treeItem =
        this->project.findChildByLayerId<AutomationLayerTreeItem>(this->layerId))
    {
        this->numEvents = treeItem->getLayer()->size();
        this->serializedTreeItem = treeItem->serialize();
        this->xPath = treeItem->getXPath();
        return TreeItem::deleteItem(treeItem);
    }
    
    return false;
}

bool AutoLayerTreeItemRemoveAction::undo()
{
    if (this->serializedTreeItem != nullptr)
    {
        MidiLayerTreeItem *layer = new AutomationLayerTreeItem("empty");
        this->project.addChildTreeItem(layer);
        layer->deserialize(*this->serializedTreeItem);
        layer->onRename(this->xPath);
        return true;
    }
    
    return false;
}

int AutoLayerTreeItemRemoveAction::getSizeInUnits()
{
    if (this->serializedTreeItem != nullptr)
    {
        return (this->numEvents * sizeof(MidiEvent));
    }
    
    return 1;
}

XmlElement *AutoLayerTreeItemRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::autoLayerTreeItemRemoveAction);
    xml->setAttribute(Serialization::Undo::xPath, this->xPath);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(new XmlElement(*this->serializedTreeItem)); // deep copy
    return xml;
}

void AutoLayerTreeItemRemoveAction::deserialize(const XmlElement &xml)
{
    this->xPath = xml.getStringAttribute(Serialization::Undo::xPath);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->serializedTreeItem = new XmlElement(*xml.getFirstChildElement()); // deep copy
}

void AutoLayerTreeItemRemoveAction::reset()
{
    this->xPath.clear();
    this->layerId.clear();
    this->serializedTreeItem->deleteAllChildElements();
}

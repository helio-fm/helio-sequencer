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
#include "PianoLayerTreeItemActions.h"
#include "ProjectTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "TreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

PianoLayerTreeItemInsertAction::PianoLayerTreeItemInsertAction(ProjectTreeItem &parentProject,
                                                               String targetSerializedState,
                                                               String targetXPath) :
    UndoAction(parentProject),
    serializedState(std::move(targetSerializedState)),
    xPath(std::move(targetXPath))
{
}

bool PianoLayerTreeItemInsertAction::perform()
{
    MidiTrackTreeItem *layer = new PianoTrackTreeItem("empty");
    this->project.addChildTreeItem(layer);
    
    ScopedPointer<XmlElement> layerState = XmlDocument::parse(this->serializedState);
    layer->deserialize(*layerState);

    this->layerId = layer->getLayer()->getLayerIdAsString();
    layer->onRename(this->xPath);

    return true;
}

bool PianoLayerTreeItemInsertAction::undo()
{
    if (PianoTrackTreeItem *treeItem = this->project.findChildByLayerId<PianoTrackTreeItem>(this->layerId))
    {
        // here the item state should be the same as when it was created
        // so don't serialize anything again
        return TreeItem::deleteItem(treeItem);
    }
    
    return false;
}

int PianoLayerTreeItemInsertAction::getSizeInUnits()
{
    return this->xPath.length();
}

XmlElement *PianoLayerTreeItemInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::pianoLayerTreeItemInsertAction);
    xml->setAttribute(Serialization::Undo::xPath, this->xPath);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(XmlDocument::parse(this->serializedState));
    return xml;
}

void PianoLayerTreeItemInsertAction::deserialize(const XmlElement &xml)
{
    this->xPath = xml.getStringAttribute(Serialization::Undo::xPath);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->serializedState = xml.getFirstChildElement()->createDocument("");
}

void PianoLayerTreeItemInsertAction::reset()
{
    this->xPath.clear();
    this->layerId.clear();
    this->serializedState.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

PianoLayerTreeItemRemoveAction::PianoLayerTreeItemRemoveAction(ProjectTreeItem &parentProject,
                                                               String targetLayerId) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    numEvents(0)
{
}

bool PianoLayerTreeItemRemoveAction::perform()
{
    if (PianoTrackTreeItem *treeItem = this->project.findChildByLayerId<PianoTrackTreeItem>(this->layerId))
    {
        this->numEvents = treeItem->getLayer()->size();
        this->serializedTreeItem = treeItem->serialize();
        this->xPath = treeItem->getXPath();
        return TreeItem::deleteItem(treeItem);
    }
    
    return false;
}

bool PianoLayerTreeItemRemoveAction::undo()
{
    if (this->serializedTreeItem != nullptr)
    {
        MidiTrackTreeItem *layer = new PianoTrackTreeItem("empty");
        this->project.addChildTreeItem(layer);
        layer->deserialize(*this->serializedTreeItem);
        layer->onRename(this->xPath);
        return true;
    }
    
    return false;
}

int PianoLayerTreeItemRemoveAction::getSizeInUnits()
{
    if (this->serializedTreeItem != nullptr)
    {
        return (this->numEvents * sizeof(MidiEvent));
    }
    
    return 1;
}

XmlElement *PianoLayerTreeItemRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::pianoLayerTreeItemRemoveAction);
    xml->setAttribute(Serialization::Undo::xPath, this->xPath);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(new XmlElement(*this->serializedTreeItem)); // deep copy
    return xml;
}

void PianoLayerTreeItemRemoveAction::deserialize(const XmlElement &xml)
{
    this->xPath = xml.getStringAttribute(Serialization::Undo::xPath);
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->serializedTreeItem = new XmlElement(*xml.getFirstChildElement()); // deep copy
}

void PianoLayerTreeItemRemoveAction::reset()
{
    this->xPath.clear();
    this->layerId.clear();
    this->serializedTreeItem->deleteAllChildElements();
}

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
#include "PianoTrackActions.h"
#include "ProjectTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "TreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

PianoTrackInsertAction::PianoTrackInsertAction(ProjectTreeItem &parentProject,
                                                               String targetSerializedState,
                                                               String targetXPath) :
    UndoAction(parentProject),
    serializedState(std::move(targetSerializedState)),
    trackName(std::move(targetXPath))
{
}

bool PianoTrackInsertAction::perform()
{
    MidiTrackTreeItem *track = new PianoTrackTreeItem("empty");
    this->project.addChildTreeItem(track);
    
    ScopedPointer<XmlElement> trackState = XmlDocument::parse(this->serializedState);
    track->deserialize(*trackState);

    this->trackId = track->getTrackId().toString();
    track->setTrackName(this->trackName);

    return true;
}

bool PianoTrackInsertAction::undo()
{
    if (PianoTrackTreeItem *treeItem =
        this->project.findTrackById<PianoTrackTreeItem>(this->trackId))
    {
        // here the item state should be the same as when it was created
        // so don't serialize anything again
        return TreeItem::deleteItem(treeItem);
    }
    
    return false;
}

int PianoTrackInsertAction::getSizeInUnits()
{
    return this->trackName.length();
}

XmlElement *PianoTrackInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::pianoTrackInsertAction);
    xml->setAttribute(Serialization::Undo::xPath, this->trackName);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(XmlDocument::parse(this->serializedState));
    return xml;
}

void PianoTrackInsertAction::deserialize(const XmlElement &xml)
{
    this->trackName = xml.getStringAttribute(Serialization::Undo::xPath);
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->serializedState = xml.getFirstChildElement()->createDocument("");
}

void PianoTrackInsertAction::reset()
{
    this->trackName.clear();
    this->trackId.clear();
    this->serializedState.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

PianoTrackRemoveAction::PianoTrackRemoveAction(ProjectTreeItem &parentProject,
    String targetTrackId) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId)),
    numEvents(0)
{
}

bool PianoTrackRemoveAction::perform()
{
    if (PianoTrackTreeItem *treeItem =
        this->project.findTrackById<PianoTrackTreeItem>(this->trackId))
    {
        this->numEvents = treeItem->getSequence()->size();
        this->serializedTreeItem = treeItem->serialize();
        this->trackName = treeItem->getTrackName();
        return TreeItem::deleteItem(treeItem);
    }
    
    return false;
}

bool PianoTrackRemoveAction::undo()
{
    if (this->serializedTreeItem != nullptr)
    {
        MidiTrackTreeItem *track = new PianoTrackTreeItem("empty");
        this->project.addChildTreeItem(track);
        track->deserialize(*this->serializedTreeItem);
        track->setTrackName(this->trackName);
        return true;
    }
    
    return false;
}

int PianoTrackRemoveAction::getSizeInUnits()
{
    if (this->serializedTreeItem != nullptr)
    {
        return (this->numEvents * sizeof(MidiEvent));
    }
    
    return 1;
}

XmlElement *PianoTrackRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::pianoTrackRemoveAction);
    xml->setAttribute(Serialization::Undo::xPath, this->trackName);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(new XmlElement(*this->serializedTreeItem)); // deep copy
    return xml;
}

void PianoTrackRemoveAction::deserialize(const XmlElement &xml)
{
    this->trackName = xml.getStringAttribute(Serialization::Undo::xPath);
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->serializedTreeItem = new XmlElement(*xml.getFirstChildElement()); // deep copy
}

void PianoTrackRemoveAction::reset()
{
    this->trackName.clear();
    this->trackId.clear();
    this->serializedTreeItem->deleteAllChildElements();
}

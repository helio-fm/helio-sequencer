/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "PianoTrackActions.h"
#include "MidiTrackSource.h"
#include "PianoTrackNode.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

PianoTrackInsertAction::PianoTrackInsertAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem) noexcept :
    UndoAction(source),
    parentTreeItem(parentTreeItem) {}

PianoTrackInsertAction::PianoTrackInsertAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem,
    SerializedData targetSerializedState,
    const String &xPath) noexcept :
    UndoAction(source),
    parentTreeItem(parentTreeItem),
    trackState(targetSerializedState),
    trackName(xPath) {}

bool PianoTrackInsertAction::perform()
{
    MidiTrackNode *track = new PianoTrackNode("empty");
    track->deserialize(this->trackState);

    this->parentTreeItem->addChildNode(track);

    this->trackId = track->getTrackId();
    track->setTrackName(this->trackName, false, sendNotification);
    track->dispatchChangeProjectBeatRange();
    return true;
}

bool PianoTrackInsertAction::undo()
{
    if (auto *treeItem = this->source.findTrackById<PianoTrackNode>(this->trackId))
    {
        // here the item state should be the same as when it was created
        // so don't serialize anything again
        return this->parentTreeItem->deleteNode(treeItem, true);
    }
    
    return false;
}

int PianoTrackInsertAction::getSizeInUnits()
{
    return this->trackName.length();
}

SerializedData PianoTrackInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::pianoTrackInsertAction);
    tree.setProperty(Serialization::Undo::path, this->trackName);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->trackState.createCopy());
    return tree;
}

void PianoTrackInsertAction::deserialize(const SerializedData &data)
{
    this->trackName = data.getProperty(Serialization::Undo::path);
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->trackState = data.getChild(0).createCopy();
}

void PianoTrackInsertAction::reset()
{
    this->trackName.clear();
    this->trackId.clear();
    this->trackState = {};
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

PianoTrackRemoveAction::PianoTrackRemoveAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem) noexcept :
    UndoAction(source),
    parentTreeItem(parentTreeItem) {}

PianoTrackRemoveAction::PianoTrackRemoveAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem,
    const String &trackId) noexcept :
    UndoAction(source),
    parentTreeItem(parentTreeItem),
    trackId(trackId) {}

bool PianoTrackRemoveAction::perform()
{
    if (PianoTrackNode *treeItem =
        this->source.findTrackById<PianoTrackNode>(this->trackId))
    {
        this->numEvents = treeItem->getSequence()->size();
        this->serializedTreeItem = treeItem->serialize();
        this->trackName = treeItem->getTrackName();
        return this->parentTreeItem->deleteNode(treeItem, true);
    }
    
    return false;
}

bool PianoTrackRemoveAction::undo()
{
    if (this->serializedTreeItem.isValid())
    {
        MidiTrackNode *track = new PianoTrackNode("empty");
        track->deserialize(this->serializedTreeItem);
        this->parentTreeItem->addChildNode(track);
        track->setTrackName(this->trackName, false, sendNotification);
        track->dispatchChangeProjectBeatRange();
        return true;
    }
    
    return false;
}

int PianoTrackRemoveAction::getSizeInUnits()
{
    if (this->serializedTreeItem.isValid())
    {
        return (this->numEvents * sizeof(MidiEvent));
    }
    
    return 1;
}

SerializedData PianoTrackRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::pianoTrackRemoveAction);
    tree.setProperty(Serialization::Undo::path, this->trackName);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->serializedTreeItem.createCopy());
    return tree;
}

void PianoTrackRemoveAction::deserialize(const SerializedData &data)
{
    this->trackName = data.getProperty(Serialization::Undo::path);
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->serializedTreeItem = data.getChild(0).createCopy();
}

void PianoTrackRemoveAction::reset()
{
    this->trackName.clear();
    this->trackId.clear();
    this->serializedTreeItem = {};
}

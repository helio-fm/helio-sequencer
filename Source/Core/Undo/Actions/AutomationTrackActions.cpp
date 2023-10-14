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
#include "AutomationTrackActions.h"
#include "MidiTrackSource.h"
#include "AutomationTrackNode.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AutomationTrackInsertAction::AutomationTrackInsertAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem) noexcept :
    UndoAction(source), parentTreeItem(parentTreeItem) {}

AutomationTrackInsertAction::AutomationTrackInsertAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem,
    SerializedData targetSerializedState,
    const String &trackName) noexcept :
    UndoAction(source),
    parentTreeItem(parentTreeItem),
    trackState(targetSerializedState),
    trackName(trackName) {}

bool AutomationTrackInsertAction::perform()
{
    MidiTrackNode *track = new AutomationTrackNode("empty");
    track->deserialize(this->trackState);
    this->parentTreeItem->addChildNode(track);

    this->trackId = track->getTrackId();
    track->setTrackName(this->trackName, false, sendNotification);
    
    return true;
}

bool AutomationTrackInsertAction::undo()
{
    if (auto *treeItem =
        this->source.findTrackById<AutomationTrackNode>(this->trackId))
    {
        // here the item state should be the same as when it was created
        // so don't serialize anything again
        return this->parentTreeItem->deleteNode(treeItem, true);
    }
    
    return false;
}

int AutomationTrackInsertAction::getSizeInUnits()
{
    return this->trackName.length();
}

SerializedData AutomationTrackInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationTrackInsertAction);
    tree.setProperty(Serialization::Undo::xPath, this->trackName);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->trackState.createCopy());
    return tree;
}

void AutomationTrackInsertAction::deserialize(const SerializedData &data)
{
    this->trackName = data.getProperty(Serialization::Undo::xPath);
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->trackState = data.getChild(0).createCopy();
}

void AutomationTrackInsertAction::reset()
{
    this->trackName.clear();
    this->trackId.clear();
    this->trackState = {};
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AutomationTrackRemoveAction::AutomationTrackRemoveAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem) noexcept :
    UndoAction(source), parentTreeItem(parentTreeItem) {}

AutomationTrackRemoveAction::AutomationTrackRemoveAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem, const String &trackId) noexcept :
    UndoAction(source),
    parentTreeItem(parentTreeItem),
    trackId(trackId) {}

bool AutomationTrackRemoveAction::perform()
{
    if (AutomationTrackNode *treeItem =
        this->source.findTrackById<AutomationTrackNode>(this->trackId))
    {
        this->numEvents = treeItem->getSequence()->size();
        this->serializedTreeItem = treeItem->serialize();
        this->trackName = treeItem->getTrackName();
        return this->parentTreeItem->deleteNode(treeItem, true);
    }
    
    return false;
}

bool AutomationTrackRemoveAction::undo()
{
    if (this->serializedTreeItem.isValid())
    {
        MidiTrackNode *track = new AutomationTrackNode("empty");
        track->deserialize(this->serializedTreeItem);
        this->parentTreeItem->addChildNode(track);
        track->setTrackName(this->trackName, false, sendNotification);
        return true;
    }
    
    return false;
}

int AutomationTrackRemoveAction::getSizeInUnits()
{
    if (this->serializedTreeItem.isValid())
    {
        return (this->numEvents * sizeof(MidiEvent));
    }
    
    return 1;
}

SerializedData AutomationTrackRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationTrackRemoveAction);
    tree.setProperty(Serialization::Undo::xPath, this->trackName);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->serializedTreeItem.createCopy());
    return tree;
}

void AutomationTrackRemoveAction::deserialize(const SerializedData &data)
{
    this->trackName = data.getProperty(Serialization::Undo::xPath);
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->serializedTreeItem = data.getChild(0).createCopy();
}

void AutomationTrackRemoveAction::reset()
{
    this->trackName.clear();
    this->trackId.clear();
    this->serializedTreeItem = {};
}

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
#include "AutomationTrackActions.h"
#include "MidiTrackSource.h"
#include "AutomationTrackNode.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AutomationTrackInsertAction::AutomationTrackInsertAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem) noexcept :
    UndoAction(source), parentTreeItem(parentTreeItem) {}

AutomationTrackInsertAction::AutomationTrackInsertAction(MidiTrackSource &source,
    WeakReference<TreeNode> parentTreeItem,
    ValueTree targetSerializedState,
    const String &xPath) noexcept :
    UndoAction(source),
    parentTreeItem(parentTreeItem),
    trackState(targetSerializedState),
    trackName(xPath) {}

bool AutomationTrackInsertAction::perform()
{
    MidiTrackNode *track = new AutomationTrackNode("empty");
    track->deserialize(this->trackState);
    this->parentTreeItem->addChildTreeItem(track);

    this->trackId = track->getTrackId();
    track->setTrackName(this->trackName, true);
    
    return true;
}

bool AutomationTrackInsertAction::undo()
{
    if (AutomationTrackNode *treeItem =
        this->source.findTrackById<AutomationTrackNode>(this->trackId))
    {
        // here the item state should be the same as when it was created
        // so don't serialize anything again
        return this->parentTreeItem->deleteItem(treeItem, true);
    }
    
    return false;
}

int AutomationTrackInsertAction::getSizeInUnits()
{
    return this->trackName.length();
}

ValueTree AutomationTrackInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::automationTrackInsertAction);
    tree.setProperty(Serialization::Undo::xPath, this->trackName, nullptr);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->trackState.createCopy(), nullptr);
    return tree;
}

void AutomationTrackInsertAction::deserialize(const ValueTree &tree)
{
    this->trackName = tree.getProperty(Serialization::Undo::xPath);
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->trackState = tree.getChild(0).createCopy();
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
    trackId(trackId),
    numEvents(0) {}

bool AutomationTrackRemoveAction::perform()
{
    if (AutomationTrackNode *treeItem =
        this->source.findTrackById<AutomationTrackNode>(this->trackId))
    {
        this->numEvents = treeItem->getSequence()->size();
        this->serializedTreeItem = treeItem->serialize();
        this->trackName = treeItem->getTrackName();
        return this->parentTreeItem->deleteItem(treeItem, true);
    }
    
    return false;
}

bool AutomationTrackRemoveAction::undo()
{
    if (this->serializedTreeItem.isValid())
    {
        MidiTrackNode *track = new AutomationTrackNode("empty");
        track->deserialize(this->serializedTreeItem);
        this->parentTreeItem->addChildTreeItem(track);
        track->setTrackName(this->trackName, true);
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

ValueTree AutomationTrackRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::automationTrackRemoveAction);
    tree.setProperty(Serialization::Undo::xPath, this->trackName, nullptr);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->serializedTreeItem.createCopy(), nullptr);
    return tree;
}

void AutomationTrackRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackName = tree.getProperty(Serialization::Undo::xPath);
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->serializedTreeItem = tree.getChild(0).createCopy();
}

void AutomationTrackRemoveAction::reset()
{
    this->trackName.clear();
    this->trackId.clear();
    this->serializedTreeItem = {};
}

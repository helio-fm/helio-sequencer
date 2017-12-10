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

#pragma once

class MidiTrackSource;

#include "AutomationSequence.h"
#include "UndoAction.h"
#include "TreeItem.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class AutomationTrackInsertAction : public UndoAction
{
public:

    AutomationTrackInsertAction(MidiTrackSource &source,
        WeakReference<TreeItem> parentTreeItem);
    
    AutomationTrackInsertAction(MidiTrackSource &source,
        WeakReference<TreeItem> parentTreeItem,
        String serializedState,
        String xPath);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    WeakReference<TreeItem> parentTreeItem;

    String trackId;
    
    String trackName;
    String serializedState;
    
    JUCE_DECLARE_NON_COPYABLE(AutomationTrackInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class AutomationTrackRemoveAction : public UndoAction
{
public:

    AutomationTrackRemoveAction(MidiTrackSource &source,
        WeakReference<TreeItem> parentTreeItem);
    
    AutomationTrackRemoveAction(MidiTrackSource &source,
        WeakReference<TreeItem> parentTreeItem,
        String trackId);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    WeakReference<TreeItem> parentTreeItem;

    String trackId;
    int numEvents;
    
    ScopedPointer<XmlElement> serializedTreeItem;
    String trackName;
    
    JUCE_DECLARE_NON_COPYABLE(AutomationTrackRemoveAction)
};

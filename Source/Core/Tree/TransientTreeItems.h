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

#include "TreeItem.h"

// Transient tree items are created and deleted on the fly.
// They don't really stand for something in terms of project's data,
// instead they help to reflect project's logical structure,
// and are used to access menus and pop-ups for such sub-entities, as:
// 
//  - piano roll current selection
//  - pattern roll selection
//  - vcs: current stage changes
//  - vcs: selected revision item in history
//  - plugins list: selected plugin
//  - instrument graph: selected node

class TransientTreeItem : public TreeItem
{
public:

    TransientTreeItem() : TreeItem({}, "TransientTreeItem") {}
    void showPage() override {}
    
    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    var getDragSourceDescription() override
    { return var::null; }

    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override
    { return false; }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    // Transient tree items will be serialized on save
    // (just because serializer saves everything)
    // but they won't be deserialized back on project load

    XmlElement *serialize() const override
    { return new XmlElement("TransientTreeItem"); }

    void deserialize(const XmlElement &xml) override {}

};

class PianoRollSelectionTreeItem : public TransientTreeItem
{
public:

    Image getIcon() const override;
    String getName() const override;
    ScopedPointer<Component> createItemMenu() override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollSelectionTreeItem)
};

class PatternRollSelectionTreeItem : public TransientTreeItem
{
public:

    Image getIcon() const override;
    String getName() const override;
    ScopedPointer<Component> createItemMenu() override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRollSelectionTreeItem)
};


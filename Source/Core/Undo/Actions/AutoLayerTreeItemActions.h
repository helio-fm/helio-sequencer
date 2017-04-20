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

class ProjectTreeItem;

#include "AutomationLayer.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class AutoLayerTreeItemInsertAction : public UndoAction
{
public:

    explicit AutoLayerTreeItemInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    AutoLayerTreeItemInsertAction(ProjectTreeItem &project,
                                  String serializedState,
                                  String xPath);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    String layerId;
    
    String xPath;
    String serializedState;
    
    JUCE_DECLARE_NON_COPYABLE(AutoLayerTreeItemInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class AutoLayerTreeItemRemoveAction : public UndoAction
{
public:

    explicit AutoLayerTreeItemRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    AutoLayerTreeItemRemoveAction(ProjectTreeItem &project,
                                   String layerId);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    String layerId;
    int numEvents;
    
    ScopedPointer<XmlElement> serializedTreeItem;
    String xPath;
    
    JUCE_DECLARE_NON_COPYABLE(AutoLayerTreeItemRemoveAction)
};

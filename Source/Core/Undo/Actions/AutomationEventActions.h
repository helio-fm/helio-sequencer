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

class AutomationSequence;
class ProjectTreeItem;

#include "AutomationEvent.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class AutomationEventInsertAction : public UndoAction
{
public:
    
    explicit AutomationEventInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AutomationEventInsertAction(ProjectTreeItem &project,
                                String trackId,
                                const AutomationEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    AutomationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class AutomationEventRemoveAction : public UndoAction
{
public:
    
    explicit AutomationEventRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AutomationEventRemoveAction(ProjectTreeItem &project,
                                String trackId,
                                const AutomationEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    AutomationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class AutomationEventChangeAction : public UndoAction
{
public:
    
    explicit AutomationEventChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AutomationEventChangeAction(ProjectTreeItem &project,
                                String trackId,
                                const AutomationEvent &target,
                                const AutomationEvent &newParameters);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;

    AutomationEvent eventBefore;
    AutomationEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventChangeAction)

};


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class AutomationEventsGroupInsertAction : public UndoAction
{
public:
    
    explicit AutomationEventsGroupInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    AutomationEventsGroupInsertAction(ProjectTreeItem &project,
                                      String trackId,
                                      Array<AutomationEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AutomationEvent> events;
    
    JUCE_DECLARE_NON_COPYABLE(AutomationEventsGroupInsertAction)
    
};


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class AutomationEventsGroupRemoveAction : public UndoAction
{
public:
    
    explicit AutomationEventsGroupRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    AutomationEventsGroupRemoveAction(ProjectTreeItem &project,
                                      String trackId,
                                      Array<AutomationEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AutomationEvent> events;
    
    JUCE_DECLARE_NON_COPYABLE(AutomationEventsGroupRemoveAction)
    
};


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class AutomationEventsGroupChangeAction : public UndoAction
{
public:
    
    explicit AutomationEventsGroupChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AutomationEventsGroupChangeAction(ProjectTreeItem &project,
                                      String trackId,
                                      const Array<AutomationEvent> state1,
                                      const Array<AutomationEvent> state2);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;

    Array<AutomationEvent> eventsBefore;
    Array<AutomationEvent> eventsAfter;

    JUCE_DECLARE_NON_COPYABLE(AutomationEventsGroupChangeAction)

};

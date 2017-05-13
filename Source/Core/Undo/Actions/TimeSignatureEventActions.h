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

class TimeSignaturesLayer;
class ProjectTreeItem;

#include "TimeSignatureEvent.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class TimeSignatureEventInsertAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    TimeSignatureEventInsertAction(ProjectTreeItem &project,
                                   String layerId,
                                   const TimeSignatureEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String layerId;
    TimeSignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class TimeSignatureEventRemoveAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    TimeSignatureEventRemoveAction(ProjectTreeItem &project,
                                   String layerId,
                                   const TimeSignatureEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String layerId;
    TimeSignatureEvent event;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class TimeSignatureEventChangeAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    TimeSignatureEventChangeAction(ProjectTreeItem &project,
                                   String layerId,
                                   const TimeSignatureEvent &target,
                                   const TimeSignatureEvent &newParameters);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String layerId;
    
    TimeSignatureEvent eventBefore;
    TimeSignatureEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventChangeAction)

};


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class TimeSignatureEventsGroupInsertAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventsGroupInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    TimeSignatureEventsGroupInsertAction(ProjectTreeItem &project,
                                         String layerId,
                                         Array<TimeSignatureEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String layerId;
    Array<TimeSignatureEvent> signatures;
    
    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventsGroupInsertAction)
    
};


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class TimeSignatureEventsGroupRemoveAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventsGroupRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    TimeSignatureEventsGroupRemoveAction(ProjectTreeItem &project,
                                         String layerId,
                                         Array<TimeSignatureEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String layerId;
    Array<TimeSignatureEvent> signatures;
    
    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventsGroupRemoveAction)
    
};


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class TimeSignatureEventsGroupChangeAction : public UndoAction
{
public:
    
    explicit TimeSignatureEventsGroupChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    TimeSignatureEventsGroupChangeAction(ProjectTreeItem &project,
                                         String layerId,
                                         const Array<TimeSignatureEvent> state1,
                                         const Array<TimeSignatureEvent> state2);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String layerId;
    
    Array<TimeSignatureEvent> eventsBefore;
    Array<TimeSignatureEvent> eventsAfter;

    JUCE_DECLARE_NON_COPYABLE(TimeSignatureEventsGroupChangeAction)

};


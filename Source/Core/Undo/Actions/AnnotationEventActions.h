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

class AnnotationsSequence;
class ProjectTreeItem;

#include "AnnotationEvent.h"
#include "UndoAction.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

class AnnotationEventInsertAction : public UndoAction
{
public:
    
    explicit AnnotationEventInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AnnotationEventInsertAction(ProjectTreeItem &project,
                                String trackId,
                                const AnnotationEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    AnnotationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventInsertAction)
};


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

class AnnotationEventRemoveAction : public UndoAction
{
public:
    
    explicit AnnotationEventRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AnnotationEventRemoveAction(ProjectTreeItem &project,
                                String trackId,
                                const AnnotationEvent &target);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    AnnotationEvent event;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventRemoveAction)
};


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

class AnnotationEventChangeAction : public UndoAction
{
public:
    
    explicit AnnotationEventChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AnnotationEventChangeAction(ProjectTreeItem &project,
                                String trackId,
                                const AnnotationEvent &target,
                                const AnnotationEvent &newParameters);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    
    AnnotationEvent eventBefore;
    AnnotationEvent eventAfter;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventChangeAction)

};


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

class AnnotationEventsGroupInsertAction : public UndoAction
{
public:
    
    explicit AnnotationEventsGroupInsertAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    AnnotationEventsGroupInsertAction(ProjectTreeItem &project,
                                      String trackId,
                                      Array<AnnotationEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AnnotationEvent> annotations;
    
    JUCE_DECLARE_NON_COPYABLE(AnnotationEventsGroupInsertAction)
    
};


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

class AnnotationEventsGroupRemoveAction : public UndoAction
{
public:
    
    explicit AnnotationEventsGroupRemoveAction(ProjectTreeItem &project) :
    UndoAction(project) {}
    
    AnnotationEventsGroupRemoveAction(ProjectTreeItem &project,
                                      String trackId,
                                      Array<AnnotationEvent> &target);
    
    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    String trackId;
    Array<AnnotationEvent> annotations;
    
    JUCE_DECLARE_NON_COPYABLE(AnnotationEventsGroupRemoveAction)
    
};


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

class AnnotationEventsGroupChangeAction : public UndoAction
{
public:
    
    explicit AnnotationEventsGroupChangeAction(ProjectTreeItem &project) :
    UndoAction(project) {}

    AnnotationEventsGroupChangeAction(ProjectTreeItem &project,
                                      String trackId,
                                      const Array<AnnotationEvent> state1,
                                      const Array<AnnotationEvent> state2);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override;
    UndoAction *createCoalescedAction(UndoAction *nextAction) override;
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    String trackId;
    
    Array<AnnotationEvent> eventsBefore;
    Array<AnnotationEvent> eventsAfter;

    JUCE_DECLARE_NON_COPYABLE(AnnotationEventsGroupChangeAction)

};


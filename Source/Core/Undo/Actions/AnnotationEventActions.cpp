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
#include "AnnotationEventActions.h"
#include "AnnotationsSequence.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AnnotationEventInsertAction::AnnotationEventInsertAction(MidiTrackSource &source,
    String targetTrackId, const AnnotationEvent &event) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    event(event) {}

bool AnnotationEventInsertAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool AnnotationEventInsertAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

int AnnotationEventInsertAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

ValueTree AnnotationEventInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::annotationEventInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.addChild(this->event.serialize());
    return tree;
}

void AnnotationEventInsertAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*tree.getFirstChildElement());
}

void AnnotationEventInsertAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AnnotationEventRemoveAction::AnnotationEventRemoveAction(MidiTrackSource &source,
    String targetTrackId, const AnnotationEvent &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    event(target) {}

bool AnnotationEventRemoveAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

bool AnnotationEventRemoveAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int AnnotationEventRemoveAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

ValueTree AnnotationEventRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::annotationEventRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.addChild(this->event.serialize());
    return tree;
}

void AnnotationEventRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*tree.getFirstChildElement());
}

void AnnotationEventRemoveAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

AnnotationEventChangeAction::AnnotationEventChangeAction(MidiTrackSource &source,
    String targetTrackId, const AnnotationEvent &target, const AnnotationEvent &newParameters) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    eventBefore(target),
    eventAfter(newParameters) {}

bool AnnotationEventChangeAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool AnnotationEventChangeAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int AnnotationEventChangeAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent) * 2;
}

UndoAction *AnnotationEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        if (AnnotationEventChangeAction *nextChanger =
            dynamic_cast<AnnotationEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual =
                (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                    this->trackId == nextChanger->trackId);
            
            if (idsAreEqual)
            {
                return new AnnotationEventChangeAction(this->source,
                    this->trackId, this->eventBefore, nextChanger->eventAfter);
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

ValueTree AnnotationEventChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::annotationEventChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    auto annotationBeforeChild = new XmlElement(Serialization::Undo::annotationBefore);
    annotationBeforeChild->prependChildElement(this->eventBefore.serialize());
    tree.addChild(annotationBeforeChild);
    
    auto annotationAfterChild = new XmlElement(Serialization::Undo::annotationAfter);
    annotationAfterChild->prependChildElement(this->eventAfter.serialize());
    tree.addChild(annotationAfterChild);
    
    return tree;
}

void AnnotationEventChangeAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *annotationBeforeChild = tree.getChildByName(Serialization::Undo::annotationBefore);
    XmlElement *annotationAfterChild = tree.getChildByName(Serialization::Undo::annotationAfter);
    
    this->eventBefore.deserialize(*annotationBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*annotationAfterChild->getFirstChildElement());
}

void AnnotationEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupInsertAction::AnnotationEventsGroupInsertAction(MidiTrackSource &source,
    String targetTrackId, Array<AnnotationEvent> &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
{
    this->annotations.swapWith(target);
}

bool AnnotationEventsGroupInsertAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->insertGroup(this->annotations, false);
    }
    
    return false;
}

bool AnnotationEventsGroupInsertAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->removeGroup(this->annotations, false);
    }
    
    return false;
}

int AnnotationEventsGroupInsertAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->annotations.size());
}

ValueTree AnnotationEventsGroupInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::annotationEventsGroupInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        tree.addChild(this->annotations.getUnchecked(i).serialize());
    }
    
    return tree;
}

void AnnotationEventsGroupInsertAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(tree, noteXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*noteXml);
        this->annotations.add(ae);
    }
}

void AnnotationEventsGroupInsertAction::reset()
{
    this->annotations.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupRemoveAction::AnnotationEventsGroupRemoveAction(MidiTrackSource &source,
    String targetTrackId, Array<AnnotationEvent> &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
{
    this->annotations.swapWith(target);
}

bool AnnotationEventsGroupRemoveAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->removeGroup(this->annotations, false);
    }
    
    return false;
}

bool AnnotationEventsGroupRemoveAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->insertGroup(this->annotations, false);
    }
    
    return false;
}

int AnnotationEventsGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->annotations.size());
}

ValueTree AnnotationEventsGroupRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::annotationEventsGroupRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        tree.addChild(this->annotations.getUnchecked(i).serialize());
    }
    
    return tree;
}

void AnnotationEventsGroupRemoveAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(tree, noteXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*noteXml);
        this->annotations.add(ae);
    }
}

void AnnotationEventsGroupRemoveAction::reset()
{
    this->annotations.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupChangeAction::AnnotationEventsGroupChangeAction(MidiTrackSource &source,
    String targetTrackId, const Array<AnnotationEvent> state1, const Array<AnnotationEvent> state2) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool AnnotationEventsGroupChangeAction::perform()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool AnnotationEventsGroupChangeAction::undo()
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsAfter, this->eventsBefore, false);
    }
    
    return false;
}

int AnnotationEventsGroupChangeAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->eventsBefore.size()) +
        (sizeof(AnnotationEvent) * this->eventsAfter.size());
}

UndoAction *AnnotationEventsGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AnnotationsSequence *sequence =
        this->source.findSequenceByTrackId<AnnotationsSequence>(this->trackId))
    {
        if (AnnotationEventsGroupChangeAction *nextChanger =
            dynamic_cast<AnnotationEventsGroupChangeAction *>(nextAction))
        {
            if (nextChanger->trackId != this->trackId)
            {
                return nullptr;
            }
            
            // simple checking the first and the last ones should be enough here
            bool arraysContainSameEvents =
                (this->eventsBefore.size() == nextChanger->eventsAfter.size()) &&
                (this->eventsBefore[0].getId() == nextChanger->eventsAfter[0].getId());
            
            if (arraysContainSameEvents)
            {
                return new AnnotationEventsGroupChangeAction(this->source,
                    this->trackId, this->eventsBefore, nextChanger->eventsAfter);
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree AnnotationEventsGroupChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::annotationEventsGroupChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    auto groupBeforeChild = new XmlElement(Serialization::Undo::groupBefore);
    auto groupAfterChild = new XmlElement(Serialization::Undo::groupAfter);
    
    for (int i = 0; i < this->eventsBefore.size(); ++i)
    {
        groupBeforeChild->prependChildElement(this->eventsBefore.getUnchecked(i).serialize());
    }
    
    for (int i = 0; i < this->eventsAfter.size(); ++i)
    {
        groupAfterChild->prependChildElement(this->eventsAfter.getUnchecked(i).serialize());
    }
    
    tree.addChild(groupBeforeChild);
    tree.addChild(groupAfterChild);
    
    return tree;
}

void AnnotationEventsGroupChangeAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *groupBeforeChild = tree.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = tree.getChildByName(Serialization::Undo::groupAfter);
    
    forEachXmlChildElement(*groupBeforeChild, eventXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*eventXml);
        this->eventsBefore.add(ae);
    }
    
    forEachXmlChildElement(*groupAfterChild, eventXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*eventXml);
        this->eventsAfter.add(ae);
    }
}

void AnnotationEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}

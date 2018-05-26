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
    const String &trackId, const AnnotationEvent &event) noexcept :
    UndoAction(source),
    trackId(trackId),
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->event.serialize(), nullptr);
    return tree;
}

void AnnotationEventInsertAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(tree.getChild(0));
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
    const String &trackId, const AnnotationEvent &target) noexcept :
    UndoAction(source),
    trackId(trackId),
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->event.serialize(), nullptr);
    return tree;
}

void AnnotationEventRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(tree.getChild(0));
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
    const String &trackId, const AnnotationEvent &target,
    const AnnotationEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    ValueTree annotationBeforeChild(Serialization::Undo::annotationBefore);
    annotationBeforeChild.appendChild(this->eventBefore.serialize(), nullptr);
    tree.appendChild(annotationBeforeChild, nullptr);
    
    ValueTree annotationAfterChild(Serialization::Undo::annotationAfter);
    annotationAfterChild.appendChild(this->eventAfter.serialize(), nullptr);
    tree.appendChild(annotationAfterChild, nullptr);
    
    return tree;
}

void AnnotationEventChangeAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    const auto annotationBeforeChild = tree.getChildWithName(Serialization::Undo::annotationBefore);
    const auto annotationAfterChild = tree.getChildWithName(Serialization::Undo::annotationAfter);
    
    this->eventBefore.deserialize(annotationBeforeChild.getChild(0));
    this->eventAfter.deserialize(annotationAfterChild.getChild(0));
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
    const String &trackId, Array<AnnotationEvent> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        tree.appendChild(this->annotations.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void AnnotationEventsGroupInsertAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    for (const auto &params : tree)
    {
        AnnotationEvent ae;
        ae.deserialize(params);
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
    const String &trackId, Array<AnnotationEvent> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        tree.appendChild(this->annotations.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void AnnotationEventsGroupRemoveAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    for (const auto &params : tree)
    {
        AnnotationEvent ae;
        ae.deserialize(params);
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
    const String &trackId, const Array<AnnotationEvent> state1,
    const Array<AnnotationEvent> state2) noexcept :
    UndoAction(source),
    trackId(trackId)
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    ValueTree groupBeforeChild(Serialization::Undo::groupBefore);
    ValueTree groupAfterChild(Serialization::Undo::groupAfter);
    
    for (int i = 0; i < this->eventsBefore.size(); ++i)
    {
        groupBeforeChild.appendChild(this->eventsBefore.getUnchecked(i).serialize(), nullptr);
    }
    
    for (int i = 0; i < this->eventsAfter.size(); ++i)
    {
        groupAfterChild.appendChild(this->eventsAfter.getUnchecked(i).serialize(), nullptr);
    }
    
    tree.appendChild(groupBeforeChild, nullptr);
    tree.appendChild(groupAfterChild, nullptr);
    
    return tree;
}

void AnnotationEventsGroupChangeAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    const auto groupBeforeChild = tree.getChildWithName(Serialization::Undo::groupBefore);
    const auto groupAfterChild = tree.getChildWithName(Serialization::Undo::groupAfter);
    
    for (const auto &params : groupBeforeChild)
    {
        AnnotationEvent ae;
        ae.deserialize(params);
        this->eventsBefore.add(ae);
    }
    
    for (const auto &params : groupAfterChild)
    {
        AnnotationEvent ae;
        ae.deserialize(params);
        this->eventsAfter.add(ae);
    }
}

void AnnotationEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}

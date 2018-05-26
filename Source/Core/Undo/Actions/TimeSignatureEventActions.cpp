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
#include "TimeSignatureEventActions.h"
#include "TimeSignaturesSequence.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

TimeSignatureEventInsertAction::TimeSignatureEventInsertAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &event) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(event) {}

bool TimeSignatureEventInsertAction::perform()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool TimeSignatureEventInsertAction::undo()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

int TimeSignatureEventInsertAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent);
}

ValueTree TimeSignatureEventInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::timeSignatureEventInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->event.serialize(), nullptr);
    return tree;
}

void TimeSignatureEventInsertAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(tree.getChild(0));
}

void TimeSignatureEventInsertAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

TimeSignatureEventRemoveAction::TimeSignatureEventRemoveAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &target) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(target) {}

bool TimeSignatureEventRemoveAction::perform()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

bool TimeSignatureEventRemoveAction::undo()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int TimeSignatureEventRemoveAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent);
}

ValueTree TimeSignatureEventRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::timeSignatureEventRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->event.serialize(), nullptr);
    return tree;
}

void TimeSignatureEventRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(tree.getChild(0));
}

void TimeSignatureEventRemoveAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

TimeSignatureEventChangeAction::TimeSignatureEventChangeAction(MidiTrackSource &source,
    const String &trackId, const TimeSignatureEvent &target,
    const TimeSignatureEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    eventBefore(target),
    eventAfter(newParameters) {}

bool TimeSignatureEventChangeAction::perform()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool TimeSignatureEventChangeAction::undo()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int TimeSignatureEventChangeAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent) * 2;
}

UndoAction *TimeSignatureEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        if (TimeSignatureEventChangeAction *nextChanger =
            dynamic_cast<TimeSignatureEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual = 
                (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                    this->trackId == nextChanger->trackId);
            
            if (idsAreEqual)
            {
                return new TimeSignatureEventChangeAction(this->source,
                    this->trackId, this->eventBefore, nextChanger->eventAfter);
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

ValueTree TimeSignatureEventChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::timeSignatureEventChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    ValueTree timeSignatureBeforeChild(Serialization::Undo::timeSignatureBefore);
    timeSignatureBeforeChild.appendChild(this->eventBefore.serialize(), nullptr);
    tree.appendChild(timeSignatureBeforeChild, nullptr);
    
    ValueTree timeSignatureAfterChild(Serialization::Undo::timeSignatureAfter);
    timeSignatureAfterChild.appendChild(this->eventAfter.serialize(), nullptr);
    tree.appendChild(timeSignatureAfterChild, nullptr);
    
    return tree;
}

void TimeSignatureEventChangeAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    const auto timeSignatureBeforeChild = tree.getChildWithName(Serialization::Undo::timeSignatureBefore);
    const auto timeSignatureAfterChild = tree.getChildWithName(Serialization::Undo::timeSignatureAfter);
    
    this->eventBefore.deserialize(timeSignatureBeforeChild.getChild(0));
    this->eventAfter.deserialize(timeSignatureAfterChild.getChild(0));
}

void TimeSignatureEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

TimeSignatureEventsGroupInsertAction::TimeSignatureEventsGroupInsertAction(MidiTrackSource &source,
    const String &trackId, Array<TimeSignatureEvent> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->signatures.swapWith(target);
}

bool TimeSignatureEventsGroupInsertAction::perform()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->insertGroup(this->signatures, false);
    }
    
    return false;
}

bool TimeSignatureEventsGroupInsertAction::undo()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->removeGroup(this->signatures, false);
    }
    
    return false;
}

int TimeSignatureEventsGroupInsertAction::getSizeInUnits()
{
    return (sizeof(TimeSignatureEvent) * this->signatures.size());
}

ValueTree TimeSignatureEventsGroupInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::timeSignatureEventsGroupInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        tree.appendChild(this->signatures.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void TimeSignatureEventsGroupInsertAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    for (const auto &params : tree)
    {
        TimeSignatureEvent ae;
        ae.deserialize(params);
        this->signatures.add(ae);
    }
}

void TimeSignatureEventsGroupInsertAction::reset()
{
    this->signatures.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

TimeSignatureEventsGroupRemoveAction::TimeSignatureEventsGroupRemoveAction(MidiTrackSource &source,
    const String &trackId, Array<TimeSignatureEvent> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->signatures.swapWith(target);
}

bool TimeSignatureEventsGroupRemoveAction::perform()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->removeGroup(this->signatures, false);
    }
    
    return false;
}

bool TimeSignatureEventsGroupRemoveAction::undo()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->insertGroup(this->signatures, false);
    }
    
    return false;
}

int TimeSignatureEventsGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(TimeSignatureEvent) * this->signatures.size());
}

ValueTree TimeSignatureEventsGroupRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::timeSignatureEventsGroupRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        tree.appendChild(this->signatures.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void TimeSignatureEventsGroupRemoveAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    for (const auto &params : tree)
    {
        TimeSignatureEvent ae;
        ae.deserialize(params);
        this->signatures.add(ae);
    }
}

void TimeSignatureEventsGroupRemoveAction::reset()
{
    this->signatures.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

TimeSignatureEventsGroupChangeAction::TimeSignatureEventsGroupChangeAction(MidiTrackSource &source,
    const String &trackId, const Array<TimeSignatureEvent> state1,
    const Array<TimeSignatureEvent> state2) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool TimeSignatureEventsGroupChangeAction::perform()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool TimeSignatureEventsGroupChangeAction::undo()
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsAfter, this->eventsBefore, false);
    }
    
    return false;
}

int TimeSignatureEventsGroupChangeAction::getSizeInUnits()
{
    return (sizeof(TimeSignatureEvent) * this->eventsBefore.size()) +
           (sizeof(TimeSignatureEvent) * this->eventsAfter.size());
}

UndoAction *TimeSignatureEventsGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (TimeSignaturesSequence *sequence =
        this->source.findSequenceByTrackId<TimeSignaturesSequence>(this->trackId))
    {
        if (TimeSignatureEventsGroupChangeAction *nextChanger =
            dynamic_cast<TimeSignatureEventsGroupChangeAction *>(nextAction))
        {
            if (nextChanger->trackId != this->trackId)
            {
                return nullptr;
            }
            
            bool arraysContainSameEvents =
                (this->eventsBefore.size() == nextChanger->eventsAfter.size()) &&
                (this->eventsBefore[0].getId() == nextChanger->eventsAfter[0].getId());
            
            if (arraysContainSameEvents)
            {
                return new TimeSignatureEventsGroupChangeAction(this->source,
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

ValueTree TimeSignatureEventsGroupChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::timeSignatureEventsGroupChangeAction);
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

void TimeSignatureEventsGroupChangeAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    const auto groupBeforeChild = tree.getChildWithName(Serialization::Undo::groupBefore);
    const auto groupAfterChild = tree.getChildWithName(Serialization::Undo::groupAfter);
    
    for (const auto &params : groupBeforeChild)
    {
        TimeSignatureEvent ae;
        ae.deserialize(params);
        this->eventsBefore.add(ae);
    }
    
    for (const auto &params : groupAfterChild)
    {
        TimeSignatureEvent ae;
        ae.deserialize(params);
        this->eventsAfter.add(ae);
    }
}

void TimeSignatureEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}

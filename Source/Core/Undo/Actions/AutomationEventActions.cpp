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
#include "AutomationEventActions.h"
#include "AutomationSequence.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AutomationEventInsertAction::AutomationEventInsertAction(MidiTrackSource &source,
    const String &trackId, const AutomationEvent &event) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(event) {}

bool AutomationEventInsertAction::perform()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool AutomationEventInsertAction::undo()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

int AutomationEventInsertAction::getSizeInUnits()
{
    return sizeof(AutomationEvent);
}

SerializedData AutomationEventInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationEventInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void AutomationEventInsertAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void AutomationEventInsertAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AutomationEventRemoveAction::AutomationEventRemoveAction(MidiTrackSource &source,
    const String &trackId, const AutomationEvent &target) noexcept :
    UndoAction(source),
    trackId(trackId),
    event(target) {}

bool AutomationEventRemoveAction::perform()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

bool AutomationEventRemoveAction::undo()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int AutomationEventRemoveAction::getSizeInUnits()
{
    return sizeof(AutomationEvent);
}

SerializedData AutomationEventRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationEventRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->event.serialize());
    return tree;
}

void AutomationEventRemoveAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->event.deserialize(data.getChild(0));
}

void AutomationEventRemoveAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

AutomationEventChangeAction::AutomationEventChangeAction(MidiTrackSource &source,
    const String &trackId, const AutomationEvent &target,
    const AutomationEvent &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    eventBefore(target),
    eventAfter(newParameters) {}

bool AutomationEventChangeAction::perform()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool AutomationEventChangeAction::undo()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int AutomationEventChangeAction::getSizeInUnits()
{
    return sizeof(AutomationEvent) * 2;
}

UndoAction *AutomationEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        if (AutomationEventChangeAction *nextChanger =
            dynamic_cast<AutomationEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual = 
                (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                    this->trackId == nextChanger->trackId);
            
            if (idsAreEqual)
            {
                return new AutomationEventChangeAction(this->source,
                    this->trackId, this->eventBefore, nextChanger->eventAfter);
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

SerializedData AutomationEventChangeAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationEventChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    SerializedData eventBeforeChild(Serialization::Undo::eventBefore);
    eventBeforeChild.appendChild(this->eventBefore.serialize());
    tree.appendChild(eventBeforeChild);
    
    SerializedData eventAfterChild(Serialization::Undo::eventAfter);
    eventAfterChild.appendChild(this->eventAfter.serialize());
    tree.appendChild(eventAfterChild);
    
    return tree;
}

void AutomationEventChangeAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    const auto eventBeforeChild = data.getChildWithName(Serialization::Undo::eventBefore);
    const auto eventAfterChild = data.getChildWithName(Serialization::Undo::eventAfter);
    
    this->eventBefore.deserialize(eventBeforeChild.getChild(0));
    this->eventAfter.deserialize(eventAfterChild.getChild(0));
}

void AutomationEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

AutomationEventsGroupInsertAction::AutomationEventsGroupInsertAction(MidiTrackSource &source,
    const String &trackId, Array<AutomationEvent> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->events.swapWith(target);
}

bool AutomationEventsGroupInsertAction::perform()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->insertGroup(this->events, false);
    }
    
    return false;
}

bool AutomationEventsGroupInsertAction::undo()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->removeGroup(this->events, false);
    }
    
    return false;
}

int AutomationEventsGroupInsertAction::getSizeInUnits()
{
    return (sizeof(AutomationEvent) * this->events.size());
}

SerializedData AutomationEventsGroupInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationEventsGroupInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->events.size(); ++i)
    {
        tree.appendChild(this->events.getUnchecked(i).serialize());
    }
    
    return tree;
}

void AutomationEventsGroupInsertAction::deserialize(const SerializedData &data)
{
    this->reset();
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    for (const auto &params : data)
    {
        AutomationEvent ae;
        ae.deserialize(params);
        this->events.add(ae);
    }
}

void AutomationEventsGroupInsertAction::reset()
{
    this->events.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

AutomationEventsGroupRemoveAction::AutomationEventsGroupRemoveAction(MidiTrackSource &source,
    const String &trackId, Array<AutomationEvent> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->events.swapWith(target);
}

bool AutomationEventsGroupRemoveAction::perform()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->removeGroup(this->events, false);
    }
    
    return false;
}

bool AutomationEventsGroupRemoveAction::undo()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->insertGroup(this->events, false);
    }
    
    return false;
}

int AutomationEventsGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(AutomationEvent) * this->events.size());
}

SerializedData AutomationEventsGroupRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationEventsGroupRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->events.size(); ++i)
    {
        tree.appendChild(this->events.getUnchecked(i).serialize());
    }
    
    return tree;
}

void AutomationEventsGroupRemoveAction::deserialize(const SerializedData &data)
{
    this->reset();
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    for (const auto &params : data)
    {
        AutomationEvent ae;
        ae.deserialize(params);
        this->events.add(ae);
    }
}

void AutomationEventsGroupRemoveAction::reset()
{
    this->events.clear();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

AutomationEventsGroupChangeAction::AutomationEventsGroupChangeAction(MidiTrackSource &source,
    const String &trackId, const Array<AutomationEvent> state1,
    const Array<AutomationEvent> state2) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool AutomationEventsGroupChangeAction::perform()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool AutomationEventsGroupChangeAction::undo()
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsAfter, this->eventsBefore, false);
    }
    
    return false;
}

int AutomationEventsGroupChangeAction::getSizeInUnits()
{
    return (sizeof(AutomationEvent) * this->eventsBefore.size()) +
           (sizeof(AutomationEvent) * this->eventsAfter.size());
}

UndoAction *AutomationEventsGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AutomationSequence *sequence =
        this->source.findSequenceByTrackId<AutomationSequence>(this->trackId))
    {
        if (AutomationEventsGroupChangeAction *nextChanger =
            dynamic_cast<AutomationEventsGroupChangeAction *>(nextAction))
        {
            if (nextChanger->trackId != this->trackId)
            {
                return nullptr;
            }
            
            // это явно неполная проверка, но ее будет достаточно
            bool arraysContainSameNotes =
                (this->eventsBefore.size() == nextChanger->eventsAfter.size()) &&
                (this->eventsBefore[0].getId() == nextChanger->eventsAfter[0].getId());
            
            if (arraysContainSameNotes)
            {
                return new AutomationEventsGroupChangeAction(this->source,
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

SerializedData AutomationEventsGroupChangeAction::serialize() const
{
    SerializedData tree(Serialization::Undo::automationEventsGroupChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    SerializedData groupBeforeChild(Serialization::Undo::groupBefore);
    SerializedData groupAfterChild(Serialization::Undo::groupAfter);
    
    for (int i = 0; i < this->eventsBefore.size(); ++i)
    {
        groupBeforeChild.appendChild(this->eventsBefore.getUnchecked(i).serialize());
    }
    
    for (int i = 0; i < this->eventsAfter.size(); ++i)
    {
        groupAfterChild.appendChild(this->eventsAfter.getUnchecked(i).serialize());
    }
    
    tree.appendChild(groupBeforeChild);
    tree.appendChild(groupAfterChild);
    
    return tree;
}

void AutomationEventsGroupChangeAction::deserialize(const SerializedData &data)
{
    this->reset();
    
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    const auto groupBeforeChild = data.getChildWithName(Serialization::Undo::groupBefore);
    const auto groupAfterChild = data.getChildWithName(Serialization::Undo::groupAfter);
    
    for (const auto &params : groupBeforeChild)
    {
        AutomationEvent ae;
        ae.deserialize(params);
        this->eventsBefore.add(ae);
    }
    
    for (const auto &params : groupAfterChild)
    {
        AutomationEvent ae;
        ae.deserialize(params);
        this->eventsAfter.add(ae);
    }
}

void AutomationEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}

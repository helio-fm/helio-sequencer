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
    String targetTrackId, const TimeSignatureEvent &event) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.addChild(this->event.serialize());
    return tree;
}

void TimeSignatureEventInsertAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*tree.getFirstChildElement());
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
    String targetTrackId, const TimeSignatureEvent &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.addChild(this->event.serialize());
    return tree;
}

void TimeSignatureEventRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*tree.getFirstChildElement());
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
    String targetTrackId, const TimeSignatureEvent &target, const TimeSignatureEvent &newParameters) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    auto timeSignatureBeforeChild = new XmlElement(Serialization::Undo::timeSignatureBefore);
    timeSignatureBeforeChild->prependChildElement(this->eventBefore.serialize());
    tree.addChild(timeSignatureBeforeChild);
    
    auto timeSignatureAfterChild = new XmlElement(Serialization::Undo::timeSignatureAfter);
    timeSignatureAfterChild->prependChildElement(this->eventAfter.serialize());
    tree.addChild(timeSignatureAfterChild);
    
    return tree;
}

void TimeSignatureEventChangeAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *timeSignatureBeforeChild = tree.getChildByName(Serialization::Undo::timeSignatureBefore);
    XmlElement *timeSignatureAfterChild = tree.getChildByName(Serialization::Undo::timeSignatureAfter);
    
    this->eventBefore.deserialize(*timeSignatureBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*timeSignatureAfterChild->getFirstChildElement());
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
    String targetTrackId, Array<TimeSignatureEvent> &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        tree.addChild(this->signatures.getUnchecked(i).serialize());
    }
    
    return tree;
}

void TimeSignatureEventsGroupInsertAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(tree, noteXml)
    {
        TimeSignatureEvent ae;
        ae.deserialize(*noteXml);
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
    String targetTrackId, Array<TimeSignatureEvent> &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
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
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        tree.addChild(this->signatures.getUnchecked(i).serialize());
    }
    
    return tree;
}

void TimeSignatureEventsGroupRemoveAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(tree, noteXml)
    {
        TimeSignatureEvent ae;
        ae.deserialize(*noteXml);
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
    String targetTrackId, const Array<TimeSignatureEvent> state1, const Array<TimeSignatureEvent> state2) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
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

void TimeSignatureEventsGroupChangeAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *groupBeforeChild = tree.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = tree.getChildByName(Serialization::Undo::groupAfter);
    
    forEachXmlChildElement(*groupBeforeChild, eventXml)
    {
        TimeSignatureEvent ae;
        ae.deserialize(*eventXml);
        this->eventsBefore.add(ae);
    }
    
    forEachXmlChildElement(*groupAfterChild, eventXml)
    {
        TimeSignatureEvent ae;
        ae.deserialize(*eventXml);
        this->eventsAfter.add(ae);
    }
}

void TimeSignatureEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}

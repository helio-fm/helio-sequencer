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

XmlElement *TimeSignatureEventInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::timeSignatureEventInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void TimeSignatureEventInsertAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
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

XmlElement *TimeSignatureEventRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::timeSignatureEventRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void TimeSignatureEventRemoveAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
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

XmlElement *TimeSignatureEventChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::timeSignatureEventChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    auto timeSignatureBeforeChild = new XmlElement(Serialization::Undo::timeSignatureBefore);
    timeSignatureBeforeChild->prependChildElement(this->eventBefore.serialize());
    xml->prependChildElement(timeSignatureBeforeChild);
    
    auto timeSignatureAfterChild = new XmlElement(Serialization::Undo::timeSignatureAfter);
    timeSignatureAfterChild->prependChildElement(this->eventAfter.serialize());
    xml->prependChildElement(timeSignatureAfterChild);
    
    return xml;
}

void TimeSignatureEventChangeAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *timeSignatureBeforeChild = xml.getChildByName(Serialization::Undo::timeSignatureBefore);
    XmlElement *timeSignatureAfterChild = xml.getChildByName(Serialization::Undo::timeSignatureAfter);
    
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

XmlElement *TimeSignatureEventsGroupInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::timeSignatureEventsGroupInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        xml->prependChildElement(this->signatures.getUnchecked(i).serialize());
    }
    
    return xml;
}

void TimeSignatureEventsGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
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

XmlElement *TimeSignatureEventsGroupRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::timeSignatureEventsGroupRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        xml->prependChildElement(this->signatures.getUnchecked(i).serialize());
    }
    
    return xml;
}

void TimeSignatureEventsGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
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

XmlElement *TimeSignatureEventsGroupChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::timeSignatureEventsGroupChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
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
    
    xml->prependChildElement(groupBeforeChild);
    xml->prependChildElement(groupAfterChild);
    
    return xml;
}

void TimeSignatureEventsGroupChangeAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *groupBeforeChild = xml.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = xml.getChildByName(Serialization::Undo::groupAfter);
    
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

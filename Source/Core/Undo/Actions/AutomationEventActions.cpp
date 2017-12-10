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
    String targetTrackId, const AutomationEvent &event) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
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

XmlElement *AutomationEventInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AutomationEventInsertAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
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
    String targetTrackId, const AutomationEvent &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
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

XmlElement *AutomationEventRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AutomationEventRemoveAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
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
    String targetTrackId, const AutomationEvent &target, const AutomationEvent &newParameters) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
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

XmlElement *AutomationEventChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    auto eventBeforeChild = new XmlElement(Serialization::Undo::eventBefore);
    eventBeforeChild->prependChildElement(this->eventBefore.serialize());
    xml->prependChildElement(eventBeforeChild);
    
    auto eventAfterChild = new XmlElement(Serialization::Undo::eventAfter);
    eventAfterChild->prependChildElement(this->eventAfter.serialize());
    xml->prependChildElement(eventAfterChild);
    
    return xml;
}

void AutomationEventChangeAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *eventBeforeChild = xml.getChildByName(Serialization::Undo::eventBefore);
    XmlElement *eventAfterChild = xml.getChildByName(Serialization::Undo::eventAfter);
    
    this->eventBefore.deserialize(*eventBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*eventAfterChild->getFirstChildElement());
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
    String targetLayerId, Array<AutomationEvent> &target) :
    UndoAction(source),
    trackId(std::move(targetLayerId))
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

XmlElement *AutomationEventsGroupInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventsGroupInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->events.size(); ++i)
    {
        xml->prependChildElement(this->events.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AutomationEventsGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        AutomationEvent ae;
        ae.deserialize(*noteXml);
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
    String targetTrackId, Array<AutomationEvent> &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
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

XmlElement *AutomationEventsGroupRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventsGroupRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->events.size(); ++i)
    {
        xml->prependChildElement(this->events.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AutomationEventsGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        AutomationEvent ae;
        ae.deserialize(*noteXml);
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
    String targetTrackId, const Array<AutomationEvent> state1, const Array<AutomationEvent> state2) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
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

XmlElement *AutomationEventsGroupChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventsGroupChangeAction);
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

void AutomationEventsGroupChangeAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *groupBeforeChild = xml.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = xml.getChildByName(Serialization::Undo::groupAfter);
    
    forEachXmlChildElement(*groupBeforeChild, eventXml)
    {
        AutomationEvent ae;
        ae.deserialize(*eventXml);
        this->eventsBefore.add(ae);
    }
    
    forEachXmlChildElement(*groupAfterChild, eventXml)
    {
        AutomationEvent ae;
        ae.deserialize(*eventXml);
        this->eventsAfter.add(ae);
    }
}

void AutomationEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}

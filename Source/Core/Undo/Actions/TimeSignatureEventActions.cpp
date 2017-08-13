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
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

TimeSignatureEventInsertAction::TimeSignatureEventInsertAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const TimeSignatureEvent &event) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    event(event)
{

}

bool TimeSignatureEventInsertAction::perform()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return (layer->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool TimeSignatureEventInsertAction::undo()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->remove(this->event, false);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void TimeSignatureEventInsertAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void TimeSignatureEventInsertAction::reset()
{
    this->event.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

TimeSignatureEventRemoveAction::TimeSignatureEventRemoveAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const TimeSignatureEvent &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    event(target)
{

}

bool TimeSignatureEventRemoveAction::perform()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->remove(this->event, false);
    }
    
    return false;
}

bool TimeSignatureEventRemoveAction::undo()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return (layer->insert(this->event, false) != nullptr);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void TimeSignatureEventRemoveAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void TimeSignatureEventRemoveAction::reset()
{
    this->event.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

TimeSignatureEventChangeAction::TimeSignatureEventChangeAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const TimeSignatureEvent &target,
                                                         const TimeSignatureEvent &newParameters) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    eventBefore(target),
    eventAfter(newParameters)
{

}

bool TimeSignatureEventChangeAction::perform()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool TimeSignatureEventChangeAction::undo()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int TimeSignatureEventChangeAction::getSizeInUnits()
{
    return sizeof(TimeSignatureEvent) * 2;
}

UndoAction *TimeSignatureEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        if (TimeSignatureEventChangeAction *nextChanger = dynamic_cast<TimeSignatureEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual = (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                                      this->layerId == nextChanger->layerId);
            
            if (idsAreEqual)
            {
                auto newChanger =
                new TimeSignatureEventChangeAction(this->project, this->layerId, this->eventBefore, nextChanger->eventAfter);
                
                return newChanger;
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

XmlElement *TimeSignatureEventChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::timeSignatureEventChangeAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
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
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    XmlElement *timeSignatureBeforeChild = xml.getChildByName(Serialization::Undo::timeSignatureBefore);
    XmlElement *timeSignatureAfterChild = xml.getChildByName(Serialization::Undo::timeSignatureAfter);
    
    this->eventBefore.deserialize(*timeSignatureBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*timeSignatureAfterChild->getFirstChildElement());
}

void TimeSignatureEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

TimeSignatureEventsGroupInsertAction::TimeSignatureEventsGroupInsertAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     Array<TimeSignatureEvent> &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->signatures.swapWith(target);
}

bool TimeSignatureEventsGroupInsertAction::perform()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->insertGroup(this->signatures, false);
    }
    
    return false;
}

bool TimeSignatureEventsGroupInsertAction::undo()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->removeGroup(this->signatures, false);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        xml->prependChildElement(this->signatures.getUnchecked(i).serialize());
    }
    
    return xml;
}

void TimeSignatureEventsGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
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
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

TimeSignatureEventsGroupRemoveAction::TimeSignatureEventsGroupRemoveAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     Array<TimeSignatureEvent> &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->signatures.swapWith(target);
}

bool TimeSignatureEventsGroupRemoveAction::perform()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->removeGroup(this->signatures, false);
    }
    
    return false;
}

bool TimeSignatureEventsGroupRemoveAction::undo()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->insertGroup(this->signatures, false);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        xml->prependChildElement(this->signatures.getUnchecked(i).serialize());
    }
    
    return xml;
}

void TimeSignatureEventsGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
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
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

TimeSignatureEventsGroupChangeAction::TimeSignatureEventsGroupChangeAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     const Array<TimeSignatureEvent> state1,
                                                                     const Array<TimeSignatureEvent> state2) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool TimeSignatureEventsGroupChangeAction::perform()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool TimeSignatureEventsGroupChangeAction::undo()
{
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        return layer->changeGroup(this->eventsAfter, this->eventsBefore, false);
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
    if (TimeSignaturesSequence *layer = this->project.getLayerWithId<TimeSignaturesSequence>(this->layerId))
    {
        if (TimeSignatureEventsGroupChangeAction *nextChanger = dynamic_cast<TimeSignatureEventsGroupChangeAction *>(nextAction))
        {
            if (nextChanger->layerId != this->layerId)
            {
                return nullptr;
            }
            
            // это явно неполная проверка, но ее будет достаточно
            bool arraysContainSameNotes = (this->eventsBefore.size() == nextChanger->eventsAfter.size()) &&
                                          (this->eventsBefore[0].getId() == nextChanger->eventsAfter[0].getId());
            
            if (arraysContainSameNotes)
            {
                TimeSignatureEventsGroupChangeAction *newChanger =
                new TimeSignatureEventsGroupChangeAction(this->project, this->layerId, this->eventsBefore, nextChanger->eventsAfter);
                
                return newChanger;
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
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
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
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
    this->layerId.clear();
}

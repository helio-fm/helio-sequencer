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
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AutomationEventInsertAction::AutomationEventInsertAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const AutomationEvent &event) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    event(event)
{

}

bool AutomationEventInsertAction::perform()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return (layer->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool AutomationEventInsertAction::undo()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->remove(this->event, false);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AutomationEventInsertAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void AutomationEventInsertAction::reset()
{
    this->event.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AutomationEventRemoveAction::AutomationEventRemoveAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const AutomationEvent &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    event(target)
{

}

bool AutomationEventRemoveAction::perform()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->remove(this->event, false);
    }
    
    return false;
}

bool AutomationEventRemoveAction::undo()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return (layer->insert(this->event, false) != nullptr);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AutomationEventRemoveAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void AutomationEventRemoveAction::reset()
{
    this->event.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

AutomationEventChangeAction::AutomationEventChangeAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const AutomationEvent &target,
                                                         const AutomationEvent &newParameters) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    eventBefore(target),
    eventAfter(newParameters)
{

}

bool AutomationEventChangeAction::perform()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool AutomationEventChangeAction::undo()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int AutomationEventChangeAction::getSizeInUnits()
{
    return sizeof(AutomationEvent) * 2;
}

UndoAction *AutomationEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        if (AutomationEventChangeAction *nextChanger = dynamic_cast<AutomationEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual = (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                                      this->layerId == nextChanger->layerId);
            
            if (idsAreEqual)
            {
                auto newChanger =
                new AutomationEventChangeAction(this->project, this->layerId, this->eventBefore, nextChanger->eventAfter);
                
                return newChanger;
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

XmlElement *AutomationEventChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventChangeAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
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
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    XmlElement *eventBeforeChild = xml.getChildByName(Serialization::Undo::eventBefore);
    XmlElement *eventAfterChild = xml.getChildByName(Serialization::Undo::eventAfter);
    
    this->eventBefore.deserialize(*eventBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*eventAfterChild->getFirstChildElement());
}

void AutomationEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

AutomationEventsGroupInsertAction::AutomationEventsGroupInsertAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     Array<AutomationEvent> &target) :
UndoAction(parentProject),
layerId(std::move(targetLayerId))
{
    this->events.swapWith(target);
}

bool AutomationEventsGroupInsertAction::perform()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->insertGroup(this->events, false);
    }
    
    return false;
}

bool AutomationEventsGroupInsertAction::undo()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->removeGroup(this->events, false);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->events.size(); ++i)
    {
        xml->prependChildElement(this->events.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AutomationEventsGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
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
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

AutomationEventsGroupRemoveAction::AutomationEventsGroupRemoveAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     Array<AutomationEvent> &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->events.swapWith(target);
}

bool AutomationEventsGroupRemoveAction::perform()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->removeGroup(this->events, false);
    }
    
    return false;
}

bool AutomationEventsGroupRemoveAction::undo()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->insertGroup(this->events, false);
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
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->events.size(); ++i)
    {
        xml->prependChildElement(this->events.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AutomationEventsGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
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
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

AutomationEventsGroupChangeAction::AutomationEventsGroupChangeAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     const Array<AutomationEvent> state1,
                                                                     const Array<AutomationEvent> state2) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool AutomationEventsGroupChangeAction::perform()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool AutomationEventsGroupChangeAction::undo()
{
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        return layer->changeGroup(this->eventsAfter, this->eventsBefore, false);
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
    if (AutomationSequence *layer = this->project.getLayerWithId<AutomationSequence>(this->layerId))
    {
        if (AutomationEventsGroupChangeAction *nextChanger = dynamic_cast<AutomationEventsGroupChangeAction *>(nextAction))
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
                AutomationEventsGroupChangeAction *newChanger =
                new AutomationEventsGroupChangeAction(this->project, this->layerId, this->eventsBefore, nextChanger->eventsAfter);
                
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

XmlElement *AutomationEventsGroupChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::automationEventsGroupChangeAction);
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

void AutomationEventsGroupChangeAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
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
    this->layerId.clear();
}

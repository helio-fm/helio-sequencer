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
#include "AnnotationsLayer.h"
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

AnnotationEventInsertAction::AnnotationEventInsertAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const AnnotationEvent &event) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    event(event)
{

}

bool AnnotationEventInsertAction::perform()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return (layer->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool AnnotationEventInsertAction::undo()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->remove(this->event, false);
    }
    
    return false;
}

int AnnotationEventInsertAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

XmlElement *AnnotationEventInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventInsertAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AnnotationEventInsertAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void AnnotationEventInsertAction::reset()
{
    this->event.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

AnnotationEventRemoveAction::AnnotationEventRemoveAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const AnnotationEvent &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    event(target)
{

}

bool AnnotationEventRemoveAction::perform()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->remove(this->event, false);
    }
    
    return false;
}

bool AnnotationEventRemoveAction::undo()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return (layer->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int AnnotationEventRemoveAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent);
}

XmlElement *AnnotationEventRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventRemoveAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void AnnotationEventRemoveAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void AnnotationEventRemoveAction::reset()
{
    this->event.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

AnnotationEventChangeAction::AnnotationEventChangeAction(ProjectTreeItem &parentProject,
                                                         String targetLayerId,
                                                         const AnnotationEvent &target,
                                                         const AnnotationEvent &newParameters) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    eventBefore(target),
    eventAfter(newParameters)
{

}

bool AnnotationEventChangeAction::perform()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool AnnotationEventChangeAction::undo()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int AnnotationEventChangeAction::getSizeInUnits()
{
    return sizeof(AnnotationEvent) * 2;
}

UndoAction *AnnotationEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        if (AnnotationEventChangeAction *nextChanger = dynamic_cast<AnnotationEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual = (this->eventBefore.getID() == nextChanger->eventAfter.getID() &&
                                      this->layerId == nextChanger->layerId);
            
            if (idsAreEqual)
            {
                auto newChanger =
                new AnnotationEventChangeAction(this->project, this->layerId, this->eventBefore, nextChanger->eventAfter);
                
                return newChanger;
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

XmlElement *AnnotationEventChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventChangeAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    auto annotationBeforeChild = new XmlElement(Serialization::Undo::annotationBefore);
    annotationBeforeChild->prependChildElement(this->eventBefore.serialize());
    xml->prependChildElement(annotationBeforeChild);
    
    auto annotationAfterChild = new XmlElement(Serialization::Undo::annotationAfter);
    annotationAfterChild->prependChildElement(this->eventAfter.serialize());
    xml->prependChildElement(annotationAfterChild);
    
    return xml;
}

void AnnotationEventChangeAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    XmlElement *annotationBeforeChild = xml.getChildByName(Serialization::Undo::annotationBefore);
    XmlElement *annotationAfterChild = xml.getChildByName(Serialization::Undo::annotationAfter);
    
    this->eventBefore.deserialize(*annotationBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*annotationAfterChild->getFirstChildElement());
}

void AnnotationEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupInsertAction::AnnotationEventsGroupInsertAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     Array<AnnotationEvent> &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->annotations.swapWith(target);
}

bool AnnotationEventsGroupInsertAction::perform()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->insertGroup(this->annotations, false);
    }
    
    return false;
}

bool AnnotationEventsGroupInsertAction::undo()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->removeGroup(this->annotations, false);
    }
    
    return false;
}

int AnnotationEventsGroupInsertAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->annotations.size());
}

XmlElement *AnnotationEventsGroupInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventsGroupInsertAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        xml->prependChildElement(this->annotations.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AnnotationEventsGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*noteXml);
        this->annotations.add(ae);
    }
}

void AnnotationEventsGroupInsertAction::reset()
{
    this->annotations.clear();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupRemoveAction::AnnotationEventsGroupRemoveAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     Array<AnnotationEvent> &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->annotations.swapWith(target);
}

bool AnnotationEventsGroupRemoveAction::perform()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->removeGroup(this->annotations, false);
    }
    
    return false;
}

bool AnnotationEventsGroupRemoveAction::undo()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->insertGroup(this->annotations, false);
    }
    
    return false;
}

int AnnotationEventsGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(AnnotationEvent) * this->annotations.size());
}

XmlElement *AnnotationEventsGroupRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventsGroupRemoveAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->annotations.size(); ++i)
    {
        xml->prependChildElement(this->annotations.getUnchecked(i).serialize());
    }
    
    return xml;
}

void AnnotationEventsGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        AnnotationEvent ae;
        ae.deserialize(*noteXml);
        this->annotations.add(ae);
    }
}

void AnnotationEventsGroupRemoveAction::reset()
{
    this->annotations.clear();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

AnnotationEventsGroupChangeAction::AnnotationEventsGroupChangeAction(ProjectTreeItem &parentProject,
                                                                     String targetLayerId,
                                                                     const Array<AnnotationEvent> state1,
                                                                     const Array<AnnotationEvent> state2) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool AnnotationEventsGroupChangeAction::perform()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool AnnotationEventsGroupChangeAction::undo()
{
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        return layer->changeGroup(this->eventsAfter, this->eventsBefore, false);
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
    if (AnnotationsLayer *layer = this->project.getLayerWithId<AnnotationsLayer>(this->layerId))
    {
        if (AnnotationEventsGroupChangeAction *nextChanger = dynamic_cast<AnnotationEventsGroupChangeAction *>(nextAction))
        {
            if (nextChanger->layerId != this->layerId)
            {
                return nullptr;
            }
            
            // это явно неполная проверка, но ее будет достаточно
            bool arraysContainSameNotes = (this->eventsBefore.size() == nextChanger->eventsAfter.size()) &&
                                          (this->eventsBefore[0].getID() == nextChanger->eventsAfter[0].getID());
            
            if (arraysContainSameNotes)
            {
                AnnotationEventsGroupChangeAction *newChanger =
                new AnnotationEventsGroupChangeAction(this->project, this->layerId, this->eventsBefore, nextChanger->eventsAfter);
                
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

XmlElement *AnnotationEventsGroupChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::annotationEventsGroupChangeAction);
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

void AnnotationEventsGroupChangeAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    XmlElement *groupBeforeChild = xml.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = xml.getChildByName(Serialization::Undo::groupAfter);
    
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
    this->layerId.clear();
}

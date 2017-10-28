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
#include "KeySignatureEventActions.h"
#include "KeySignaturesSequence.h"
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

KeySignatureEventInsertAction::KeySignatureEventInsertAction(ProjectTreeItem &parentProject,
                                                         String targetTrackId,
                                                         const KeySignatureEvent &event) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId)),
    event(event)
{
}

bool KeySignatureEventInsertAction::perform()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

bool KeySignatureEventInsertAction::undo()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

int KeySignatureEventInsertAction::getSizeInUnits()
{
    return sizeof(KeySignatureEvent);
}

XmlElement *KeySignatureEventInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::keySignatureEventInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void KeySignatureEventInsertAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void KeySignatureEventInsertAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

KeySignatureEventRemoveAction::KeySignatureEventRemoveAction(ProjectTreeItem &parentProject,
                                                         String targetTrackId,
                                                         const KeySignatureEvent &target) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId)),
    event(target)
{
}

bool KeySignatureEventRemoveAction::perform()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->remove(this->event, false);
    }
    
    return false;
}

bool KeySignatureEventRemoveAction::undo()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return (sequence->insert(this->event, false) != nullptr);
    }
    
    return false;
}

int KeySignatureEventRemoveAction::getSizeInUnits()
{
    return sizeof(KeySignatureEvent);
}

XmlElement *KeySignatureEventRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::keySignatureEventRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->event.serialize());
    return xml;
}

void KeySignatureEventRemoveAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->event.deserialize(*xml.getFirstChildElement());
}

void KeySignatureEventRemoveAction::reset()
{
    this->event.reset();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

KeySignatureEventChangeAction::KeySignatureEventChangeAction(ProjectTreeItem &parentProject,
                                                         String targetTrackId,
                                                         const KeySignatureEvent &target,
                                                         const KeySignatureEvent &newParameters) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId)),
    eventBefore(target),
    eventAfter(newParameters)
{

}

bool KeySignatureEventChangeAction::perform()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventBefore, this->eventAfter, false);
    }
    
    return false;
}

bool KeySignatureEventChangeAction::undo()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->change(this->eventAfter, this->eventBefore, false);
    }
    
    return false;
}

int KeySignatureEventChangeAction::getSizeInUnits()
{
    return sizeof(KeySignatureEvent) * 2;
}

UndoAction *KeySignatureEventChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        if (KeySignatureEventChangeAction *nextChanger =
            dynamic_cast<KeySignatureEventChangeAction *>(nextAction))
        {
            const bool idsAreEqual = 
                (this->eventBefore.getId() == nextChanger->eventAfter.getId() &&
                    this->trackId == nextChanger->trackId);
            
            if (idsAreEqual)
            {
                return new KeySignatureEventChangeAction(this->project,
                    this->trackId, this->eventBefore, nextChanger->eventAfter);
            }
        }
    }

    (void) nextAction;
    return nullptr;
}

XmlElement *KeySignatureEventChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::keySignatureEventChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    auto KeySignatureBeforeChild = new XmlElement(Serialization::Undo::keySignatureBefore);
    KeySignatureBeforeChild->prependChildElement(this->eventBefore.serialize());
    xml->prependChildElement(KeySignatureBeforeChild);
    
    auto KeySignatureAfterChild = new XmlElement(Serialization::Undo::keySignatureAfter);
    KeySignatureAfterChild->prependChildElement(this->eventAfter.serialize());
    xml->prependChildElement(KeySignatureAfterChild);
    
    return xml;
}

void KeySignatureEventChangeAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *KeySignatureBeforeChild = xml.getChildByName(Serialization::Undo::keySignatureBefore);
    XmlElement *KeySignatureAfterChild = xml.getChildByName(Serialization::Undo::keySignatureAfter);
    
    this->eventBefore.deserialize(*KeySignatureBeforeChild->getFirstChildElement());
    this->eventAfter.deserialize(*KeySignatureAfterChild->getFirstChildElement());
}

void KeySignatureEventChangeAction::reset()
{
    this->eventBefore.reset();
    this->eventAfter.reset();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

KeySignatureEventsGroupInsertAction::KeySignatureEventsGroupInsertAction(ProjectTreeItem &parentProject,
                                                                     String targetTrackId,
                                                                     Array<KeySignatureEvent> &target) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId))
{
    this->signatures.swapWith(target);
}

bool KeySignatureEventsGroupInsertAction::perform()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->insertGroup(this->signatures, false);
    }
    
    return false;
}

bool KeySignatureEventsGroupInsertAction::undo()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->removeGroup(this->signatures, false);
    }
    
    return false;
}

int KeySignatureEventsGroupInsertAction::getSizeInUnits()
{
    return (sizeof(KeySignatureEvent) * this->signatures.size());
}

XmlElement *KeySignatureEventsGroupInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::keySignatureEventsGroupInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        xml->prependChildElement(this->signatures.getUnchecked(i).serialize());
    }
    
    return xml;
}

void KeySignatureEventsGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        KeySignatureEvent ae;
        ae.deserialize(*noteXml);
        this->signatures.add(ae);
    }
}

void KeySignatureEventsGroupInsertAction::reset()
{
    this->signatures.clear();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

KeySignatureEventsGroupRemoveAction::KeySignatureEventsGroupRemoveAction(ProjectTreeItem &parentProject,
                                                                     String targetTrackId,
                                                                     Array<KeySignatureEvent> &target) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId))
{
    this->signatures.swapWith(target);
}

bool KeySignatureEventsGroupRemoveAction::perform()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->removeGroup(this->signatures, false);
    }
    
    return false;
}

bool KeySignatureEventsGroupRemoveAction::undo()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->insertGroup(this->signatures, false);
    }
    
    return false;
}

int KeySignatureEventsGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(KeySignatureEvent) * this->signatures.size());
}

XmlElement *KeySignatureEventsGroupRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::keySignatureEventsGroupRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->signatures.size(); ++i)
    {
        xml->prependChildElement(this->signatures.getUnchecked(i).serialize());
    }
    
    return xml;
}

void KeySignatureEventsGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        KeySignatureEvent ae;
        ae.deserialize(*noteXml);
        this->signatures.add(ae);
    }
}

void KeySignatureEventsGroupRemoveAction::reset()
{
    this->signatures.clear();
    this->trackId.clear();
}


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

KeySignatureEventsGroupChangeAction::KeySignatureEventsGroupChangeAction(ProjectTreeItem &parentProject,
                                                                     String targetTrackId,
                                                                     const Array<KeySignatureEvent> state1,
                                                                     const Array<KeySignatureEvent> state2) :
    UndoAction(parentProject),
    trackId(std::move(targetTrackId))
{
    this->eventsBefore.addArray(state1);
    this->eventsAfter.addArray(state2);
}

bool KeySignatureEventsGroupChangeAction::perform()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsBefore, this->eventsAfter, false);
    }
    
    return false;
}

bool KeySignatureEventsGroupChangeAction::undo()
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        return sequence->changeGroup(this->eventsAfter, this->eventsBefore, false);
    }
    
    return false;
}

int KeySignatureEventsGroupChangeAction::getSizeInUnits()
{
    return (sizeof(KeySignatureEvent) * this->eventsBefore.size()) +
           (sizeof(KeySignatureEvent) * this->eventsAfter.size());
}

UndoAction *KeySignatureEventsGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (KeySignaturesSequence *sequence =
        this->project.findSequenceByTrackId<KeySignaturesSequence>(this->trackId))
    {
        if (KeySignatureEventsGroupChangeAction *nextChanger =
            dynamic_cast<KeySignatureEventsGroupChangeAction *>(nextAction))
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
                return new KeySignatureEventsGroupChangeAction(this->project,
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

XmlElement *KeySignatureEventsGroupChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::keySignatureEventsGroupChangeAction);
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

void KeySignatureEventsGroupChangeAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *groupBeforeChild = xml.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = xml.getChildByName(Serialization::Undo::groupAfter);
    
    forEachXmlChildElement(*groupBeforeChild, eventXml)
    {
        KeySignatureEvent ae;
        ae.deserialize(*eventXml);
        this->eventsBefore.add(ae);
    }
    
    forEachXmlChildElement(*groupAfterChild, eventXml)
    {
        KeySignatureEvent ae;
        ae.deserialize(*eventXml);
        this->eventsAfter.add(ae);
    }
}

void KeySignatureEventsGroupChangeAction::reset()
{
    this->eventsBefore.clear();
    this->eventsAfter.clear();
    this->trackId.clear();
}

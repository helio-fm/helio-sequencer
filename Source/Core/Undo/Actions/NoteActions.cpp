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
#include "NoteActions.h"
#include "PianoLayer.h"
#include "ProjectTreeItem.h"
#include "SerializationKeys.h"


//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

NoteInsertAction::NoteInsertAction(ProjectTreeItem &parentProject,
                                   String targetLayerId,
                                   const Note &event) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    note(event)
{

}

bool NoteInsertAction::perform()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return (layer->insert(this->note, false) != nullptr);
    }
    
    return false;
}

bool NoteInsertAction::undo()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->remove(this->note, false);
    }
    
    return false;
}

int NoteInsertAction::getSizeInUnits()
{
    return sizeof(Note);
}

XmlElement *NoteInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::noteInsertAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->note.serialize());
    return xml;
}

void NoteInsertAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->note.deserialize(*xml.getFirstChildElement());
}

void NoteInsertAction::reset()
{
    this->note.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

NoteRemoveAction::NoteRemoveAction(ProjectTreeItem &parentProject,
                                   String targetLayerId,
                                   const Note &event) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    note(event)
{

}

bool NoteRemoveAction::perform()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->remove(this->note, false);
    }
    
    return false;
}

bool NoteRemoveAction::undo()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return (layer->insert(this->note, false) != nullptr);
    }
    
    return false;
}

int NoteRemoveAction::getSizeInUnits()
{
    return sizeof(Note);
}

XmlElement *NoteRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::noteRemoveAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    xml->prependChildElement(this->note.serialize());
    return xml;
}

void NoteRemoveAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    this->note.deserialize(*xml.getFirstChildElement());
}

void NoteRemoveAction::reset()
{
    this->note.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

NoteChangeAction::NoteChangeAction(ProjectTreeItem &parentProject,
                                   String targetLayerId,
                                   const Note &note,
                                   const Note &newParameters) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId)),
    noteBefore(note),
    noteAfter(newParameters)
{
    jassert(note.getId() == newParameters.getId());
}

bool NoteChangeAction::perform()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->change(this->noteBefore, this->noteAfter, false);
    }
    
    return false;
}

bool NoteChangeAction::undo()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->change(this->noteAfter, this->noteBefore, false);
    }
    
    return false;
}

int NoteChangeAction::getSizeInUnits()
{
    return sizeof(Note) * 2;
}

UndoAction *NoteChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        if (NoteChangeAction *nextChanger = dynamic_cast<NoteChangeAction *>(nextAction))
        {
            // если прямо объединять события - это портит групповые изменения, так что смотрим по id
            const bool idsAreEqual = (this->noteBefore.getId() == nextChanger->noteAfter.getId() &&
                                      this->layerId == nextChanger->layerId);
            
            if (idsAreEqual)
            {
                auto newChanger = new NoteChangeAction(this->project, this->layerId, this->noteBefore, nextChanger->noteAfter);
                return newChanger;
            }
        }
    }
    
    (void) nextAction;
    return nullptr;
}

XmlElement *NoteChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::noteChangeAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    auto noteBeforeChild = new XmlElement(Serialization::Undo::noteBefore);
    noteBeforeChild->prependChildElement(this->noteBefore.serialize());
    xml->prependChildElement(noteBeforeChild);

    auto noteAfterChild = new XmlElement(Serialization::Undo::noteAfter);
    noteAfterChild->prependChildElement(this->noteAfter.serialize());
    xml->prependChildElement(noteAfterChild);

    return xml;
}

void NoteChangeAction::deserialize(const XmlElement &xml)
{
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    XmlElement *noteBeforeChild = xml.getChildByName(Serialization::Undo::noteBefore);
    XmlElement *noteAfterChild = xml.getChildByName(Serialization::Undo::noteAfter);
    
    this->noteBefore.deserialize(*noteBeforeChild->getFirstChildElement());
    this->noteAfter.deserialize(*noteAfterChild->getFirstChildElement());
}

void NoteChangeAction::reset()
{
    this->noteBefore.reset();
    this->noteAfter.reset();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

NotesGroupInsertAction::NotesGroupInsertAction(ProjectTreeItem &parentProject,
                                               String targetLayerId,
                                               Array<Note> &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->notes.swapWith(target);
}

bool NotesGroupInsertAction::perform()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->insertGroup(this->notes, false);
    }
    
    return false;
}

bool NotesGroupInsertAction::undo()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->removeGroup(this->notes, false);
    }
    
    return false;
}

int NotesGroupInsertAction::getSizeInUnits()
{
    return (sizeof(Note) * this->notes.size());
}

XmlElement *NotesGroupInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::notesGroupInsertAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        xml->prependChildElement(this->notes.getUnchecked(i).serialize());
    }
    
    return xml;
}

void NotesGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        Note n;
        n.deserialize(*noteXml);
        this->notes.add(n);
    }
}

void NotesGroupInsertAction::reset()
{
    this->notes.clear();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

NotesGroupRemoveAction::NotesGroupRemoveAction(ProjectTreeItem &parentProject,
                                               String targetLayerId,
                                               Array<Note> &target) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->notes.swapWith(target);
}

bool NotesGroupRemoveAction::perform()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->removeGroup(this->notes, false);
    }
    
    return false;
}

bool NotesGroupRemoveAction::undo()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->insertGroup(this->notes, false);
    }
    
    return false;
}

int NotesGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(Note) * this->notes.size());
}

XmlElement *NotesGroupRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::notesGroupRemoveAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        xml->prependChildElement(this->notes.getUnchecked(i).serialize());
    }
    
    return xml;
}

void NotesGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    forEachXmlChildElement(xml, noteXml)
    {
        Note n;
        n.deserialize(*noteXml);
        this->notes.add(n);
    }
}

void NotesGroupRemoveAction::reset()
{
    this->notes.clear();
    this->layerId.clear();
}


//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

NotesGroupChangeAction::NotesGroupChangeAction(ProjectTreeItem &parentProject,
                                               String targetLayerId,
                                               Array<Note> &state1,
                                               Array<Note> &state2) :
    UndoAction(parentProject),
    layerId(std::move(targetLayerId))
{
    this->notesBefore.swapWith(state1);
    this->notesAfter.swapWith(state2);
}

bool NotesGroupChangeAction::perform()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->changeGroup(this->notesBefore, this->notesAfter, false);
    }
    
    return false;
}

bool NotesGroupChangeAction::undo()
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        return layer->changeGroup(this->notesAfter, this->notesBefore, false);
    }
    
    return false;
}

int NotesGroupChangeAction::getSizeInUnits()
{
    return (sizeof(Note) * this->notesBefore.size()) + (sizeof(Note) * this->notesAfter.size());
}

UndoAction *NotesGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (PianoLayer *layer = this->project.getLayerWithId<PianoLayer>(this->layerId))
    {
        if (NotesGroupChangeAction *nextChanger = dynamic_cast<NotesGroupChangeAction *>(nextAction))
        {
            if (nextChanger->layerId != this->layerId)
            {
                return nullptr;
            }
            
            if (this->notesBefore.size() != nextChanger->notesAfter.size())
            {
                return nullptr;
            }
            
            for (int i = 0; i < this->notesBefore.size(); ++i)
            {
                if (this->notesBefore.getUnchecked(i).getId() != nextChanger->notesAfter.getUnchecked(i).getId())
                {
                    return nullptr;
                }
            }
            
            auto newChanger =
            new NotesGroupChangeAction(this->project, this->layerId, this->notesBefore, nextChanger->notesAfter);
            
            return newChanger;
        }
    }

    (void) nextAction;
    return nullptr;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *NotesGroupChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::notesGroupChangeAction);
    xml->setAttribute(Serialization::Undo::layerId, this->layerId);
    
    auto groupBeforeChild = new XmlElement(Serialization::Undo::groupBefore);
    auto groupAfterChild = new XmlElement(Serialization::Undo::groupAfter);
    
    for (int i = 0; i < this->notesBefore.size(); ++i)
    {
        groupBeforeChild->prependChildElement(this->notesBefore.getUnchecked(i).serialize());
    }
    
    for (int i = 0; i < this->notesAfter.size(); ++i)
    {
        groupAfterChild->prependChildElement(this->notesAfter.getUnchecked(i).serialize());
    }
    
    xml->prependChildElement(groupBeforeChild);
    xml->prependChildElement(groupAfterChild);
    
    return xml;
}

void NotesGroupChangeAction::deserialize(const XmlElement &xml)
{
    this->reset();
    
    this->layerId = xml.getStringAttribute(Serialization::Undo::layerId);
    
    XmlElement *groupBeforeChild = xml.getChildByName(Serialization::Undo::groupBefore);
    XmlElement *groupAfterChild = xml.getChildByName(Serialization::Undo::groupAfter);

    forEachXmlChildElement(*groupBeforeChild, noteXml)
    {
        Note n;
        n.deserialize(*noteXml);
        this->notesBefore.add(n);
    }

    forEachXmlChildElement(*groupAfterChild, noteXml)
    {
        Note n;
        n.deserialize(*noteXml);
        this->notesAfter.add(n);
    }
}

void NotesGroupChangeAction::reset()
{
    this->notesBefore.clear();
    this->notesAfter.clear();
    this->layerId.clear();
}

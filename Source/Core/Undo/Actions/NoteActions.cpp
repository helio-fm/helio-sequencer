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
#include "MidiTrack.h"
#include "PianoSequence.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert
//===----------------------------------------------------------------------===//

NoteInsertAction::NoteInsertAction(MidiTrackSource &source,
    String targetTrackId, const Note &event) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    note(event) {}

bool NoteInsertAction::perform()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return (sequence->insert(this->note, false) != nullptr);
    }
    
    return false;
}

bool NoteInsertAction::undo()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->remove(this->note, false);
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
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->note.serialize());
    return xml;
}

void NoteInsertAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->note.deserialize(*xml.getFirstChildElement());
}

void NoteInsertAction::reset()
{
    this->note.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

NoteRemoveAction::NoteRemoveAction(MidiTrackSource &source,
    String targetTrackId, const Note &event) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    note(event) {}

bool NoteRemoveAction::perform()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->remove(this->note, false);
    }
    
    return false;
}

bool NoteRemoveAction::undo()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return (sequence->insert(this->note, false) != nullptr);
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
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->note.serialize());
    return xml;
}

void NoteRemoveAction::deserialize(const XmlElement &xml)
{
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    this->note.deserialize(*xml.getFirstChildElement());
}

void NoteRemoveAction::reset()
{
    this->note.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change
//===----------------------------------------------------------------------===//

NoteChangeAction::NoteChangeAction(MidiTrackSource &source,
    String targetTrackId, const Note &note, const Note &newParameters) :
    UndoAction(source),
    trackId(std::move(targetTrackId)),
    noteBefore(note),
    noteAfter(newParameters)
{
    jassert(note.getId() == newParameters.getId());
}

bool NoteChangeAction::perform()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->change(this->noteBefore, this->noteAfter, false);
    }
    
    return false;
}

bool NoteChangeAction::undo()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->change(this->noteAfter, this->noteBefore, false);
    }
    
    return false;
}

int NoteChangeAction::getSizeInUnits()
{
    return sizeof(Note) * 2;
}

UndoAction *NoteChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        if (NoteChangeAction *nextChanger =
            dynamic_cast<NoteChangeAction *>(nextAction))
        {
            // checking id's here is necessary to keep group changes ok
            const bool idsAreEqual =
                (this->noteBefore.getId() == nextChanger->noteAfter.getId() &&
                    this->trackId == nextChanger->trackId);
            
            if (idsAreEqual)
            {
                return new NoteChangeAction(this->source,
                    this->trackId, this->noteBefore, nextChanger->noteAfter);
            }
        }
    }
    
    (void) nextAction;
    return nullptr;
}

XmlElement *NoteChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::noteChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
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
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
    XmlElement *noteBeforeChild = xml.getChildByName(Serialization::Undo::noteBefore);
    XmlElement *noteAfterChild = xml.getChildByName(Serialization::Undo::noteAfter);
    
    this->noteBefore.deserialize(*noteBeforeChild->getFirstChildElement());
    this->noteAfter.deserialize(*noteAfterChild->getFirstChildElement());
}

void NoteChangeAction::reset()
{
    this->noteBefore.reset();
    this->noteAfter.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Insert Group
//===----------------------------------------------------------------------===//

NotesGroupInsertAction::NotesGroupInsertAction(MidiTrackSource &source,
    String targetTrackId, Array<Note> &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
{
    this->notes.swapWith(target);
}

bool NotesGroupInsertAction::perform()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->insertGroup(this->notes, false);
    }
    
    return false;
}

bool NotesGroupInsertAction::undo()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->removeGroup(this->notes, false);
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
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        xml->prependChildElement(this->notes.getUnchecked(i).serialize());
    }
    
    return xml;
}

void NotesGroupInsertAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
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
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

NotesGroupRemoveAction::NotesGroupRemoveAction(MidiTrackSource &source,
    String targetTrackId, Array<Note> &target) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
{
    this->notes.swapWith(target);
}

bool NotesGroupRemoveAction::perform()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->removeGroup(this->notes, false);
    }
    
    return false;
}

bool NotesGroupRemoveAction::undo()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->insertGroup(this->notes, false);
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
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        xml->prependChildElement(this->notes.getUnchecked(i).serialize());
    }
    
    return xml;
}

void NotesGroupRemoveAction::deserialize(const XmlElement &xml)
{
    this->reset();
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
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
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Group
//===----------------------------------------------------------------------===//

NotesGroupChangeAction::NotesGroupChangeAction(MidiTrackSource &source,
    String targetTrackId, Array<Note> &state1, Array<Note> &state2) :
    UndoAction(source),
    trackId(std::move(targetTrackId))
{
    this->notesBefore.swapWith(state1);
    this->notesAfter.swapWith(state2);
}

bool NotesGroupChangeAction::perform()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->changeGroup(this->notesBefore, this->notesAfter, false);
    }
    
    return false;
}

bool NotesGroupChangeAction::undo()
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->changeGroup(this->notesAfter, this->notesBefore, false);
    }
    
    return false;
}

int NotesGroupChangeAction::getSizeInUnits()
{
    return (sizeof(Note) * this->notesBefore.size()) +
        (sizeof(Note) * this->notesAfter.size());
}

UndoAction *NotesGroupChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (PianoSequence *sequence =
        this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        if (NotesGroupChangeAction *nextChanger =
            dynamic_cast<NotesGroupChangeAction *>(nextAction))
        {
            if (nextChanger->trackId != this->trackId)
            {
                return nullptr;
            }
            
            if (this->notesBefore.size() != nextChanger->notesAfter.size())
            {
                return nullptr;
            }
            
            for (int i = 0; i < this->notesBefore.size(); ++i)
            {
                if (this->notesBefore.getUnchecked(i).getId() !=
                    nextChanger->notesAfter.getUnchecked(i).getId())
                {
                    return nullptr;
                }
            }
            
            return new NotesGroupChangeAction(this->source,
                this->trackId, this->notesBefore, nextChanger->notesAfter);
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
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    
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
    
    this->trackId = xml.getStringAttribute(Serialization::Undo::trackId);
    
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
    this->trackId.clear();
}

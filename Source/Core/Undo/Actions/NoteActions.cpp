/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
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
    const String &trackId, const Note &event) noexcept :
    UndoAction(source),
    trackId(trackId),
    note(event) {}

bool NoteInsertAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return (sequence->insert(this->note, false) != nullptr);
    }
    
    return false;
}

bool NoteInsertAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->remove(this->note, false);
    }
    
    return false;
}

int NoteInsertAction::getSizeInUnits()
{
    return sizeof(Note);
}

SerializedData NoteInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::noteInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->note.serialize());
    return tree;
}

void NoteInsertAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->note.deserialize(data.getChild(0));
}

void NoteInsertAction::reset()
{
    this->note.reset();
    this->trackId.clear();
}

// a way to make note insertions a bit smarter;
// two single note inserts will coalesce into a grouped insert,
// and the grouped insert action will coalesce with the following
// single insert as well, so that several single insert actions
// will always end up being coalesced into one grouped insert action:
UndoAction *NoteInsertAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextChanger = dynamic_cast<NoteInsertAction *>(nextAction))
    {
        if (nextChanger->trackId != this->trackId)
        {
            return nullptr;
        }

        return new NotesGroupInsertAction(this->source,
            this->trackId, this->note, nextChanger->note);
    }

    (void)nextAction;
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Remove
//===----------------------------------------------------------------------===//

NoteRemoveAction::NoteRemoveAction(MidiTrackSource &source,
    const String &trackId, const Note &event) noexcept :
    UndoAction(source),
    trackId(trackId),
    note(event) {}

bool NoteRemoveAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->remove(this->note, false);
    }
    
    return false;
}

bool NoteRemoveAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return (sequence->insert(this->note, false) != nullptr);
    }
    
    return false;
}

int NoteRemoveAction::getSizeInUnits()
{
    return sizeof(Note);
}

SerializedData NoteRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::noteRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    tree.appendChild(this->note.serialize());
    return tree;
}

void NoteRemoveAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    this->note.deserialize(data.getChild(0));
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
    const String &trackId, const Note &note, const Note &newParameters) noexcept :
    UndoAction(source),
    trackId(trackId),
    noteBefore(note),
    noteAfter(newParameters)
{
    jassert(note.getId() == newParameters.getId());
}

bool NoteChangeAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->change(this->noteBefore, this->noteAfter, false);
    }
    
    return false;
}

bool NoteChangeAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
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
    if (auto *nextChanger = dynamic_cast<NoteChangeAction *>(nextAction))
    {
        const bool idsAreEqual =
            (this->noteBefore.getId() == nextChanger->noteAfter.getId() &&
                this->trackId == nextChanger->trackId);
            
        if (idsAreEqual)
        {
            return new NoteChangeAction(this->source,
                this->trackId, this->noteBefore, nextChanger->noteAfter);
        }
    }
    
    (void) nextAction;
    return nullptr;
}

SerializedData NoteChangeAction::serialize() const
{
    SerializedData tree(Serialization::Undo::noteChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    SerializedData noteBeforeChild(Serialization::Undo::noteBefore);
    noteBeforeChild.appendChild(this->noteBefore.serialize());
    tree.appendChild(noteBeforeChild);

    SerializedData noteAfterChild(Serialization::Undo::noteAfter);
    noteAfterChild.appendChild(this->noteAfter.serialize());
    tree.appendChild(noteAfterChild);

    return tree;
}

void NoteChangeAction::deserialize(const SerializedData &data)
{
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    const auto noteBeforeChild = data.getChildWithName(Serialization::Undo::noteBefore);
    const auto noteAfterChild = data.getChildWithName(Serialization::Undo::noteAfter);
    
    this->noteBefore.deserialize(noteBeforeChild.getChild(0));
    this->noteAfter.deserialize(noteAfterChild.getChild(0));
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
    const String &trackId, Array<Note> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->notes.swapWith(target);
}

NotesGroupInsertAction::NotesGroupInsertAction(MidiTrackSource &source,
    const String &trackId, Note &action1Note, Note &action2Note) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->notes.add(action1Note, action2Note);
}

bool NotesGroupInsertAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->insertGroup(this->notes, false);
    }
    
    return false;
}

bool NotesGroupInsertAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->removeGroup(this->notes, false);
    }
    
    return false;
}

int NotesGroupInsertAction::getSizeInUnits()
{
    return (sizeof(Note) * this->notes.size());
}

SerializedData NotesGroupInsertAction::serialize() const
{
    SerializedData tree(Serialization::Undo::notesGroupInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        tree.appendChild(this->notes.getUnchecked(i).serialize());
    }
    
    return tree;
}

void NotesGroupInsertAction::deserialize(const SerializedData &data)
{
    this->reset();
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    for (const auto &props : data)
    {
        Note n;
        n.deserialize(props);
        this->notes.add(n);
    }
}

void NotesGroupInsertAction::reset()
{
    this->notes.clear();
    this->trackId.clear();
}

UndoAction *NotesGroupInsertAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextGroup = dynamic_cast<NotesGroupInsertAction *>(nextAction))
    {
        if (nextGroup->trackId != this->trackId)
        {
            return nullptr;
        }

        this->notes.addArray(nextGroup->notes);
        return new NotesGroupInsertAction(this->source,
            this->trackId, this->notes);
    }
    else if (auto *nextChanger = dynamic_cast<NoteInsertAction *>(nextAction))
    {
        if (nextChanger->trackId != this->trackId)
        {
            return nullptr;
        }

        this->notes.add(nextChanger->note);
        return new NotesGroupInsertAction(this->source,
            this->trackId, this->notes);
    }

    (void)nextAction;
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Remove Group
//===----------------------------------------------------------------------===//

NotesGroupRemoveAction::NotesGroupRemoveAction(MidiTrackSource &source,
    const String &trackId, Array<Note> &target) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->notes.swapWith(target);
}

bool NotesGroupRemoveAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->removeGroup(this->notes, false);
    }
    
    return false;
}

bool NotesGroupRemoveAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->insertGroup(this->notes, false);
    }
    
    return false;
}

int NotesGroupRemoveAction::getSizeInUnits()
{
    return (sizeof(Note) * this->notes.size());
}

SerializedData NotesGroupRemoveAction::serialize() const
{
    SerializedData tree(Serialization::Undo::notesGroupRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        tree.appendChild(this->notes.getUnchecked(i).serialize());
    }
    
    return tree;
}

void NotesGroupRemoveAction::deserialize(const SerializedData &data)
{
    this->reset();
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    for (const auto &props : data)
    {
        Note n;
        n.deserialize(props);
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
    const String &trackId, Array<Note> &state1, Array<Note> &state2) noexcept :
    UndoAction(source),
    trackId(trackId)
{
    this->notesBefore.swapWith(state1);
    this->notesAfter.swapWith(state2);
}

bool NotesGroupChangeAction::perform()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
    {
        return sequence->changeGroup(this->notesBefore, this->notesAfter, false);
    }
    
    return false;
}

bool NotesGroupChangeAction::undo()
{
    if (auto *sequence = this->source.findSequenceByTrackId<PianoSequence>(this->trackId))
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
    if (auto *nextChanger = dynamic_cast<NotesGroupChangeAction *>(nextAction))
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

    (void) nextAction;
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData NotesGroupChangeAction::serialize() const
{
    SerializedData tree(Serialization::Undo::notesGroupChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId);
    
    SerializedData groupBeforeChild(Serialization::Undo::groupBefore);
    SerializedData groupAfterChild(Serialization::Undo::groupAfter);
    
    for (int i = 0; i < this->notesBefore.size(); ++i)
    {
        groupBeforeChild.appendChild(this->notesBefore.getUnchecked(i).serialize());
    }
    
    for (int i = 0; i < this->notesAfter.size(); ++i)
    {
        groupAfterChild.appendChild(this->notesAfter.getUnchecked(i).serialize());
    }
    
    tree.appendChild(groupBeforeChild);
    tree.appendChild(groupAfterChild);
    
    return tree;
}

void NotesGroupChangeAction::deserialize(const SerializedData &data)
{
    this->reset();
    
    this->trackId = data.getProperty(Serialization::Undo::trackId);
    
    const auto groupBeforeChild = data.getChildWithName(Serialization::Undo::groupBefore);
    const auto groupAfterChild = data.getChildWithName(Serialization::Undo::groupAfter);

    for (const auto &props : groupBeforeChild)
    {
        Note n;
        n.deserialize(props);
        this->notesBefore.add(n);
    }

    for (const auto &props : groupAfterChild)
    {
        Note n;
        n.deserialize(props);
        this->notesAfter.add(n);
    }
}

void NotesGroupChangeAction::reset()
{
    this->notesBefore.clear();
    this->notesAfter.clear();
    this->trackId.clear();
}

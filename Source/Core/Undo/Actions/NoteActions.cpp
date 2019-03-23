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
    const String &trackId, const Note &event) noexcept :
    UndoAction(source),
    trackId(trackId),
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

ValueTree NoteInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::noteInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->note.serialize(), nullptr);
    return tree;
}

void NoteInsertAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->note.deserialize(tree.getChild(0));
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
    const String &trackId, const Note &event) noexcept :
    UndoAction(source),
    trackId(trackId),
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

ValueTree NoteRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::noteRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    tree.appendChild(this->note.serialize(), nullptr);
    return tree;
}

void NoteRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    this->note.deserialize(tree.getChild(0));
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

ValueTree NoteChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::noteChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    ValueTree noteBeforeChild(Serialization::Undo::noteBefore);
    noteBeforeChild.appendChild(this->noteBefore.serialize(), nullptr);
    tree.appendChild(noteBeforeChild, nullptr);

    ValueTree noteAfterChild(Serialization::Undo::noteAfter);
    noteAfterChild.appendChild(this->noteAfter.serialize(), nullptr);
    tree.appendChild(noteAfterChild, nullptr);

    return tree;
}

void NoteChangeAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    const auto noteBeforeChild = tree.getChildWithName(Serialization::Undo::noteBefore);
    const auto noteAfterChild = tree.getChildWithName(Serialization::Undo::noteAfter);
    
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

ValueTree NotesGroupInsertAction::serialize() const
{
    ValueTree tree(Serialization::Undo::notesGroupInsertAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        tree.appendChild(this->notes.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void NotesGroupInsertAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    for (const auto &props : tree)
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

ValueTree NotesGroupRemoveAction::serialize() const
{
    ValueTree tree(Serialization::Undo::notesGroupRemoveAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    for (int i = 0; i < this->notes.size(); ++i)
    {
        tree.appendChild(this->notes.getUnchecked(i).serialize(), nullptr);
    }
    
    return tree;
}

void NotesGroupRemoveAction::deserialize(const ValueTree &tree)
{
    this->reset();
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    for (const auto &props : tree)
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

ValueTree NotesGroupChangeAction::serialize() const
{
    ValueTree tree(Serialization::Undo::notesGroupChangeAction);
    tree.setProperty(Serialization::Undo::trackId, this->trackId, nullptr);
    
    ValueTree groupBeforeChild(Serialization::Undo::groupBefore);
    ValueTree groupAfterChild(Serialization::Undo::groupAfter);
    
    for (int i = 0; i < this->notesBefore.size(); ++i)
    {
        groupBeforeChild.appendChild(this->notesBefore.getUnchecked(i).serialize(), nullptr);
    }
    
    for (int i = 0; i < this->notesAfter.size(); ++i)
    {
        groupAfterChild.appendChild(this->notesAfter.getUnchecked(i).serialize(), nullptr);
    }
    
    tree.appendChild(groupBeforeChild, nullptr);
    tree.appendChild(groupAfterChild, nullptr);
    
    return tree;
}

void NotesGroupChangeAction::deserialize(const ValueTree &tree)
{
    this->reset();
    
    this->trackId = tree.getProperty(Serialization::Undo::trackId);
    
    const auto groupBeforeChild = tree.getChildWithName(Serialization::Undo::groupBefore);
    const auto groupAfterChild = tree.getChildWithName(Serialization::Undo::groupAfter);

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

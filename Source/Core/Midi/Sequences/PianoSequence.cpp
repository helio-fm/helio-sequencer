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
#include "PianoSequence.h"

#include "PianoRoll.h"
#include "Note.h"
#include "NoteActions.h"
#include "SerializationKeys.h"
#include "ProjectTreeItem.h"
#include "UndoStack.h"

PianoSequence::PianoSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void PianoSequence::importMidi(const MidiMessageSequence &sequence)
{
    this->clearUndoHistory();
    this->checkpoint();
    this->reset();

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const MidiMessage &messageOn = sequence.getEventPointer(i)->message;

        if (messageOn.isNoteOn())
        {
            const double startTimestamp = messageOn.getTimeStamp() / MIDI_IMPORT_SCALE;
            const int key = messageOn.getNoteNumber();
            const float velocity = messageOn.getVelocity() / 128.f;
            const float beat = float(startTimestamp);

            // find corresponding note-off:
            const int noteOffIndex = sequence.getIndexOfMatchingKeyUp(i);

            if (noteOffIndex > 0)
            {
                const MidiMessage &messageOff = sequence.getEventPointer(noteOffIndex)->message;
                const double endTimestamp = messageOff.getTimeStamp() / MIDI_IMPORT_SCALE;

                if (endTimestamp > startTimestamp)
                {
                    const float length = float(endTimestamp - startTimestamp);

                    // bottleneck warning
                    const Note note(this, key, beat, length, velocity);
                    this->silentImport(note);
                }
            }
        }
    }

    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

//===----------------------------------------------------------------------===//
// Undo-able track editing
//===----------------------------------------------------------------------===//

void PianoSequence::silentImport(const MidiEvent &eventToImport)
{
    const auto &note = static_cast<const Note &>(eventToImport);
    jassert(note.isValid());

    if (!this->usedEventIds.contains(note.getId()))
    {
        jassertfalse;
        return;
    }

    auto *storedNote = new Note(this, note);
    this->midiEvents.addSorted(*storedNote, storedNote); // bottleneck warning

    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

MidiEvent *PianoSequence::insert(const Note &eventParams, const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new NoteInsertAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        const auto ownedNote = new Note(this, eventParams);
        this->midiEvents.addSorted(*ownedNote, ownedNote);
        this->notifyEventAdded(*ownedNote);
        this->updateBeatRange(true);
        return ownedNote;
    }

    return nullptr;
}

bool PianoSequence::remove(const Note &eventParams, const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new NoteRemoveAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(eventParams, &eventParams);
        jassert(index >= 0);
        if (index >= 0)
        {
            MidiEvent *const removedNote = this->midiEvents[index];
            jassert(removedNote->isValid());
            this->notifyEventRemoved(*removedNote);
            this->midiEvents.remove(index, true);
            this->updateBeatRange(true);
            this->notifyEventRemovedPostAction();
            return true;
        }
        
        return false;
    }

    return true;
}

bool PianoSequence::change(const Note &oldParams,
    const Note &newParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new NoteChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
        jassert(index >= 0);
        if (index >= 0)
        {
            const auto changedNote = static_cast<Note *>(this->midiEvents[index]);
            changedNote->applyChanges(newParams);
            this->midiEvents.remove(index, false);
            this->midiEvents.addSorted(*changedNote, changedNote);
            this->notifyEventChanged(oldParams, *changedNote);
            this->updateBeatRange(true);
            return true;
        }
        
        return false;
    }

    return true;
}

bool PianoSequence::insertGroup(Array<Note> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new NotesGroupInsertAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const Note &eventParams = group.getUnchecked(i);
            const auto ownedNote = new Note(this, eventParams);
            this->midiEvents.addSorted(*ownedNote, ownedNote);
            this->notifyEventAdded(*ownedNote);
        }

        this->updateBeatRange(true);
    }

    return true;
}

bool PianoSequence::removeGroup(Array<Note> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new NotesGroupRemoveAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const Note &note = group.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(note, &note);
            // Hitting this assertion almost likely means that target note array
            // contains more than one instance of the same note, but from different clips.
            // All the code here and in SequencerOperations class assumes this never happens,
            // so make sure PianoRoll restricts editing scope to a single clip instance.
            jassert(index >= 0);
            if (index >= 0)
            {
                const auto removedNote = this->midiEvents[index];
                this->notifyEventRemoved(*removedNote);
                this->midiEvents.remove(index, true);
            }
        }

        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
    }

    return true;
}

bool PianoSequence::changeGroup(Array<Note> &groupBefore,
    Array<Note> &groupAfter, bool undoable)
{
    jassert(groupBefore.size() == groupAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new NotesGroupChangeAction(*this->getProject(),
                this->getTrackId(), groupBefore, groupAfter));
    }
    else
    {
        for (int i = 0; i < groupBefore.size(); ++i)
        {
            const Note &oldParams = groupBefore.getUnchecked(i);
            const Note &newParams = groupAfter.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
            jassert(index >= 0);
            if (index >= 0)
            {
                const auto changedNote = static_cast<Note *>(this->midiEvents[index]);
                changedNote->applyChanges(newParams);
                this->midiEvents.remove(index, false);
                this->midiEvents.addSorted(*changedNote, changedNote);
                this->notifyEventChanged(oldParams, *changedNote);
            }
        }

        this->updateBeatRange(true);
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Batch operations
//===----------------------------------------------------------------------===//

void PianoSequence::transposeAll(int keyDelta, bool shouldCheckpoint)
{
    if (this->midiEvents.size() == 0)
    {
        return;
    }

    Array<Note> groupBefore, groupAfter;

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        if (Note *n = dynamic_cast<Note *>(this->midiEvents.getUnchecked(i)))
        {
            Note n1 = *n;
            Note n2 = n1.withDeltaKey(keyDelta);

            groupBefore.add(n1);
            groupAfter.add(n2);
        }
    }

    if (shouldCheckpoint)
    {
        this->checkpoint();
    }

    this->changeGroup(groupBefore, groupAfter, true);
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

float PianoSequence::getLastBeat() const noexcept
{
    float lastBeat = -FLT_MAX;
    if (this->midiEvents.size() == 0)
    {
        return lastBeat;
    }

    // FIXME:
    // sometimes the last event is not the one that lasts longer
    // (as events *must* be sorted by start beat, not by end beat),
    // so here we have to iterate through a number of last events
    // to guess where the sequence really ends: 
    const int checkStart = jmax(0, this->midiEvents.size() - 5);
    for (int i = checkStart; i < this->midiEvents.size(); ++i)
    {
        const auto *n = static_cast<const Note *>(this->midiEvents.getUnchecked(i));
        lastBeat = jmax(lastBeat, n->getBeat() + n->getLength());
    }

    return lastBeat;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree PianoSequence::serialize() const
{
    ValueTree tree(Serialization::Midi::track);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr); // faster than addChildElement
    }
    
    return tree;
}

void PianoSequence::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root =
        tree.hasType(Serialization::Midi::track) ?
        tree : tree.getChildWithName(Serialization::Midi::track);

    if (!root.isValid())
    {
        return;
    }

    forEachValueTreeChildWithType(root, e, Serialization::Midi::note)
    {
        auto note = new Note(this);
        note->deserialize(e);

        this->midiEvents.add(note); // sorted later
        this->usedEventIds.insert(note->getId());
    }

    this->sort();
    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

void PianoSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
    this->invalidateSequenceCache();
}

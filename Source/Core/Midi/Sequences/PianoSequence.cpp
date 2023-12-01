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
#include "PianoSequence.h"

#include "PianoRoll.h"
#include "NoteActions.h"
#include "SerializationKeys.h"
#include "UndoStack.h"

PianoSequence::PianoSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void PianoSequence::importMidi(const MidiMessageSequence &sequence, short timeFormat)
{
    this->clearUndoHistory();
    this->checkpoint();

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const auto &messageOn = sequence.getEventPointer(i)->message;
        if (messageOn.isNoteOn())
        {
            const int key = messageOn.getNoteNumber();
            const float velocity = messageOn.getVelocity() / 128.f;
            const float startBeat = MidiSequence::midiTicksToBeats(messageOn.getTimeStamp(), timeFormat);
            const int noteOffIndex = sequence.getIndexOfMatchingKeyUp(i);
            if (noteOffIndex > 0)
            {
                const MidiMessage &messageOff = sequence.getEventPointer(noteOffIndex)->message;
                const float endBeat = MidiSequence::midiTicksToBeats(messageOff.getTimeStamp(), timeFormat);
                if (endBeat > startBeat)
                {
                    const float length = endBeat - startBeat;
                    const Note note(this, key, startBeat, length, velocity);
                    this->importMidiEvent<Note>(note);
                }
            }
        }
    }

    this->updateBeatRange(false);
}

//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

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
        auto *ownedNote = new Note(this, eventParams);
        this->midiEvents.addSorted(*ownedNote, ownedNote);
        this->eventDispatcher.dispatchAddEvent(*ownedNote);
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
            auto *removedNote = this->midiEvents.getUnchecked(index);
            jassert(removedNote->isValid());
            this->eventDispatcher.dispatchRemoveEvent(*removedNote);
            this->midiEvents.remove(index, true);
            this->updateBeatRange(true);
            this->eventDispatcher.dispatchPostRemoveEvent(this);
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
            auto *changedNote = static_cast<Note *>(this->midiEvents.getUnchecked(index));
            changedNote->applyChanges(newParams);
            this->midiEvents.remove(index, false);
            this->midiEvents.addSorted(*changedNote, changedNote);
            this->eventDispatcher.dispatchChangeEvent(oldParams, *changedNote);
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
            auto *ownedNote = new Note(this, eventParams);
            this->midiEvents.addSorted(*ownedNote, ownedNote);
            this->eventDispatcher.dispatchAddEvent(*ownedNote);
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
                auto *removedNote = this->midiEvents.getUnchecked(index);
                this->eventDispatcher.dispatchRemoveEvent(*removedNote);
                this->midiEvents.remove(index, true);
            }
        }

        this->updateBeatRange(true);
        this->eventDispatcher.dispatchPostRemoveEvent(this);
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
            const Note &oldParams = groupBefore.getReference(i);
            const Note &newParams = groupAfter.getReference(i);
            const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
            // if you're hitting this assertion, one of the reasons might be
            // allowing user to somehow select notes of different clips simultaneously,
            // and then editing the selection, which leads to applying the same
            // transformation to one set of notes twice, which is kinda nonsense,
            // so make sure the selection is always limited to active track and clip:
            jassert(index >= 0);
            if (index >= 0)
            {
                auto *changedNote = static_cast<Note *>(this->midiEvents.getUnchecked(index));
                changedNote->applyChanges(newParams);
                this->midiEvents.remove(index, false);
                this->midiEvents.addSorted(*changedNote, changedNote);
                this->eventDispatcher.dispatchChangeEvent(oldParams, *changedNote);
            }
        }

        this->updateBeatRange(true);
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

float PianoSequence::findLastBeat() const noexcept
{
    if (this->isEmpty())
    {
        return 0.f;
    }

    // FIXME:
    // sometimes the last event is not the one that lasts longer
    // (as events *must* be sorted by start beat, not by end beat),
    // so here we have to iterate through a number of last events
    // to guess where the sequence really ends, with still no guarantee
    // we'll end up with right event (not sure how to fix it at the moment):
    float lastBeat = -FLT_MAX;
    const int checkStart = jmax(0, this->midiEvents.size() - 20);
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

SerializedData PianoSequence::serialize() const
{
    SerializedData tree(Serialization::Midi::track);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const auto *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize());
    }
    
    return tree;
}

void PianoSequence::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root =
        data.hasType(Serialization::Midi::track) ?
        data : data.getChildWithName(Serialization::Midi::track);

    if (!root.isValid())
    {
        return;
    }

    // a hack to avoid generating a unique id each time we create `new Note(this)`:
    // instead, deserialize parameters into this temporary unowned struct,
    // and later create an owned note with known parameters
    Note parameters;

    forEachChildWithType(root, e, Serialization::Midi::note)
    {
        parameters.deserialize(e);
        
        if (this->usedEventIds.contains(parameters.getId()))
        {
            // this should not happen, since all notes should have unique ids, but
            jassertfalse;
            // due bugs in older versions and a more recent bug in moving notes between tracks,
            // there's a chance for hitting a duplicate id in some projects,
            // so the best thing we can do here is to fix the note's id right away
            // (todo remove this workaround in future versions)
            parameters = parameters.withNewId(this);
        }

        this->midiEvents.add(new Note(this, parameters));
        this->usedEventIds.insert(parameters.getId());
    }

    this->sort<Note>();
    this->updateBeatRange(false);
}

void PianoSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
}

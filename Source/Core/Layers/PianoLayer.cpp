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
#include "PianoLayer.h"

#include "PianoRoll.h"
#include "Note.h"
#include "NoteActions.h"
#include "SerializationKeys.h"
#include "ProjectTreeItem.h"
#include "UndoStack.h"

#include <float.h>

// todo optimize data structures >_<
// using std::dense_hash_map ?

PianoLayer::PianoLayer(MidiLayerOwner &parent) : MidiLayer(parent)
{
}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void PianoLayer::importMidi(const MidiMessageSequence &sequence)
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

            // ищем соответствующий note-off:
            const int noteOffIndex = sequence.getIndexOfMatchingKeyUp(i);

            if (noteOffIndex > 0)
            {
                const MidiMessage &messageOff = sequence.getEventPointer(noteOffIndex)->message;
                const double endTimestamp = messageOff.getTimeStamp() / MIDI_IMPORT_SCALE;

                if (endTimestamp > startTimestamp)
                {
                    const float length = float(endTimestamp - startTimestamp);

                    // bottleneck warning!
                    const Note note(this, key, beat, length, velocity);
                    this->silentImport(note);
                }
            }
        }
    }

    this->notifyBeatRangeChanged();
    this->notifyLayerChanged();
}


//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void PianoLayer::silentImport(const MidiEvent &eventToImport)
{
    const Note &note = static_cast<const Note &>(eventToImport);

    if (this->notesHashTable.contains(note))
    { return; }

    auto const storedNote = new Note(this);
    *storedNote = note;
    
    // we need it to be sorted just because of sequence building performance?
    this->midiEvents.addSorted(*storedNote, storedNote); // bottleneck warning
    this->notesHashTable.set(note, storedNote);

    this->updateBeatRange(false);
}

MidiEvent *PianoLayer::insert(const Note &note, const bool undoable)
{
    if (this->notesHashTable.contains(note))
    {
        return nullptr;
    }

    if (undoable)
    {
        this->getUndoStack()->perform(new NoteInsertAction(*this->getProject(),
                                                           this->getLayerIdAsString(),
                                                           note));
    }
    else
    {
        auto storedNote = new Note(this, note);
        
        this->midiEvents.addSorted(*storedNote, storedNote);
        this->notesHashTable.set(note, storedNote);

        this->notifyEventAdded(*storedNote);
        this->updateBeatRange(true);

        return storedNote;
    }

    return nullptr;
}

bool PianoLayer::remove(const Note &note, const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new NoteRemoveAction(*this->getProject(),
                                                           this->getLayerIdAsString(),
                                                           note));
    }
    else
    {
        // fixme! use dense_hash_map of <int noteHash, Note *objectPointer>
        if (Note *matchingNote = this->notesHashTable[note])
        {
            this->notifyEventRemoved(*matchingNote);
            
            const int matchingNoteIndex = this->indexOfSorted(matchingNote);
            this->midiEvents.remove(matchingNoteIndex, true);
            //this->midiEvents.removeObject(matchingNote);
            
            this->notesHashTable.remove(note);
            this->updateBeatRange(true);
            this->notifyEventRemovedPostAction();
            return true;
        }
        
        
            return false;
        
    }

    return true;
}

bool PianoLayer::change(const Note &note,
                        const Note &newNote,
                        const bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new NoteChangeAction(*this->getProject(),
                                                           this->getLayerIdAsString(),
                                                           note,
                                                           newNote));
    }
    else
    {
        if (Note *matchingNote = this->notesHashTable[note])
        {
            // fixme - remove and addSorted instead?
            (*matchingNote) = newNote;

            this->notesHashTable.set(newNote, matchingNote);

            // fixme - remove and addSorted instead?
            this->sort();

            this->notifyEventChanged(note, *matchingNote);
            this->updateBeatRange(true);
            return true;
        }
        
        
            return false;
        
    }

    return true;
}


bool PianoLayer::insertGroup(Array<Note> &notes, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new NotesGroupInsertAction(*this->getProject(),
                                                                 this->getLayerIdAsString(),
                                                                 notes));
    }
    else
    {
        for (int i = 0; i < notes.size(); ++i)
        {
            const Note &note = notes.getUnchecked(i);
            auto storedNote = new Note(this, note);
            
            this->midiEvents.add(storedNote); // sorted later
            this->notesHashTable.set(note, storedNote);
            this->notifyEventAdded(*storedNote);
        }

        this->sort();
        this->updateBeatRange(true);
    }

    return true;
}

bool PianoLayer::removeGroup(Array<Note> &notes, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new NotesGroupRemoveAction(*this->getProject(),
                                                                 this->getLayerIdAsString(),
                                                                 notes));
    }
    else
    {
        for (int i = 0; i < notes.size(); ++i)
        {
            const Note &note = notes.getUnchecked(i);

            if (Note *matchingNote = this->notesHashTable[note])
            {
                this->notifyEventRemoved(*matchingNote);
                
                const int matchingNoteIndex = this->indexOfSorted(matchingNote);
                this->midiEvents.remove(matchingNoteIndex, true);
                //this->midiEvents.removeObject(matchingNote);
                
                this->notesHashTable.remove(note);
            }
        }

        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
    }

    return true;
}

bool PianoLayer::changeGroup(Array<Note> &notesBefore,
                             Array<Note> &notesAfter,
                             bool undoable)
{
    jassert(notesBefore.size() == notesAfter.size());

    if (undoable)
    {
        this->getUndoStack()->perform(new NotesGroupChangeAction(*this->getProject(),
                                                                 this->getLayerIdAsString(),
                                                                 notesBefore,
                                                                 notesAfter));
    }
    else
    {
        for (int i = 0; i < notesBefore.size(); ++i)
        {
            const Note &note = notesBefore.getUnchecked(i);
            const Note &newNote = notesAfter.getUnchecked(i);

            if (Note *matchingNote = this->notesHashTable[note])
            {
                (*matchingNote) = newNote;

                this->notesHashTable.set(newNote, matchingNote);
                this->notifyEventChanged(note, *matchingNote);
            }
        }

        this->sort();
        this->updateBeatRange(true);
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Batch operations
//===----------------------------------------------------------------------===//

void PianoLayer::transposeAll(int keyDelta, bool shouldCheckpoint)
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

float PianoLayer::getLastBeat() const
{
    // todo fix
    // сейчас тут скрыт один баг: последний бит считается по последнему событию
    // но могут быть и предыдущие, которые длиннее его
    // в общем, я пока закрою глаза на это

    if (this->midiEvents.size() == 0)
    {
        return -FLT_MAX;
    }

    const Note &note = static_cast<const Note &>(*this->midiEvents.getUnchecked(this->midiEvents.size() - 1));
    return note.getBeat() + note.getLength();
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *PianoLayer::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::track);
    xml->setAttribute("col", this->getColour().toString());
    xml->setAttribute("mute", this->getMuteStateAsString());
    xml->setAttribute("channel", this->getChannel());
    xml->setAttribute("instrument", this->getInstrumentId());
    xml->setAttribute("cc", this->getControllerNumber());
    xml->setAttribute("id", this->getLayerId().toString());

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        xml->prependChildElement(event->serialize()); // todo test
        //xml->addChildElement(event->serialize());
    }

    return xml;
}

void PianoLayer::deserialize(const XmlElement &xml)
{
    //this->reset(); // this will send change notifications
    this->midiEvents.clear();
    this->notesHashTable.clear();

    const XmlElement *mainSlot = (xml.getTagName() == Serialization::Core::track) ?
                                 &xml : xml.getChildByName(Serialization::Core::track);

    if (mainSlot == nullptr)
    { return; }

    this->colour = (Colour::fromString(xml.getStringAttribute("col")));
    this->channel = (xml.getIntAttribute("channel", this->getChannel()));
    this->instrumentId = (xml.getStringAttribute("instrument", this->getInstrumentId()));
    this->controllerNumber = (xml.getIntAttribute("cc", this->getControllerNumber()));
    this->layerId = Uuid(xml.getStringAttribute("id", this->getLayerId().toString()));
    this->muted = MidiLayer::isMuted(xml.getStringAttribute("mute"));

    float lastBeat = 0;
    float firstBeat = 0;

    forEachXmlChildElementWithTagName(*mainSlot, e, Serialization::Core::note)
    {
        auto note = new Note(this);
        note->deserialize(*e);

        //this->midiEvents.addSorted(*note, note); // sorted later
        this->midiEvents.add(note);

        const float noteEnd = (note->getBeat() + note->getLength());
        lastBeat = jmax(lastBeat, noteEnd);
        firstBeat = jmin(firstBeat, note->getBeat());

        this->notesHashTable.set(*note, note);
    }

    this->sort();
    this->updateBeatRange(false);
    this->notifyLayerChanged();
}

void PianoLayer::reset()
{
    this->midiEvents.clear();
    this->notesHashTable.clear();
    this->notifyLayerChanged();
}

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

#pragma once

#include "MidiSequence.h"
#include "Note.h"

class PianoRoll;

// this is the base class for the collection of notes that can be
// transformed by SequencerOperations methods; it is inherited by Lasso
// for manual editing, and by PianoSequence for transforming entire
// sequences either by a user or by parametric modifiers
class NoteListBase
{
public:

    virtual ~NoteListBase() = default;

    virtual int size() const noexcept = 0;
    virtual const Note &getNoteUnchecked(int i) const = 0;

    // see the comment in Lasso class
    virtual UndoActionId generateTransactionId(int actionId) const = 0;

};

class PianoSequence final : public MidiSequence, public NoteListBase
{
public:

    PianoSequence(MidiTrack &track, ProjectEventDispatcher &dispatcher) noexcept;

    // a constructor for GeneratedSequenceBuilder
    PianoSequence(MidiTrack &track, ProjectEventDispatcher &dispatcher,
        const PianoSequence &sequenceToCopy) noexcept;

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence,
        short timeFormat, Optional<int> filterByChannel) override;

    //===------------------------------------------------------------------===//
    // Undoable track editing
    //===------------------------------------------------------------------===//
    
    MidiEvent *insert(const Note &note, const bool undoable);
    bool remove(const Note &note, const bool undoable);
    bool change(const Note &note, const Note &newNote, bool undoable);

    bool insertGroup(Array<Note> &notes, bool undoable);
    bool removeGroup(Array<Note> &notes, bool undoable);
    bool changeGroup(Array<Note> &eventsBefore,
        Array<Note> &eventsAfter, bool undoable);

    //===------------------------------------------------------------------===//
    // NoteListBase
    //===------------------------------------------------------------------===//

    int size() const noexcept override;
    const Note &getNoteUnchecked(int i) const override;
    UndoActionId generateTransactionId(int actionId) const override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    float findLastBeat() const noexcept override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoSequence);
    JUCE_DECLARE_WEAK_REFERENCEABLE(PianoSequence);
};

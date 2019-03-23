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

#pragma once

#include "MidiSequence.h"
#include "Note.h"

class PianoRoll;

class PianoSequence final : public MidiSequence
{
public:

    PianoSequence(MidiTrack &track, ProjectEventDispatcher &dispatcher) noexcept;

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence, short timeFormat) override;

    //===------------------------------------------------------------------===//
    // Undoable track editing
    //===------------------------------------------------------------------===//
    
    MidiEvent *insert(const Note &note, const bool undoable);
    bool remove(const Note &note, const bool undoable);
    bool change(const Note &note,
        const Note &newNote,
        bool undoable);

    bool insertGroup(Array<Note> &notes, bool undoable);
    bool removeGroup(Array<Note> &notes, bool undoable);
    bool changeGroup(Array<Note> &eventsBefore,
        Array<Note> &eventsAfter,
        bool undoable);

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//
    
    float getLastBeat() const noexcept override;
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoSequence);
    JUCE_DECLARE_WEAK_REFERENCEABLE(PianoSequence);
};

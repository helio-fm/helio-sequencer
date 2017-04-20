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

#include "MidiLayer.h"
#include "Note.h"

class PianoRoll;

class PianoLayer : public MidiLayer
{
public:

    explicit PianoLayer(MidiLayerOwner &parent);


    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence) override;


    //===------------------------------------------------------------------===//
    // Undoable track editing
    //===------------------------------------------------------------------===//

    void silentImport(const MidiEvent &eventToImport) override;
    
    
    MidiEvent *insert(const Note &note, const bool undoable);

    bool remove(const Note &note, const bool undoable);

    bool change(const Note &note, const Note &newNote, const bool undoable);

    bool insertGroup(Array<Note> &notes, bool undoable);
    
    bool removeGroup(Array<Note> &notes, bool undoable);
    
    bool changeGroup(Array<Note> &eventsBefore,
                     Array<Note> &eventsAfter,
                     bool undoable);

    
    //===------------------------------------------------------------------===//
    // Batch operations
    //===------------------------------------------------------------------===//
    
    void transposeAll(int keyDelta, bool shouldCheckpoint = true);
    
    
    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//
    
    float getLastBeat() const override; // overriding to set beat+length
    
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

    void reset() override;

private:

    // быстрый доступ к указателю на событие по соответствующим ему параметрам
    // todo вот прям быстрый? замени на dense_hash_map или flat_hash_map
    HashMap<Note, Note *, NoteHashFunction> notesHashTable;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoLayer);

};

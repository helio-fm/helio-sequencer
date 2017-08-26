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
#include "TimeSignatureEvent.h"

class TimeSignaturesSequence : public MidiSequence
{
public:

    explicit TimeSignaturesSequence(MidiTrack &track, ProjectEventDispatcher &dispatcher);


    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence) override;


    //===------------------------------------------------------------------===//
    // Undoable track editing
    //===------------------------------------------------------------------===//

    void silentImport(const MidiEvent &eventToImport) override;
    
    MidiEvent *insert(const TimeSignatureEvent &signatureToCopy, bool undoable);

    bool remove(const TimeSignatureEvent &signature, bool undoable);

    bool change(const TimeSignatureEvent &signature, const TimeSignatureEvent &newSignature, bool undoable);

    bool insertGroup(Array<TimeSignatureEvent> &signatures, bool undoable);
    
    bool removeGroup(Array<TimeSignatureEvent> &signatures, bool undoable);
    
    bool changeGroup(Array<TimeSignatureEvent> &signaturesBefore,
                     Array<TimeSignatureEvent> &signaturesAfter,
                     bool undoable);


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

    void reset() override;

private:

    // быстрый доступ к указателю на событие по соответствующим ему параметрам
    HashMap<TimeSignatureEvent, TimeSignatureEvent *, TimeSignatureEventHashFunction> signaturesHashTable;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignaturesSequence);

};

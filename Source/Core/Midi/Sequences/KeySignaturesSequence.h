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
#include "KeySignatureEvent.h"
#include "Temperament.h"

class KeySignaturesSequence final : public MidiSequence
{
public:

    explicit KeySignaturesSequence(MidiTrack &track,
        ProjectEventDispatcher &dispatcher) noexcept;

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence,
        short timeFormat, Optional<int> customFilter) override;

    //===------------------------------------------------------------------===//
    // Undoable track editing
    //===------------------------------------------------------------------===//
    
    MidiEvent *insert(const KeySignatureEvent &signatureToCopy, bool undoable);
    bool remove(const KeySignatureEvent &signature, bool undoable);
    bool change(const KeySignatureEvent &signature,
        const KeySignatureEvent &newSignature,
        bool undoable);
    
    // Batch actions:
    void transposeAll(int keyDelta, Temperament::Ptr temperament, bool checkpoint);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySignaturesSequence);
    JUCE_DECLARE_WEAK_REFERENCEABLE(KeySignaturesSequence);
};

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
#include "KeySignaturesSequence.h"
#include "KeySignatureEventActions.h"
#include "SerializationKeys.h"
#include "ProjectNode.h"
#include "UndoStack.h"

KeySignaturesSequence::KeySignaturesSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void KeySignaturesSequence::importMidi(const MidiMessageSequence &sequence,
    short timeFormat, Optional<int> customFilter)
{
    this->clearUndoHistory();
    this->checkpoint();

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const auto &message = sequence.getEventPointer(i)->message;
        if (message.isKeySignatureMetaEvent())
        {
            const bool isMajor = message.isKeySignatureMajorKey();
            const int n = message.getKeySignatureNumberOfSharpsOrFlats();
            if (n >= -7 && n <= 7)
            {
                // Hard-coded circle of fifths, where indices are numbers of flats of sharps,
                // and values are the corresponding keys in chromatic scale:
                //  0    1     2    3     4    5    6     7    8     9    10    11
                // "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
                // "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"
                static const int sharpsMajor[] = { 0, 7, 2, 9, 4, 11, 6, 1 };
                static const int sharpsMinor[] = { 9, 4, 11, 6, 1, 8, 3, 10 };
                static const int flatsMajor[] = { 0, 5, 10, 3, 8, 1, 6, 11 };
                static const int flatsMinor[] = { 9, 2, 7, 0, 5, 10, 3, 8 };
                const int rootKey = (n < 0) ?
                    (isMajor ? flatsMajor[-n] : flatsMinor[-n]) :
                    (isMajor ? sharpsMajor[n] : sharpsMinor[n]);
                const float startBeat = MidiSequence::midiTicksToBeats(message.getTimeStamp(), timeFormat);
                const KeySignatureEvent signature(this,
                    isMajor ? Scale::makeNaturalMajorScale() : Scale::makeNaturalMinorScale(),
                    startBeat, rootKey);
                this->importMidiEvent<KeySignatureEvent>(signature);
            }
        }
    }

    this->updateBeatRange(false);
}


//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

MidiEvent *KeySignaturesSequence::insert(const KeySignatureEvent &eventParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventInsertAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        auto *ownedSignature = new KeySignatureEvent(this, eventParams);
        this->midiEvents.addSorted(*ownedSignature, ownedSignature);
        this->eventDispatcher.dispatchAddEvent(*ownedSignature);
        this->updateBeatRange(true);
        return ownedSignature;
    }

    return nullptr;
}

bool KeySignaturesSequence::remove(const KeySignatureEvent &signature, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventRemoveAction(*this->getProject(),
                this->getTrackId(), signature));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(signature, &signature);
        if (index >= 0)
        {
            auto *removedEvent = this->midiEvents.getUnchecked(index);
            this->eventDispatcher.dispatchRemoveEvent(*removedEvent);
            this->midiEvents.remove(index, true);
            this->updateBeatRange(true);
            this->eventDispatcher.dispatchPostRemoveEvent(this);
            return true;
        }
        
        return false;
    }

    return true;
}

bool KeySignaturesSequence::change(const KeySignatureEvent &oldParams,
    const KeySignatureEvent &newParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
        if (index >= 0)
        {
            auto *changedEvent = static_cast<KeySignatureEvent *>(this->midiEvents.getUnchecked(index));
            changedEvent->applyChanges(newParams);
            this->midiEvents.remove(index, false);
            this->midiEvents.addSorted(*changedEvent, changedEvent);
            this->eventDispatcher.dispatchChangeEvent(oldParams, *changedEvent);
            this->updateBeatRange(true);
            return true;
        }
        
        return false;
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData KeySignaturesSequence::serialize() const
{
    SerializedData tree(Serialization::Midi::keySignatures);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const auto *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}

void KeySignaturesSequence::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root =
        data.hasType(Serialization::Midi::keySignatures) ?
        data : data.getChildWithName(Serialization::Midi::keySignatures);

    if (!root.isValid())
    {
        return;
    }

    float lastBeat = 0;
    float firstBeat = 0;

    forEachChildWithType(root, e, Serialization::Midi::keySignature)
    {
        auto *signature = new KeySignatureEvent(this);
        signature->deserialize(e);
        
        this->midiEvents.add(signature); // sorted later

        lastBeat = jmax(lastBeat, signature->getBeat());
        firstBeat = jmin(firstBeat, signature->getBeat());

        this->usedEventIds.insert(signature->getId());
    }

    this->sort<KeySignatureEvent>();
    this->updateBeatRange(false);
}

void KeySignaturesSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
}

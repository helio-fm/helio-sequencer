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
#include "KeySignaturesSequence.h"
#include "KeySignatureEventActions.h"
#include "SerializationKeys.h"
#include "UndoStack.h"

KeySignaturesSequence::KeySignaturesSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) :
    MidiSequence(track, dispatcher)
{
}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void KeySignaturesSequence::importMidi(const MidiMessageSequence &sequence)
{
    this->clearUndoHistory();
    this->checkpoint();
    this->reset();

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const MidiMessage &message = sequence.getEventPointer(i)->message;
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
                    (isMajor ? flatsMajor[n] : flatsMinor[n]) :
                    (isMajor ? sharpsMajor[n] : sharpsMinor[n]);
                const double startTimestamp = message.getTimeStamp() / MIDI_IMPORT_SCALE;
                const KeySignatureEvent signature(this,
                    float(startTimestamp), KEY_C5 + rootKey,
                    isMajor ? Scale::getNaturalMajorScale() : Scale::getNaturalMiniorScale());
                this->silentImport(signature);
            }
        }
    }

    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}


//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void KeySignaturesSequence::silentImport(const MidiEvent &eventToImport)
{
    const KeySignatureEvent &signature = static_cast<const KeySignatureEvent &>(eventToImport);

    if (this->signaturesHashTable.contains(signature))
    {
        return;
    }

    KeySignatureEvent *storedSignature = new KeySignatureEvent(this);
    *storedSignature = signature;
    
    this->midiEvents.addSorted(*storedSignature, storedSignature);
    this->signaturesHashTable.set(signature, storedSignature);
    
    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

MidiEvent *KeySignaturesSequence::insert(const KeySignatureEvent &signature, bool undoable)
{
    if (this->signaturesHashTable.contains(signature))
    {
        return nullptr;
    }

    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventInsertAction(*this->getProject(),
                this->getTrackId(), signature));
    }
    else
    {
        KeySignatureEvent *storedSignature = new KeySignatureEvent(this);
        *storedSignature = signature;
        
        this->midiEvents.addSorted(*storedSignature, storedSignature);
        this->signaturesHashTable.set(signature, storedSignature);

        this->notifyEventAdded(*storedSignature);
        this->updateBeatRange(true);

        return storedSignature;
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
        if (KeySignatureEvent *matchingSignature = this->signaturesHashTable[signature])
        {
            this->notifyEventRemoved(*matchingSignature);
            this->midiEvents.removeObject(matchingSignature);
            this->signaturesHashTable.removeValue(matchingSignature);
            this->updateBeatRange(true);
            this->notifyEventRemovedPostAction();
            return true;
        }
        
        
            return false;
        
    }

    return true;
}

bool KeySignaturesSequence::change(const KeySignatureEvent &signature,
    const KeySignatureEvent &newSignature, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventChangeAction(*this->getProject(),
                this->getTrackId(), signature, newSignature));
    }
    else
    {
        if (KeySignatureEvent *matchingSignature = this->signaturesHashTable[signature])
        {
            (*matchingSignature) = newSignature;
            this->signaturesHashTable.set(newSignature, matchingSignature);
            this->sort();

            this->notifyEventChanged(signature, *matchingSignature);
            this->updateBeatRange(true);
            return true;
        }
        
        return false;
    }

    return true;
}

bool KeySignaturesSequence::insertGroup(Array<KeySignatureEvent> &signatures, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventsGroupInsertAction(*this->getProject(),
                this->getTrackId(), signatures));
    }
    else
    {
        for (int i = 0; i < signatures.size(); ++i)
        {
            const KeySignatureEvent &signature = signatures.getUnchecked(i);
            KeySignatureEvent *storedSignature = new KeySignatureEvent(this);
            *storedSignature = signature;
            
            this->midiEvents.add(storedSignature); // sorted later
            this->signaturesHashTable.set(signature, storedSignature);
            this->notifyEventAdded(*storedSignature);
        }
        
        this->sort();
        this->updateBeatRange(true);
    }
    
    return true;
}

bool KeySignaturesSequence::removeGroup(Array<KeySignatureEvent> &signatures, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventsGroupRemoveAction(*this->getProject(),
                this->getTrackId(), signatures));
    }
    else
    {
        for (int i = 0; i < signatures.size(); ++i)
        {
            const KeySignatureEvent &signature = signatures.getUnchecked(i);
            
            if (KeySignatureEvent *matchingSignature = this->signaturesHashTable[signature])
            {
                this->notifyEventRemoved(*matchingSignature);
                this->midiEvents.removeObject(matchingSignature);
                this->signaturesHashTable.removeValue(matchingSignature);
            }
        }
        
        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
    }
    
    return true;
}

bool KeySignaturesSequence::changeGroup(Array<KeySignatureEvent> &signaturesBefore,
                                      Array<KeySignatureEvent> &signaturesAfter,
                                      bool undoable)
{
    jassert(signaturesBefore.size() == signaturesAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventsGroupChangeAction(*this->getProject(),
                this->getTrackId(), signaturesBefore, signaturesAfter));
    }
    else
    {
        for (int i = 0; i < signaturesBefore.size(); ++i)
        {
            // doing this sucks
            //this->change(signaturesBefore[i], signaturesAfter[i], false);
            
            const KeySignatureEvent &signature = signaturesBefore.getUnchecked(i);
            const KeySignatureEvent &newSignature = signaturesAfter.getUnchecked(i);
            
            if (KeySignatureEvent *matchingSignature = this->signaturesHashTable[signature])
            {
                (*matchingSignature) = newSignature;
                
                this->signaturesHashTable.set(newSignature, matchingSignature);
                this->notifyEventChanged(signature, *matchingSignature);
            }
        }

        this->sort();
        this->updateBeatRange(true);
    }

    return true;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *KeySignaturesSequence::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::keySignatures);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        xml->prependChildElement(event->serialize()); // todo test
        //xml->addChildElement(event->serialize());
    }

    return xml;
}

void KeySignaturesSequence::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *mainSlot = (xml.getTagName() == Serialization::Core::keySignatures) ?
                                 &xml : xml.getChildByName(Serialization::Core::keySignatures);

    if (mainSlot == nullptr)
    {
        return;
    }

    float lastBeat = 0;
    float firstBeat = 0;

    forEachXmlChildElementWithTagName(*mainSlot, e, Serialization::Core::keySignature)
    {
        KeySignatureEvent *signature = new KeySignatureEvent(this);
        signature->deserialize(*e);
        
        //this->midiEvents.addSorted(*signature, signature); // sorted later
        this->midiEvents.add(signature);

        lastBeat = jmax(lastBeat, signature->getBeat());
        firstBeat = jmin(firstBeat, signature->getBeat());

        this->signaturesHashTable.set(*signature, signature);
    }

    this->sort();
    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

void KeySignaturesSequence::reset()
{
    this->midiEvents.clear();
    this->signaturesHashTable.clear();
    this->invalidateSequenceCache();
}

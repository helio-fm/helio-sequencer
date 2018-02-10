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
#include "ProjectTreeItem.h"
#include "UndoStack.h"

KeySignaturesSequence::KeySignaturesSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) :
    MidiSequence(track, dispatcher) {}

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
    const KeySignatureEvent &signature =
        static_cast<const KeySignatureEvent &>(eventToImport);
    jassert(signature.isValid());

    if (this->usedEventIds.contains(signature.getId()))
    {
        return;
    }

    const auto storedSignature = new KeySignatureEvent(this, signature);
    
    this->midiEvents.addSorted(*storedSignature, storedSignature);
    this->usedEventIds.insert(storedSignature->getId());

    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

MidiEvent *KeySignaturesSequence::insert(const KeySignatureEvent &eventParams, bool undoable)
{
    if (this->usedEventIds.contains(eventParams.getId()))
    {
        return nullptr;
    }

    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventInsertAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        const auto ownedSignature = new KeySignatureEvent(this, eventParams);
        this->midiEvents.addSorted(*ownedSignature, ownedSignature);
        this->usedEventIds.insert(ownedSignature->getId());
        this->notifyEventAdded(*ownedSignature);
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
            MidiEvent *const removedEvent = this->midiEvents[index];
            this->notifyEventRemoved(*removedEvent);
            this->usedEventIds.erase(removedEvent->getId());
            this->midiEvents.remove(index, true);
            this->updateBeatRange(true);
            this->notifyEventRemovedPostAction();
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
            const auto changedEvent = static_cast<KeySignatureEvent *>(this->midiEvents[index]);
            changedEvent->applyChanges(newParams);
            this->midiEvents.remove(index, false);
            this->midiEvents.addSorted(*changedEvent, changedEvent);
            this->notifyEventChanged(oldParams, *changedEvent);
            this->updateBeatRange(true);
            return true;
        }
        
        return false;
    }

    return true;
}

bool KeySignaturesSequence::insertGroup(Array<KeySignatureEvent> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventsGroupInsertAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const KeySignatureEvent &eventParams = group.getUnchecked(i);
            const auto ownedEvent = new KeySignatureEvent(this, eventParams);
            this->midiEvents.addSorted(*ownedEvent, ownedEvent);
            this->usedEventIds.insert(ownedEvent->getId());
            this->notifyEventAdded(*ownedEvent);
        }
        
        this->updateBeatRange(true);
    }
    
    return true;
}

bool KeySignaturesSequence::removeGroup(Array<KeySignatureEvent> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventsGroupRemoveAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const KeySignatureEvent &signature = group.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(signature, &signature);
            if (index >= 0)
            {
                const auto removedEvent = this->midiEvents[index];
                this->notifyEventRemoved(*removedEvent);
                this->usedEventIds.erase(removedEvent->getId());
                this->midiEvents.remove(index, true);
            }
        }
        
        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
    }
    
    return true;
}

bool KeySignaturesSequence::changeGroup(Array<KeySignatureEvent> &groupBefore,
    Array<KeySignatureEvent> &groupAfter, bool undoable)
{
    jassert(groupBefore.size() == groupAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new KeySignatureEventsGroupChangeAction(*this->getProject(),
                this->getTrackId(), groupBefore, groupAfter));
    }
    else
    {
        for (int i = 0; i < groupBefore.size(); ++i)
        {
            const KeySignatureEvent &oldParams = groupBefore.getUnchecked(i);
            const KeySignatureEvent &newParams = groupAfter.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
            if (index >= 0)
            {
                const auto changedEvent = static_cast<KeySignatureEvent *>(this->midiEvents[index]);
                changedEvent->applyChanges(newParams);
                this->midiEvents.remove(index, false);
                this->midiEvents.addSorted(*changedEvent, changedEvent);
                this->notifyEventChanged(oldParams, *changedEvent);
            }
        }

        this->updateBeatRange(true);
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree KeySignaturesSequence::serialize() const
{
    ValueTree tree(Serialization::Core::keySignatures);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        tree.addChild(event->serialize());
    }

    return tree;
}

void KeySignaturesSequence::deserialize(const ValueTree &tree)
{
    this->reset();

    const XmlElement *root =
        (tree.getTagName() == Serialization::Core::keySignatures) ?
        &tree : tree.getChildByName(Serialization::Core::keySignatures);

    if (root == nullptr)
    {
        return;
    }

    float lastBeat = 0;
    float firstBeat = 0;

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::keySignature)
    {
        auto signature = new KeySignatureEvent(this);
        signature->deserialize(*e);
        
        this->midiEvents.add(signature); // sorted later

        lastBeat = jmax(lastBeat, signature->getBeat());
        firstBeat = jmin(firstBeat, signature->getBeat());

        this->usedEventIds.insert(signature->getId());
    }

    this->sort();
    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

void KeySignaturesSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
    this->invalidateSequenceCache();
}

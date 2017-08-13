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
#include "TimeSignaturesSequence.h"
#include "Note.h"
#include "TimeSignatureEventActions.h"
#include "SerializationKeys.h"
#include "UndoStack.h"


TimeSignaturesSequence::TimeSignaturesSequence(ProjectEventDispatcher &parent) : MidiSequence(parent)
{
}


//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void TimeSignaturesSequence::importMidi(const MidiMessageSequence &sequence)
{
    this->clearUndoHistory();
    this->checkpoint();
    this->reset();

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const MidiMessage &message = sequence.getEventPointer(i)->message;

        if (message.isTimeSignatureMetaEvent())
        {
            int numerator = 0;
            int denominator = 0;
            message.getTimeSignatureInfo(numerator, denominator);
            const double startTimestamp = message.getTimeStamp() / MIDI_IMPORT_SCALE;
            const float beat = float(startTimestamp);
            const TimeSignatureEvent signature(this, beat, numerator, denominator);
            this->silentImport(signature);
        }
    }

    this->notifyBeatRangeChanged();
    this->notifyLayerChanged();
}


//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void TimeSignaturesSequence::silentImport(const MidiEvent &eventToImport)
{
    const TimeSignatureEvent &signature = static_cast<const TimeSignatureEvent &>(eventToImport);

    if (this->signaturesHashTable.contains(signature))
    {
        return;
    }

    TimeSignatureEvent *storedSignature = new TimeSignatureEvent(this);
    *storedSignature = signature;
    
    this->midiEvents.addSorted(*storedSignature, storedSignature);
    this->signaturesHashTable.set(signature, storedSignature);
    
    this->updateBeatRange(false);
}

MidiEvent *TimeSignaturesSequence::insert(const TimeSignatureEvent &signature, bool undoable)
{
    if (this->signaturesHashTable.contains(signature))
    {
        return nullptr;
    }

    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventInsertAction(*this->getProject(),
                                                       this->getLayerIdAsString(),
                                                       signature));
    }
    else
    {
        TimeSignatureEvent *storedSignature = new TimeSignatureEvent(this);
        *storedSignature = signature;
        
        this->midiEvents.addSorted(*storedSignature, storedSignature);
        this->signaturesHashTable.set(signature, storedSignature);

        this->notifyEventAdded(*storedSignature);
        this->updateBeatRange(true);

        return storedSignature;
    }

    return nullptr;
}

bool TimeSignaturesSequence::remove(const TimeSignatureEvent &signature, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventRemoveAction(*this->getProject(),
                                                       this->getLayerIdAsString(),
                                                       signature));
    }
    else
    {
        if (TimeSignatureEvent *matchingSignature = this->signaturesHashTable[signature])
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

bool TimeSignaturesSequence::change(const TimeSignatureEvent &signature,
                                 const TimeSignatureEvent &newSignature,
                                 bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventChangeAction(*this->getProject(),
                                                       this->getLayerIdAsString(),
                                                       signature,
                                                       newSignature));
    }
    else
    {
        if (TimeSignatureEvent *matchingSignature = this->signaturesHashTable[signature])
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

bool TimeSignaturesSequence::insertGroup(Array<TimeSignatureEvent> &signatures, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventsGroupInsertAction(*this->getProject(),
                                                             this->getLayerIdAsString(),
                                                             signatures));
    }
    else
    {
        for (int i = 0; i < signatures.size(); ++i)
        {
            const TimeSignatureEvent &signature = signatures.getUnchecked(i);
            TimeSignatureEvent *storedSignature = new TimeSignatureEvent(this);
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

bool TimeSignaturesSequence::removeGroup(Array<TimeSignatureEvent> &signatures, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventsGroupRemoveAction(*this->getProject(),
                                                             this->getLayerIdAsString(),
                                                             signatures));
    }
    else
    {
        for (int i = 0; i < signatures.size(); ++i)
        {
            const TimeSignatureEvent &signature = signatures.getUnchecked(i);
            
            if (TimeSignatureEvent *matchingSignature = this->signaturesHashTable[signature])
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

bool TimeSignaturesSequence::changeGroup(Array<TimeSignatureEvent> &signaturesBefore,
                                      Array<TimeSignatureEvent> &signaturesAfter,
                                      bool undoable)
{
    jassert(signaturesBefore.size() == signaturesAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventsGroupChangeAction(*this->getProject(),
                                                             this->getLayerIdAsString(),
                                                             signaturesBefore,
                                                             signaturesAfter));
    }
    else
    {
        for (int i = 0; i < signaturesBefore.size(); ++i)
        {
            // doing this sucks
            //this->change(signaturesBefore[i], signaturesAfter[i], false);
            
            const TimeSignatureEvent &signature = signaturesBefore.getUnchecked(i);
            const TimeSignatureEvent &newSignature = signaturesAfter.getUnchecked(i);
            
            if (TimeSignatureEvent *matchingSignature = this->signaturesHashTable[signature])
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

XmlElement *TimeSignaturesSequence::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::timeSignatures);
    
    xml->setAttribute("col", this->getColour().toString());
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

void TimeSignaturesSequence::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *mainSlot = (xml.getTagName() == Serialization::Core::timeSignatures) ?
                                 &xml : xml.getChildByName(Serialization::Core::timeSignatures);

    if (mainSlot == nullptr)
    {
        return;
    }

    this->setColour(Colour::fromString(xml.getStringAttribute("col")));
    this->setChannel(xml.getIntAttribute("channel", this->getChannel()));
    this->setInstrumentId(xml.getStringAttribute("instrument", this->getInstrumentId()));
    this->setControllerNumber(xml.getIntAttribute("cc", this->getControllerNumber()));
    this->setLayerId(xml.getStringAttribute("id", this->getLayerId().toString()));

    float lastBeat = 0;
    float firstBeat = 0;

    forEachXmlChildElementWithTagName(*mainSlot, e, Serialization::Core::timeSignature)
    {
        TimeSignatureEvent *signature = new TimeSignatureEvent(this);
        signature->deserialize(*e);
        
        //this->midiEvents.addSorted(*signature, signature); // sorted later
        this->midiEvents.add(signature);

        lastBeat = jmax(lastBeat, signature->getBeat());
        firstBeat = jmin(firstBeat, signature->getBeat());

        this->signaturesHashTable.set(*signature, signature);
    }

    this->sort();
    this->updateBeatRange(false);
    this->notifyLayerChanged();
}

void TimeSignaturesSequence::reset()
{
    this->midiEvents.clear();
    this->signaturesHashTable.clear();
    this->notifyLayerChanged();
}

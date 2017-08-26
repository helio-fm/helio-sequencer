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
#include "AnnotationsSequence.h"
#include "Note.h"
#include "AnnotationEventActions.h"
#include "SerializationKeys.h"
#include "UndoStack.h"


AnnotationsSequence::AnnotationsSequence(MidiTrack &track, 
    ProjectEventDispatcher &dispatcher) :
    MidiSequence(track, dispatcher)
{
}


//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void AnnotationsSequence::importMidi(const MidiMessageSequence &sequence)
{
    this->clearUndoHistory();
    this->checkpoint();
    this->reset();

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const MidiMessage &message = sequence.getEventPointer(i)->message;

        if (message.isTextMetaEvent())
        {
            const String text = message.getTextFromTextMetaEvent();
            const double startTimestamp = message.getTimeStamp() / MIDI_IMPORT_SCALE;
            const float beat = float(startTimestamp);
            
            // bottleneck warning!
            const AnnotationEvent annotation(this, beat, text, Colours::white);
            this->silentImport(annotation);
        }
    }

    this->notifyBeatRangeChanged();
    this->notifySequenceChanged();
}


//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void AnnotationsSequence::silentImport(const MidiEvent &eventToImport)
{
    const AnnotationEvent &annotation = static_cast<const AnnotationEvent &>(eventToImport);

    if (this->annotationsHashTable.contains(annotation))
    {
        return;
    }

    AnnotationEvent *storedAnnotation = new AnnotationEvent(this);
    *storedAnnotation = annotation;
    
    this->midiEvents.addSorted(*storedAnnotation, storedAnnotation);
    //this->midiEvents.add(storedAnnotation);
    this->annotationsHashTable.set(annotation, storedAnnotation);
    
    this->updateBeatRange(false);
}

MidiEvent *AnnotationsSequence::insert(const AnnotationEvent &annotation, bool undoable)
{
    if (this->annotationsHashTable.contains(annotation))
    {
        return nullptr;
    }

    if (undoable)
    {
        this->getUndoStack()->perform(new AnnotationEventInsertAction(*this->getProject(),
            this->getTrackId(), annotation));
    }
    else
    {
        AnnotationEvent *storedAnnotation = new AnnotationEvent(this);
        *storedAnnotation = annotation;
        
        this->midiEvents.addSorted(*storedAnnotation, storedAnnotation);
        //this->midiEvents.add(storedAnnotation);

        this->annotationsHashTable.set(annotation, storedAnnotation);

        this->notifyEventAdded(*storedAnnotation);
        this->updateBeatRange(true);

        return storedAnnotation;
    }

    return nullptr;
}

bool AnnotationsSequence::remove(const AnnotationEvent &annotation, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AnnotationEventRemoveAction(*this->getProject(),
                                                                      this->getTrackId(),
                                                                      annotation));
    }
    else
    {
        if (AnnotationEvent *matchingAnnotation = this->annotationsHashTable[annotation])
        {
            this->notifyEventRemoved(*matchingAnnotation);
            this->midiEvents.removeObject(matchingAnnotation);
            this->annotationsHashTable.removeValue(matchingAnnotation);
            this->updateBeatRange(true);
            this->notifyEventRemovedPostAction();
            return true;
        }
        
        return false;
    }

    return true;
}

bool AnnotationsSequence::change(const AnnotationEvent &annotation,
                              const AnnotationEvent &newAnnotation,
                              bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AnnotationEventChangeAction(*this->getProject(),
                                                                      this->getTrackId(),
                                                                      annotation,
                                                                      newAnnotation));
    }
    else
    {
        if (AnnotationEvent *matchingAnnotation = this->annotationsHashTable[annotation])
        {
            (*matchingAnnotation) = newAnnotation;
            this->annotationsHashTable.set(newAnnotation, matchingAnnotation);
            this->sort();

            this->notifyEventChanged(annotation, *matchingAnnotation);
            this->updateBeatRange(true);
            return true;
        }
        
        return false;
    }

    return true;
}

bool AnnotationsSequence::insertGroup(Array<AnnotationEvent> &annotations, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AnnotationEventsGroupInsertAction(*this->getProject(),
                                                                            this->getTrackId(),
                                                                            annotations));
    }
    else
    {
        for (int i = 0; i < annotations.size(); ++i)
        {
            const AnnotationEvent &annotation = annotations.getUnchecked(i);
            AnnotationEvent *storedAnnotation = new AnnotationEvent(this);
            *storedAnnotation = annotation;
            
            this->midiEvents.add(storedAnnotation); // sorted later
            this->annotationsHashTable.set(annotation, storedAnnotation);
            this->notifyEventAdded(*storedAnnotation);
        }
        
        this->sort();
        this->updateBeatRange(true);
    }
    
    return true;
}

bool AnnotationsSequence::removeGroup(Array<AnnotationEvent> &annotations, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AnnotationEventsGroupRemoveAction(*this->getProject(),
                                                                            this->getTrackId(),
                                                                            annotations));
    }
    else
    {
        for (int i = 0; i < annotations.size(); ++i)
        {
            const AnnotationEvent &annotation = annotations.getUnchecked(i);
            
            if (AnnotationEvent *matchingAnnotation = this->annotationsHashTable[annotation])
            {
                this->notifyEventRemoved(*matchingAnnotation);
                this->midiEvents.removeObject(matchingAnnotation);
                this->annotationsHashTable.removeValue(matchingAnnotation);
            }
        }
        
        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
    }
    
    return true;
}

bool AnnotationsSequence::changeGroup(Array<AnnotationEvent> &annotationsBefore,
                                   Array<AnnotationEvent> &annotationsAfter,
                                   bool undoable)
{
    jassert(annotationsBefore.size() == annotationsAfter.size());

    if (undoable)
    {
        this->getUndoStack()->perform(new AnnotationEventsGroupChangeAction(*this->getProject(),
                                                                            this->getTrackId(),
                                                                            annotationsBefore,
                                                                            annotationsAfter));
    }
    else
    {
        for (int i = 0; i < annotationsBefore.size(); ++i)
        {
            // doing this sucks
            //this->change(annotationsBefore[i], annotationsAfter[i], false);
            
            const AnnotationEvent &annotation = annotationsBefore.getUnchecked(i);
            const AnnotationEvent &newAnnotation = annotationsAfter.getUnchecked(i);
            
            if (AnnotationEvent *matchingAnnotation = this->annotationsHashTable[annotation])
            {
                (*matchingAnnotation) = newAnnotation;
                
                this->annotationsHashTable.set(newAnnotation, matchingAnnotation);
                this->notifyEventChanged(annotation, *matchingAnnotation);
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

XmlElement *AnnotationsSequence::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::annotations);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        xml->prependChildElement(event->serialize());
    }

    return xml;
}

void AnnotationsSequence::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *mainSlot = (xml.getTagName() == Serialization::Core::annotations) ?
                                 &xml : xml.getChildByName(Serialization::Core::annotations);

    if (mainSlot == nullptr)
    {
        return;
    }

    float lastBeat = 0;
    float firstBeat = 0;

    forEachXmlChildElementWithTagName(*mainSlot, e, Serialization::Core::annotation)
    {
        AnnotationEvent *annotation = new AnnotationEvent(this);
        annotation->deserialize(*e);
        
        //this->midiEvents.addSorted(*annotation, annotation); // sorted later
        this->midiEvents.add(annotation);

        lastBeat = jmax(lastBeat, annotation->getBeat());
        firstBeat = jmin(firstBeat, annotation->getBeat());

        this->annotationsHashTable.set(*annotation, annotation);
    }

    this->sort();
    this->updateBeatRange(false);
    this->notifySequenceChanged();
}

void AnnotationsSequence::reset()
{
    this->midiEvents.clear();
    this->annotationsHashTable.clear();
    this->notifySequenceChanged();
}

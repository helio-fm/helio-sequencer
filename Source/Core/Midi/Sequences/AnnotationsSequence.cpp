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
#include "ProjectTreeItem.h"
#include "UndoStack.h"

AnnotationsSequence::AnnotationsSequence(MidiTrack &track, 
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

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

    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void AnnotationsSequence::silentImport(const MidiEvent &eventToImport)
{
    const AnnotationEvent &annotation =
        static_cast<const AnnotationEvent &>(eventToImport);
    jassert(annotation.isValid());

    if (this->usedEventIds.contains(annotation.getId()))
    {
        jassertfalse;
        return;
    }

    AnnotationEvent *storedAnnotation = new AnnotationEvent(this, annotation);
    
    this->midiEvents.addSorted(*storedAnnotation, storedAnnotation);
    this->usedEventIds.insert(storedAnnotation->getId());

    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

MidiEvent *AnnotationsSequence::insert(const AnnotationEvent &eventParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AnnotationEventInsertAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        const auto ownedEvent = new AnnotationEvent(this, eventParams);
        this->midiEvents.addSorted(*ownedEvent, ownedEvent);
        this->notifyEventAdded(*ownedEvent);
        this->updateBeatRange(true);
        return ownedEvent;
    }

    return nullptr;
}

bool AnnotationsSequence::remove(const AnnotationEvent &eventParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AnnotationEventRemoveAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(eventParams, &eventParams);
        if (index >= 0)
        {
            MidiEvent *const removedEvent = this->midiEvents[index];
            jassert(removedEvent->isValid());
            this->notifyEventRemoved(*removedEvent);
            this->midiEvents.remove(index, true);
            this->updateBeatRange(true);
            this->notifyEventRemovedPostAction();
            return true;
        }
        
        return false;
    }

    return true;
}

bool AnnotationsSequence::change(const AnnotationEvent &oldParams,
    const AnnotationEvent &newParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AnnotationEventChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
        if (index >= 0)
        {
            const auto changedEvent = static_cast<AnnotationEvent *>(this->midiEvents[index]);
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

bool AnnotationsSequence::insertGroup(Array<AnnotationEvent> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AnnotationEventsGroupInsertAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const AnnotationEvent &eventParams = group.getUnchecked(i);
            const auto ownedEvent = new AnnotationEvent(this, eventParams);
            jassert(ownedEvent->isValid());
            this->midiEvents.addSorted(*ownedEvent, ownedEvent);
            this->notifyEventAdded(*ownedEvent);
        }
        
        this->updateBeatRange(true);
    }
    
    return true;
}

bool AnnotationsSequence::removeGroup(Array<AnnotationEvent> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AnnotationEventsGroupRemoveAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const AnnotationEvent &annotation = group.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(annotation, &annotation);
            if (index >= 0)
            {
                const auto removedEvent = this->midiEvents[index];
                this->notifyEventRemoved(*removedEvent);
                this->midiEvents.remove(index, true);
            }
        }
        
        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
    }
    
    return true;
}

bool AnnotationsSequence::changeGroup(Array<AnnotationEvent> &groupBefore,
    Array<AnnotationEvent> &groupAfter, bool undoable)
{
    jassert(groupBefore.size() == groupAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new AnnotationEventsGroupChangeAction(*this->getProject(),
                this->getTrackId(), groupBefore, groupAfter));
    }
    else
    {
        for (int i = 0; i < groupBefore.size(); ++i)
        {
            const AnnotationEvent &oldParams = groupBefore.getUnchecked(i);
            const AnnotationEvent &newParams = groupAfter.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
            if (index >= 0)
            {
                const auto changedEvent = static_cast<AnnotationEvent *>(this->midiEvents[index]);
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
// Callbacks
//===----------------------------------------------------------------------===//

Function<void(const String &text)> AnnotationsSequence::getEventRenameCallback(const AnnotationEvent &event)
{
    return [this, event](const String &text)
    {
        this->checkpoint();
        this->change(event, event.withDescription(text), true);
    };
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree AnnotationsSequence::serialize() const
{
    ValueTree tree(Serialization::Midi::annotations);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }

    return tree;
}

void AnnotationsSequence::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root =
        tree.hasType(Serialization::Midi::annotations) ?
        tree : tree.getChildWithName(Serialization::Midi::annotations);

    if (!root.isValid())
    {
        return;
    }

    float lastBeat = 0;
    float firstBeat = 0;

    forEachValueTreeChildWithType(root, e, Serialization::Midi::annotation)
    {
        AnnotationEvent *annotation = new AnnotationEvent(this);
        annotation->deserialize(e);
        
        this->midiEvents.add(annotation); // sorted later

        lastBeat = jmax(lastBeat, annotation->getBeat());
        firstBeat = jmin(firstBeat, annotation->getBeat());

        this->usedEventIds.insert(annotation->getId());
    }

    this->sort();
    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

void AnnotationsSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
    this->invalidateSequenceCache();
}

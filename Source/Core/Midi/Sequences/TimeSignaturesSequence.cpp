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
#include "ProjectNode.h"
#include "UndoStack.h"

TimeSignaturesSequence::TimeSignaturesSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void TimeSignaturesSequence::importMidi(const MidiMessageSequence &sequence, short timeFormat)
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
            const float startBeat = MidiSequence::midiTicksToBeats(message.getTimeStamp(), timeFormat);
            const TimeSignatureEvent signature(this, startBeat, numerator, denominator);
            this->importMidiEvent<TimeSignatureEvent>(signature);
        }
    }

    this->updateBeatRange(false);
}

//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

MidiEvent *TimeSignaturesSequence::insert(const TimeSignatureEvent &eventParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventInsertAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        const auto ownedEvent = new TimeSignatureEvent(this, eventParams);
        this->midiEvents.addSorted(*ownedEvent, ownedEvent);
        this->eventDispatcher.dispatchAddEvent(*ownedEvent);
        this->updateBeatRange(true);
        return ownedEvent;
    }

    return nullptr;
}

bool TimeSignaturesSequence::remove(const TimeSignatureEvent &signature, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventRemoveAction(*this->getProject(),
                this->getTrackId(), signature));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(signature, &signature);
        if (index >= 0)
        {
            MidiEvent *const removedEvent = this->midiEvents[index];
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

bool TimeSignaturesSequence::change(const TimeSignatureEvent &oldParams,
    const TimeSignatureEvent &newParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
        if (index >= 0)
        {
            auto changedEvent = static_cast<TimeSignatureEvent *>(this->midiEvents[index]);
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

bool TimeSignaturesSequence::insertGroup(Array<TimeSignatureEvent> &signatures, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventsGroupInsertAction(*this->getProject(),
                this->getTrackId(), signatures));
    }
    else
    {
        for (int i = 0; i < signatures.size(); ++i)
        {
            const TimeSignatureEvent &eventParams = signatures.getUnchecked(i);
            const auto ownedEvent = new TimeSignatureEvent(this, eventParams);
            this->midiEvents.addSorted(*ownedEvent, ownedEvent);
            this->eventDispatcher.dispatchAddEvent(*ownedEvent);
        }
        
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
                this->getTrackId(), signatures));
    }
    else
    {
        for (int i = 0; i < signatures.size(); ++i)
        {
            const TimeSignatureEvent &signature = signatures.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(signature, &signature);
            if (index >= 0)
            {
                MidiEvent *const removedSignature = this->midiEvents[index];
                this->eventDispatcher.dispatchRemoveEvent(*removedSignature);
                this->midiEvents.remove(index, true);
            }
        }
        
        this->updateBeatRange(true);
        this->eventDispatcher.dispatchPostRemoveEvent(this);
    }
    
    return true;
}

bool TimeSignaturesSequence::changeGroup(Array<TimeSignatureEvent> &groupBefore,
    Array<TimeSignatureEvent> &groupAfter, bool undoable)
{
    jassert(groupBefore.size() == groupAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventsGroupChangeAction(*this->getProject(),
                this->getTrackId(), groupBefore, groupAfter));
    }
    else
    {
        for (int i = 0; i < groupBefore.size(); ++i)
        {
            const TimeSignatureEvent &oldParams = groupBefore.getUnchecked(i);
            const TimeSignatureEvent &newParams = groupAfter.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
            if (index >= 0)
            {
                const auto changedEvent = static_cast<TimeSignatureEvent *>(this->midiEvents[index]);
                changedEvent->applyChanges(newParams);
                this->midiEvents.remove(index, false);
                this->midiEvents.addSorted(*changedEvent, changedEvent);
                this->eventDispatcher.dispatchChangeEvent(oldParams, *changedEvent);
            }
        }

        this->updateBeatRange(true);
    }

    return true;
}

//===----------------------------------------------------------------------===//
// Callbacks
//===----------------------------------------------------------------------===//

Function<void(const String &text)> TimeSignaturesSequence::getEventChangeCallback(const TimeSignatureEvent &event)
{
    return[this, event](const String &text)
    {
        int numerator;
        int denominator;
        TimeSignatureEvent::parseString(text, numerator, denominator);
        this->checkpoint();
        this->change(event, event.withNumerator(numerator).withDenominator(denominator), true);

    };
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree TimeSignaturesSequence::serialize() const
{
    ValueTree tree(Serialization::Midi::timeSignatures);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr); // faster than addChildElement
    }

    return tree;
}

void TimeSignaturesSequence::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;

    const auto root =
        tree.hasType(Serialization::Midi::timeSignatures) ?
        tree : tree.getChildWithName(Serialization::Midi::timeSignatures);

    if (!root.isValid())
    {
        return;
    }

    float lastBeat = 0;
    float firstBeat = 0;

    forEachValueTreeChildWithType(root, e, Serialization::Midi::timeSignature)
    {
        TimeSignatureEvent *signature = new TimeSignatureEvent(this);
        signature->deserialize(e);
        
        this->midiEvents.add(signature); // sorted later

        lastBeat = jmax(lastBeat, signature->getBeat());
        firstBeat = jmin(firstBeat, signature->getBeat());

        this->usedEventIds.insert(signature->getId());
    }

    this->sort();
    this->updateBeatRange(false);
}

void TimeSignaturesSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
}

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
#include "AutomationSequence.h"
#include "AutomationEventActions.h"

#include "ProjectNode.h"
#include "MidiTrackNode.h"
#include "UndoStack.h"

AutomationSequence::AutomationSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

float AutomationSequence::getAverageControllerValue() const
{
    float result = 0.f;

    for (const auto *event : this->midiEvents)
    {
        const auto *cc = static_cast<const AutomationEvent *>(event);
        result += cc->getControllerValue();
    }

    return result / float(this->midiEvents.size());
}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void AutomationSequence::importMidi(const MidiMessageSequence &sequence,
    short timeFormat, Optional<int> filterByCV)
{
    this->clearUndoHistory();
    this->checkpoint();
    
    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const auto &message = sequence.getEventPointer(i)->message;
        const float startBeat = MidiSequence::midiTicksToBeats(message.getTimeStamp(), timeFormat);

        if (message.isController())
        {
            if (filterByCV.hasValue() &&
                message.getControllerNumber() != *filterByCV)
            {
                continue;
            }

            const int controllerValue = message.getControllerValue();
            const AutomationEvent event(this, startBeat, float(controllerValue) / 127.f);
            this->importMidiEvent<AutomationEvent>(event);
        }
        else if (message.isTempoMetaEvent())
        {
            const float controllerValue = Transport::getControllerValueByTempo(message.getTempoSecondsPerQuarterNote());
            const AutomationEvent event(this, startBeat, controllerValue);
            this->importMidiEvent<AutomationEvent>(event);
        }
    }
    
    this->updateBeatRange(false);
}

//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

MidiEvent *AutomationSequence::insert(const AutomationEvent &eventParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AutomationEventInsertAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        auto *ownedEvent = new AutomationEvent(this, eventParams);
        this->midiEvents.addSorted(*ownedEvent, ownedEvent);
        this->eventDispatcher.dispatchAddEvent(*ownedEvent);
        this->updateBeatRange(true);
        return ownedEvent;
    }

    return nullptr;
}

bool AutomationSequence::remove(const AutomationEvent &eventParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AutomationEventRemoveAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        jassert(this->midiEvents.size() > 1); // no empty automation tracks please
        const int index = this->midiEvents.indexOfSorted(eventParams, &eventParams);
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

bool AutomationSequence::change(const AutomationEvent &oldParams,
    const AutomationEvent &newParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AutomationEventChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
        if (index >= 0)
        {
            const auto changedEvent = static_cast<AutomationEvent *>(this->midiEvents[index]);
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

bool AutomationSequence::insertGroup(Array<AutomationEvent> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AutomationEventsGroupInsertAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        for (int i = 0; i < group.size(); ++i)
        {
            const auto &eventParams = group.getUnchecked(i);
            auto *ownedEvent = new AutomationEvent(this, eventParams);
            jassert(this->midiEvents.indexOfSorted(*ownedEvent, ownedEvent) == -1);
            this->midiEvents.addSorted(*ownedEvent, ownedEvent);
            this->eventDispatcher.dispatchAddEvent(*ownedEvent);
        }
        
        this->updateBeatRange(true);
    }
    
    return true;
}

bool AutomationSequence::removeGroup(Array<AutomationEvent> &group, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new AutomationEventsGroupRemoveAction(*this->getProject(),
                this->getTrackId(), group));
    }
    else
    {
        // hitting this when drawing shapes in AutiomationEditor is ok;
        // otherwise, no empty automation tracks please:
        jassert(this->midiEvents.size() > group.size());

        for (int i = 0; i < group.size(); ++i)
        {
            const AutomationEvent &autoEvent = group.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(autoEvent, &autoEvent);
            jassert(index >= 0);
            if (index >= 0)
            {
                auto *removedEvent = this->midiEvents[index];
                this->eventDispatcher.dispatchRemoveEvent(*removedEvent);
                this->midiEvents.remove(index, true);
            }
        }
        
        this->updateBeatRange(true);
        this->eventDispatcher.dispatchPostRemoveEvent(this);
    }
    
    return true;
}

bool AutomationSequence::changeGroup(const Array<AutomationEvent> groupBefore,
    const Array<AutomationEvent> groupAfter, bool undoable)
{
    jassert(groupBefore.size() == groupAfter.size());

    if (undoable)
    {
        this->getUndoStack()->
            perform(new AutomationEventsGroupChangeAction(*this->getProject(),
                this->getTrackId(), groupBefore, groupAfter));
    }
    else
    {
        for (int i = 0; i < groupBefore.size(); ++i)
        {
            const auto &oldParams = groupBefore.getUnchecked(i);
            const auto &newParams = groupAfter.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
            if (index >= 0)
            {
                auto *changedEvent = static_cast<AutomationEvent *>(this->midiEvents[index]);
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
// Serializable
//===----------------------------------------------------------------------===//

SerializedData AutomationSequence::serialize() const
{
    SerializedData tree(Serialization::Midi::automation);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        MidiEvent *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize());
    }
    
    return tree;
}

void AutomationSequence::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root =
        data.hasType(Serialization::Midi::automation) ?
        data : data.getChildWithName(Serialization::Midi::automation);

    if (!root.isValid())
    { return; }

    float firstBeat = 0;
    float lastBeat = 0;

    forEachChildWithType(root, e, Serialization::Midi::automationEvent)
    {
        auto *event = new AutomationEvent(this, 0, 0);
        event->deserialize(e);
        
        this->midiEvents.add(event); // sorted later
        
        lastBeat = jmax(lastBeat, event->getBeat());
        firstBeat = jmin(firstBeat, event->getBeat());

        this->usedEventIds.insert(event->getId());
    }

    this->sort<AutomationEvent>();
    this->updateBeatRange(false);
}

void AutomationSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
}

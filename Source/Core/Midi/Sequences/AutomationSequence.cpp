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
#include "AutomationSequence.h"
#include "AutomationEventActions.h"

#include "ProjectNode.h"
#include "ProjectListener.h"
#include "MidiTrackNode.h"
#include "UndoStack.h"

AutomationSequence::AutomationSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void AutomationSequence::importMidi(const MidiMessageSequence &sequence, short timeFormat)
{
    this->clearUndoHistory();
    this->checkpoint();
    this->reset();
    
    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const MidiMessage &message = sequence.getEventPointer(i)->message;
        if (message.isController())
        {
            const float startBeat = MidiSequence::midiTicksToBeats(message.getTimeStamp(), timeFormat);
            const int controllerValue = message.getControllerValue();
            const AutomationEvent event(this, startBeat, float(controllerValue));
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
        const auto ownedEvent = new AutomationEvent(this, eventParams);
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
            const AutomationEvent &eventParams = group.getUnchecked(i);
            auto ownedEvent = new AutomationEvent(this, eventParams);
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
        for (int i = 0; i < group.size(); ++i)
        {
            const AutomationEvent &autoEvent = group.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(autoEvent, &autoEvent);
            if (index >= 0)
            {
                const auto removedEvent = this->midiEvents[index];
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
            const AutomationEvent &oldParams = groupBefore.getUnchecked(i);
            const AutomationEvent &newParams = groupAfter.getUnchecked(i);
            const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
            if (index >= 0)
            {
                const auto changedEvent = static_cast<AutomationEvent *>(this->midiEvents[index]);
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

ValueTree AutomationSequence::serialize() const
{
    ValueTree tree(Serialization::Midi::automation);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        MidiEvent *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }
    
    return tree;
}

void AutomationSequence::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root =
        tree.hasType(Serialization::Midi::automation) ?
        tree : tree.getChildWithName(Serialization::Midi::automation);

    if (!root.isValid())
    { return; }

    float firstBeat = 0;
    float lastBeat = 0;

    forEachValueTreeChildWithType(root, e, Serialization::Midi::automationEvent)
    {
        auto event = new AutomationEvent(this, 0, 0);
        event->deserialize(e);
        
        this->midiEvents.add(event); // sorted later
        
        lastBeat = jmax(lastBeat, event->getBeat());
        firstBeat = jmin(firstBeat, event->getBeat());

        this->usedEventIds.insert(event->getId());
    }

    this->sort();
    this->updateBeatRange(false);
}

void AutomationSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
}

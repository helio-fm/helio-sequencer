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

#include "ProjectTreeItem.h"
#include "ProjectListener.h"
#include "MidiTrackTreeItem.h"
#include "UndoStack.h"

AutomationSequence::AutomationSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void AutomationSequence::importMidi(const MidiMessageSequence &sequence)
{
    this->clearUndoHistory();
    this->checkpoint();
    this->reset();
    
    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const MidiMessage &message = sequence.getEventPointer(i)->message;
        
        if (message.isController())
        {
            const double startTimestamp = message.getTimeStamp() / MIDI_IMPORT_SCALE;
            const int controllerValue = message.getControllerValue();
            const float beat = float(startTimestamp);
            
            const AutomationEvent event(this, beat, float(controllerValue));
            this->silentImport(event);
        }
    }
    
    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void AutomationSequence::silentImport(const MidiEvent &eventToImport)
{
    const AutomationEvent &autoEvent =
        static_cast<const AutomationEvent &>(eventToImport);

    if (this->usedEventIds.contains(autoEvent.getId()))
    {
        jassertfalse;
        return;
    }

    const auto storedEvent = new AutomationEvent(this, autoEvent);
    
    this->midiEvents.addSorted(*storedEvent, storedEvent);
    this->usedEventIds.insert(storedEvent->getId());

    this->updateBeatRange(false);
    this->invalidateSequenceCache();
}

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
        this->notifyEventAdded(*ownedEvent);
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
            this->notifyEventChanged(oldParams, *changedEvent);
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
            this->notifyEventAdded(*ownedEvent);
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
                this->notifyEventRemoved(*removedEvent);
                this->midiEvents.remove(index, true);
            }
        }
        
        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
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
    this->invalidateSequenceCache();
}

void AutomationSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
    this->invalidateSequenceCache();
}

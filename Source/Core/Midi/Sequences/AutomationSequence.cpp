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
    ProjectEventDispatcher &dispatcher) :
    MidiSequence(track, dispatcher)
{
}

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
    
    this->notifyBeatRangeChanged();
    this->notifySequenceChanged();
}


//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void AutomationSequence::silentImport(const MidiEvent &eventToImport)
{
    const AutomationEvent &autoEvent =
        static_cast<const AutomationEvent &>(eventToImport);

    if (this->eventsHashTable.contains(autoEvent))
    { return; }

    auto storedEvent = new AutomationEvent(this);
    *storedEvent = autoEvent;
    
    this->midiEvents.addSorted(*storedEvent, storedEvent);
    this->eventsHashTable.set(autoEvent, storedEvent);

    this->updateBeatRange(false);
}

MidiEvent *AutomationSequence::insert(const AutomationEvent &autoEvent, bool undoable)
{
    if (AutomationEvent *matchingEvent = this->eventsHashTable[autoEvent])
    {
        return nullptr; // exists
    }

    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventInsertAction(*this->getProject(),
                                                                      this->getTrackId(),
                                                                      autoEvent));
    }
    else
    {
        auto storedEvent = new AutomationEvent(this);
        *storedEvent = autoEvent;

        this->midiEvents.addSorted(*storedEvent, storedEvent);

        this->eventsHashTable.set(autoEvent, storedEvent);

        this->notifyEventAdded(*storedEvent);
        this->updateBeatRange(true);

        return storedEvent;
    }

    return nullptr;
}

bool AutomationSequence::remove(const AutomationEvent &autoEvent, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventRemoveAction(*this->getProject(),
                                                                      this->getTrackId(),
                                                                      autoEvent));
    }
    else
    {
        if (AutomationEvent *matchingEvent = this->eventsHashTable[autoEvent])
        {
            this->notifyEventRemoved(*matchingEvent);
            this->midiEvents.removeObject(matchingEvent);
            this->eventsHashTable.removeValue(matchingEvent);
            this->updateBeatRange(true);
            this->notifyEventRemovedPostAction();
            return true;
        }
        
        
            return false;
        
    }

    return true;
}

bool AutomationSequence::change(const AutomationEvent &autoEvent, const AutomationEvent &newAutoEvent, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventChangeAction(*this->getProject(),
                                                                      this->getTrackId(),
                                                                      autoEvent,
                                                                      newAutoEvent));
    }
    else
    {
        if (AutomationEvent *matchingEvent = this->eventsHashTable[autoEvent])
        {
            (*matchingEvent) = newAutoEvent;

            this->eventsHashTable.removeValue(matchingEvent);
            this->eventsHashTable.set(newAutoEvent, matchingEvent);
            
            this->sort();
            
            this->notifyEventChanged(autoEvent, *matchingEvent);
            this->updateBeatRange(true);
            return true;
        }
        
        
            return false;
        
    }

    return true;
}

bool AutomationSequence::insertGroup(Array<AutomationEvent> &events, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventsGroupInsertAction(*this->getProject(),
                                                                            this->getTrackId(),
                                                                            events));
    }
    else
    {
        for (int i = 0; i < events.size(); ++i)
        {
            const AutomationEvent &autoEvent = events.getUnchecked(i);
            auto storedEvent = new AutomationEvent(this);
            *storedEvent = autoEvent;
            
            this->midiEvents.add(storedEvent); // sorted later
            this->eventsHashTable.set(autoEvent, storedEvent);
            this->notifyEventAdded(*storedEvent);
        }
        
        this->sort();
        this->updateBeatRange(true);
    }
    
    return true;
}

bool AutomationSequence::removeGroup(Array<AutomationEvent> &events, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventsGroupRemoveAction(*this->getProject(),
                                                                            this->getTrackId(),
                                                                            events));
    }
    else
    {
        for (int i = 0; i < events.size(); ++i)
        {
            const AutomationEvent &autoEvent = events.getUnchecked(i);
            
            if (AutomationEvent *matchingEvent = this->eventsHashTable[autoEvent])
            {
                this->notifyEventRemoved(*matchingEvent);
                this->midiEvents.removeObject(matchingEvent);
                this->eventsHashTable.removeValue(matchingEvent);
            }
        }
        
        this->updateBeatRange(true);
        this->notifyEventRemovedPostAction();
    }
    
    return true;
}

bool AutomationSequence::changeGroup(const Array<AutomationEvent> eventsBefore,
                                  const Array<AutomationEvent> eventsAfter,
                                  bool undoable)
{
    jassert(eventsBefore.size() == eventsAfter.size());

    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventsGroupChangeAction(*this->getProject(),
                                                                            this->getTrackId(),
                                                                            eventsBefore,
                                                                            eventsAfter));
    }
    else
    {
        for (int i = 0; i < eventsBefore.size(); ++i)
        {
            // doing this sucks
            //this->change(eventsBefore[i], eventsAfter[i], false);
            
            const AutomationEvent &autoEvent = eventsBefore.getUnchecked(i);
            const AutomationEvent &newAutoEvent = eventsAfter.getUnchecked(i);
            
            if (AutomationEvent *matchingEvent = this->eventsHashTable[autoEvent])
            {
                (*matchingEvent) = newAutoEvent;
                //this->eventsHashTable.removeValue(matchingEvent);
                this->eventsHashTable.set(newAutoEvent, matchingEvent);
                //this->notifyEventChanged(autoEvent, *matchingEvent); // lets notify the whole layer change
            }
        }
        
        this->sort();
        this->notifySequenceChanged();
        this->updateBeatRange(true);
    }

    return true;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *AutomationSequence::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::automation);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        MidiEvent *event = this->midiEvents.getUnchecked(i);
        xml->addChildElement(event->serialize());
    }
    
    return xml;
}

void AutomationSequence::deserialize(const XmlElement &xml)
{
    this->clearQuick();

    const XmlElement *root = 
        (xml.getTagName() == Serialization::Core::automation) ?
        &xml : xml.getChildByName(Serialization::Core::automation);

    if (root == nullptr)
    { return; }

    float firstBeat = 0;
    float lastBeat = 0;

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::event)
    {
        auto event = new AutomationEvent(this, 0, 0);
        event->deserialize(*e);
        
        //this->midiEvents.addSorted(*event, event); // sorted later
        this->midiEvents.add(event);
        
        lastBeat = jmax(lastBeat, event->getBeat());
        firstBeat = jmin(firstBeat, event->getBeat());

        this->eventsHashTable.set(*event, event);
    }

    this->sort();
    this->updateBeatRange(false);
    this->notifySequenceChanged();
}

void AutomationSequence::reset()
{
    this->clearQuick();
    this->notifySequenceChanged();
}

void AutomationSequence::clearQuick()
{
    this->midiEvents.clearQuick(true);
    this->eventsHashTable.clear();
}

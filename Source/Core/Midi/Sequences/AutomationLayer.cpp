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
#include "AutomationLayer.h"
#include "AutomationEventActions.h"

#include "ProjectTreeItem.h"
#include "ProjectListener.h"
#include "MidiTrackTreeItem.h"
#include "UndoStack.h"


AutomationLayer::AutomationLayer(ProjectEventDispatcher &parent) : MidiLayer(parent)
{
}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void AutomationLayer::importMidi(const MidiMessageSequence &sequence)
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
    this->notifyLayerChanged();
}


//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

void AutomationLayer::silentImport(const MidiEvent &eventToImport)
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

MidiEvent *AutomationLayer::insert(const AutomationEvent &autoEvent, bool undoable)
{
    if (AutomationEvent *matchingEvent = this->eventsHashTable[autoEvent])
    {
        return nullptr; // exists
    }

    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventInsertAction(*this->getProject(),
                                                                      this->getLayerIdAsString(),
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

bool AutomationLayer::remove(const AutomationEvent &autoEvent, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventRemoveAction(*this->getProject(),
                                                                      this->getLayerIdAsString(),
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

bool AutomationLayer::change(const AutomationEvent &autoEvent, const AutomationEvent &newAutoEvent, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventChangeAction(*this->getProject(),
                                                                      this->getLayerIdAsString(),
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

bool AutomationLayer::insertGroup(Array<AutomationEvent> &events, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventsGroupInsertAction(*this->getProject(),
                                                                            this->getLayerIdAsString(),
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

bool AutomationLayer::removeGroup(Array<AutomationEvent> &events, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventsGroupRemoveAction(*this->getProject(),
                                                                            this->getLayerIdAsString(),
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

bool AutomationLayer::changeGroup(const Array<AutomationEvent> eventsBefore,
                                  const Array<AutomationEvent> eventsAfter,
                                  bool undoable)
{
    jassert(eventsBefore.size() == eventsAfter.size());

    if (undoable)
    {
        this->getUndoStack()->perform(new AutomationEventsGroupChangeAction(*this->getProject(),
                                                                            this->getLayerIdAsString(),
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
        this->notifyLayerChanged();
        this->updateBeatRange(true);
    }

    return true;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *AutomationLayer::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::automation);
    xml->setAttribute("col", this->getColour().toString());
    xml->setAttribute("mute", this->getMuteStateAsString());
    xml->setAttribute("channel", this->getChannel());
    xml->setAttribute("instrument", this->getInstrumentId());
    xml->setAttribute("cc", this->getControllerNumber());
    xml->setAttribute("id", this->getLayerId().toString());

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        MidiEvent *event = this->midiEvents.getUnchecked(i);
        xml->addChildElement(event->serialize());
    }
	
    return xml;
}

void AutomationLayer::deserialize(const XmlElement &xml)
{
	this->clearQuick();

    const XmlElement *root = 
		(xml.getTagName() == Serialization::Core::automation) ?
		&xml : xml.getChildByName(Serialization::Core::automation);

    if (root == nullptr)
    { return; }

    this->setColour(Colour::fromString(root->getStringAttribute("col")));
    this->setChannel(root->getIntAttribute("channel", this->getChannel()));
    this->setInstrumentId(root->getStringAttribute("instrument", this->getInstrumentId()));
    this->setControllerNumber(root->getIntAttribute("cc", this->getControllerNumber()));
    this->setLayerId(root->getStringAttribute("id", this->getLayerId().toString()));
    this->muted = MidiLayer::isMuted(root->getStringAttribute("mute"));
	
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
    this->notifyLayerChanged();
}

void AutomationLayer::reset()
{
	this->clearQuick();
    this->notifyLayerChanged();
}

void AutomationLayer::clearQuick()
{
	this->midiEvents.clearQuick(true);
	this->eventsHashTable.clear();
}

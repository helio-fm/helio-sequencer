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
#include "MidiLayer.h"
#include "MidiEvent.h"
#include "Transport.h"
#include "HybridRoll.h"
#include "ProjectTreeItem.h"
#include "ProjectEventDispatcher.h"
#include "SerializationKeys.h"
#include "MidiLayerActions.h"
#include "UndoStack.h"

MidiLayer::MidiLayer(MidiTrack &parentTrack,
	ProjectEventDispatcher &dispatcher) :
	track(parentTrack),
	eventDispatcher(dispatcher),
	lastStartBeat(0.f),
	lastEndBeat(0.f),
	cachedSequence(),
    cacheIsOutdated(false)
{
}

MidiLayer::~MidiLayer()
{
    this->masterReference.clear();
}

void MidiLayer::sort()
{
    if (this->midiEvents.size() > 0)
    {
        this->midiEvents.sort(*this->midiEvents.getUnchecked(0));
    }
}

//void MidiLayer::allNotesOff()
//{
//    const MidiMessage notesOff(MidiMessage::allNotesOff(this->getChannel()));
//    this->sendMidiMessage(notesOff);
//    
//    // It vas very very frustrating to realise that some plugins (like Arturia iSEM on iOS)
//    // do NOT understand allNotesOff message, so they're just never going to shut the **** up.
//    // We always have to tell them that every single ******* note is ******* off.
//    // What a pain.
//    for (int c = 0; c < 128; ++c)
//    {
//        const MidiMessage noteOff(MidiMessage::noteOff(this->getChannel(), c));
//        this->sendMidiMessage(noteOff);
//    }
//}
//
//void MidiLayer::allSoundOff()
//{
//    const MidiMessage soundOff(MidiMessage::allSoundOff(this->getChannel()));
//    this->sendMidiMessage(soundOff);
//}
//
//void MidiLayer::allControllersOff()
//{
//    const MidiMessage controllersOff(MidiMessage::allControllersOff(this->getChannel()));
//    this->sendMidiMessage(controllersOff);
//}

//===----------------------------------------------------------------------===//
// Undoing // TODO move this to project interface
//

void MidiLayer::checkpoint()
{
    this->getUndoStack()->beginNewTransaction(String::empty);
}

void MidiLayer::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint();
        this->getUndoStack()->undo();
    }
}

void MidiLayer::redo()
{
    if (this->getUndoStack()->canRedo())
    {
        this->getUndoStack()->redo();
    }
}

void MidiLayer::clearUndoHistory()
{
	this->getUndoStack()->clearUndoHistory();
}


//===----------------------------------------------------------------------===//
// Import/export
//

MidiMessageSequence MidiLayer::exportMidi() const
{
    if (this->track.isTrackMuted())
    {
        return MidiMessageSequence();
    }
    
    if (this->cacheIsOutdated)
    {
        this->cachedSequence.clear();

        for (auto event : this->midiEvents)
        {
			const Array<MidiMessage> &track = event->getSequence();

			for (auto &message : track)
			{
				this->cachedSequence.addEvent(message);
			}

			// need to call it here?
			//this->cachedSequence.updateMatchedPairs();
        }

        this->cachedSequence.updateMatchedPairs();
        //this->cachedSequence.sort();
        this->cacheIsOutdated = false;
    }

    return this->cachedSequence;
}

//===----------------------------------------------------------------------===//
// Accessors
//

float MidiLayer::getFirstBeat() const
{
    if (this->midiEvents.size() == 0)
    {
        return FLT_MAX;
    }
    
    return this->midiEvents.getFirst()->getBeat();
}

float MidiLayer::getLastBeat() const
{
    if (this->midiEvents.size() == 0)
    {
        return -FLT_MAX;
    }
    
    return this->midiEvents.getLast()->getBeat();
}

float MidiLayer::getLengthInBeats() const
{
	if (this->midiEvents.size() == 0)
	{
		return 0;
	}

	return this->getLastBeat() - this->getFirstBeat();
}


String MidiLayer::getMuteStateAsString() const
{
    return (this->isMuted() ? "yes" : "no");
}

bool MidiLayer::isMuted(const String &muteState)
{
    return (muteState == "yes");
}

ProjectTreeItem *MidiLayer::getProject()
{
    return this->eventDispatcher.getProject();
}

UndoStack *MidiLayer::getUndoStack()
{
    return this->eventDispatcher.getProject()->getUndoStack();
}


//===----------------------------------------------------------------------===//
// Events change listener
//

void MidiLayer::notifyEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchChangeEvent(oldEvent, newEvent);
}

void MidiLayer::notifyEventAdded(const MidiEvent &event)
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchAddEvent(event);
}

void MidiLayer::notifyEventRemoved(const MidiEvent &event)
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchRemoveEvent(event);
}

void MidiLayer::notifyEventRemovedPostAction()
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchPostRemoveEvent(this);
}

void MidiLayer::notifyLayerChanged()
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchReloadLayer(this);
}

void MidiLayer::notifyBeatRangeChanged()
{
    //this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchChangeLayerBeatRange();
}

void MidiLayer::updateBeatRange(bool shouldNotifyIfChanged)
{
    if (this->lastStartBeat == this->getFirstBeat() &&
        this->lastEndBeat == this->getLastBeat())
    {
        return;
    }
    
    this->lastStartBeat = this->getFirstBeat();
    this->lastEndBeat = this->getLastBeat();
    
    if (shouldNotifyIfChanged)
    {
        this->notifyBeatRangeChanged();
    }
}


//void MidiLayer::sendMidiMessage(const MidiMessage &message)
//{
//    this->owner.getTransport()->sendMidiMessage(this->getLayerId().toString(), message);
//}

void MidiLayer::setChannel(int val)
{
	this->cacheIsOutdated = true;
	this->channel = val;
}

void MidiLayer::setColour(Colour val)
{
	if (this->colour != val)
	{
		this->colour = val;
	}
}

void MidiLayer::setMuted(bool shouldBeMuted)
{
	if (this->muted != shouldBeMuted)
	{
		this->muted = shouldBeMuted;
		this->eventDispatcher.dispatchReloadLayer(this);
	}
}

void MidiLayer::setInstrumentId(const String &val)
{
    if (this->instrumentId != val)
    {
        this->instrumentId = val;
        this->eventDispatcher.dispatchReloadLayer(this);
    }
}

void MidiLayer::setControllerNumber(int val)
{
    if (this->controllerNumber != val)
    {
        this->controllerNumber = val;
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

int MidiLayer::compareElements(const MidiLayer *first, const MidiLayer *second)
{
	// Compare by names?
	if (first == second)
	{
		return 0;
	}

	return first->track->getTrackName().compare(second->track->getTrackName());
}

int MidiLayer::hashCode() const noexcept
{
	return this->layerId.toString().hashCode();
}

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
#include "MidiSequence.h"
#include "ProjectEventDispatcher.h"
#include "ProjectTreeItem.h"
#include "UndoStack.h"
#include "MidiTrack.h"

struct EventIdGenerator
{
    static String generateId(uint8 length = 2)
    {
        String id;
        Random r;
        r.setSeedRandomly();
        static const char idChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        for (size_t i = 0; i < length; ++i)
        {
            id += idChars[r.nextInt(62)];
        }
        return id;
    }
};

MidiSequence::MidiSequence(MidiTrack &parentTrack,
    ProjectEventDispatcher &dispatcher) noexcept :
    track(parentTrack),
    eventDispatcher(dispatcher),
    lastStartBeat(0.f),
    lastEndBeat(0.f),
    cachedSequence(),
    cacheIsOutdated(false) {}

void MidiSequence::sort()
{
    if (this->midiEvents.size() > 0)
    {
        this->midiEvents.sort(*this->midiEvents.getFirst());
    }
}

//===----------------------------------------------------------------------===//
// Undoing // TODO move this to project interface
//===----------------------------------------------------------------------===//

void MidiSequence::checkpoint() noexcept
{
    this->getUndoStack()->beginNewTransaction({});
}

void MidiSequence::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint();
        this->getUndoStack()->undo();
    }
}

void MidiSequence::redo()
{
    if (this->getUndoStack()->canRedo())
    {
        this->getUndoStack()->redo();
    }
}

void MidiSequence::clearUndoHistory()
{
    this->getUndoStack()->clearUndoHistory();
}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

MidiMessageSequence MidiSequence::exportMidi() const
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
            const auto &track = event->toMidiMessages();

            for (auto &message : track)
            {
                this->cachedSequence.addEvent(message);
            }

            // need to call it here?
            //this->cachedSequence.updateMatchedPairs();
        }

        this->cachedSequence.updateMatchedPairs();
        //this->cachedSequence.sort(); // will be sorted by caller
        this->cacheIsOutdated = false;
    }

    return this->cachedSequence;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

float MidiSequence::getFirstBeat() const noexcept
{
    if (this->midiEvents.size() == 0)
    {
        return FLT_MAX;
    }
    
    return this->midiEvents.getFirst()->getBeat();
}

float MidiSequence::getLastBeat() const noexcept
{
    if (this->midiEvents.size() == 0)
    {
        return -FLT_MAX;
    }
    
    return this->midiEvents.getLast()->getBeat();
}

float MidiSequence::getLengthInBeats() const noexcept
{
    if (this->midiEvents.size() == 0)
    {
        return 0;
    }

    return this->getLastBeat() - this->getFirstBeat();
}

MidiTrack *MidiSequence::getTrack() const noexcept
{
    return &this->track;
}

ProjectTreeItem *MidiSequence::getProject() const noexcept
{
    return this->eventDispatcher.getProject();
}

UndoStack *MidiSequence::getUndoStack() const noexcept
{
    return this->eventDispatcher.getProject()->getUndoStack();
}

//===----------------------------------------------------------------------===//
// Events change listener
//===----------------------------------------------------------------------===//

void MidiSequence::notifyEventChanged(const MidiEvent &e1, const MidiEvent &e2)
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchChangeEvent(e1, e2);
}

void MidiSequence::notifyEventAdded(const MidiEvent &event)
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchAddEvent(event);
}

void MidiSequence::notifyEventRemoved(const MidiEvent &event)
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchRemoveEvent(event);
}

void MidiSequence::notifyEventRemovedPostAction()
{
    this->cacheIsOutdated = true;
    this->eventDispatcher.dispatchPostRemoveEvent(this);
}

void MidiSequence::invalidateSequenceCache()
{
    this->cacheIsOutdated = true;
}

void MidiSequence::updateBeatRange(bool shouldNotifyIfChanged)
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
        this->eventDispatcher.dispatchChangeProjectBeatRange();
    }
}

String MidiSequence::createUniqueEventId() const noexcept
{
    uint8 length = 2;
    String eventId = EventIdGenerator::generateId(length);
    while (this->usedEventIds.contains(eventId))
    {
        length++;
        eventId = EventIdGenerator::generateId(length);
    }
    
    this->usedEventIds.insert({eventId});
    return eventId;
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

String MidiSequence::getTrackId() const noexcept
{
    return this->track.getTrackId().toString();
}

int MidiSequence::getChannel() const noexcept
{
    return this->track.getTrackChannel();
}

//void MidiSequence::sendMidiMessage(const MidiMessage &message)
//{
//    this->owner.getTransport()->sendMidiMessage(this->getLayerId().toString(), message);
//}

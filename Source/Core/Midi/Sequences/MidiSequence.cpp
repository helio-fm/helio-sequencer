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
#include "ProjectNode.h"
#include "UndoStack.h"
#include "MidiTrack.h"

struct EventIdGenerator
{
    static String generateId(uint8 length = 2)
    {
        String id;
        static Random r;
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
    lastEndBeat(0.f) {}


void MidiSequence::sort()
{
    if (this->midiEvents.size() > 0)
    {
        this->midiEvents.sort(*this->midiEvents.getFirst());
    }
}

//===----------------------------------------------------------------------===//
// Undoing
//===----------------------------------------------------------------------===//

String MidiSequence::getLastUndoDescription() const
{
    return this->getUndoStack()->getUndoDescription();
}

void MidiSequence::checkpoint(const String &transactionName) noexcept
{
    this->getUndoStack()->beginNewTransaction(transactionName);
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

void MidiSequence::exportMidi(MidiMessageSequence &outSequence,
    const Clip &clip, double timeAdjustment, double timeFactor) const
{
    if (this->track.isTrackMuted())
    {
        return;
    }

    for (const auto *event : this->midiEvents)
    {
        event->exportMessages(outSequence, clip, timeAdjustment, timeFactor);
    }

    outSequence.updateMatchedPairs();
}

float MidiSequence::midiTicksToBeats(double ticks, int timeFormat) noexcept
{
    const double secsPerQuarterNoteAt120BPM = 0.5;

    if (timeFormat < 0)
    {
        const double timeInSeconds = ticks / (-(timeFormat >> 8) * (timeFormat & 0xff));
        return float(timeInSeconds * secsPerQuarterNoteAt120BPM * BEATS_PER_BAR);
    }

    const auto tickLen = 1.0 / (timeFormat & 0x7fff);
    const auto secsPerTick = 0.5 * tickLen;
    const double timeInSeconds = ticks * secsPerTick;
    return float(timeInSeconds * secsPerQuarterNoteAt120BPM * BEATS_PER_BAR);
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

ProjectNode *MidiSequence::getProject() const noexcept
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

const String &MidiSequence::getTrackId() const noexcept
{
    return this->track.getTrackId();
}

int MidiSequence::getChannel() const noexcept
{
    return this->track.getTrackChannel();
}

//void MidiSequence::sendMidiMessage(const MidiMessage &message)
//{
//    this->owner.getTransport()->sendMidiMessage(this->getLayerId().toString(), message);
//}

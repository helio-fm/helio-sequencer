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

struct EventIdGenerator final
{
    static MidiEvent::Id generateId(int length = 2)
    {
        jassert(length <= 4);
        MidiEvent::Id id = 0;
        static Random r;
        r.setSeedRandomly();
        static const char idChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < length; ++i)
        {
            id |= idChars[r.nextInt(62)] << (i * CHAR_BIT);
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

UndoActionId MidiSequence::getLastUndoActionId() const
{
    return this->getUndoStack()->getUndoActionId();
}

void MidiSequence::checkpoint(UndoActionId id) noexcept
{
    this->getUndoStack()->beginNewTransaction(id);
}

void MidiSequence::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint(); // finish current transaction to be undoed
        this->getUndoStack()->undo();
    }
}

void MidiSequence::undoCurrentTransactionOnly()
{
    this->getUndoStack()->undoCurrentTransactionOnly();
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

void MidiSequence::exportMidi(MidiMessageSequence &outSequence, const Clip &clip,
    bool soloPlaybackMode, double timeAdjustment, double timeFactor) const
{
    if (clip.isMuted())
    {
        return;
    }

    // Common logic is to ignore soloPlaybackMode flag
    // (which means there's at least one solo clip somewhere),
    // since not all sequence types are supposed to be soloed,
    // for example, automations should be exported all the time unless muted.
    // Moreover, for now, only PianoSequence will override this method
    // and make sure it skips a no-solo clip, when soloPlaybackMode is true.

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
        this->eventDispatcher.dispatchChangeTrackBeatRange();
        this->eventDispatcher.dispatchChangeProjectBeatRange();
    }
}

MidiEvent::Id MidiSequence::createUniqueEventId() const noexcept
{
    int length = 2;
    auto eventId = EventIdGenerator::generateId(length);
    while (this->usedEventIds.contains(eventId))
    {
        length = jmin(4, length + 1);
        eventId = EventIdGenerator::generateId(length);
    }
    
    this->usedEventIds.insert(eventId);
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

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

class LegacyEventFormatSupportTests final : public UnitTest
{
public:
    LegacyEventFormatSupportTests() : UnitTest("Legacy note id format support tests", UnitTestCategories::helio) {}

    void runTest() override
    {
        beginTest("Legacy note id serialization");

        const auto id2 = EventIdGenerator::generateId(2);
        const auto id3 = EventIdGenerator::generateId(3);
        const auto id4 = EventIdGenerator::generateId(4);

        const auto p2 = MidiEvent::packId(id2);
        expectEquals(p2.length(), 2);

        const auto p3 = MidiEvent::packId(id3);
        expectEquals(p3.length(), 3);

        const auto p4 = MidiEvent::packId(id4);
        expectEquals(p4.length(), 4);

        const auto u2 = MidiEvent::unpackId(p2);
        expectEquals(id2, u2);

        const auto u3 = MidiEvent::unpackId(p3);
        expectEquals(id3, u3);

        const auto u4 = MidiEvent::unpackId(p4);
        expectEquals(id4, u4);

        const String s = "aA0";
        const auto u = MidiEvent::unpackId(s);
        const auto p = MidiEvent::packId(u);
        expectEquals(p, s);
    }
};

static LegacyEventFormatSupportTests legacyFormatSupportTests;

#endif

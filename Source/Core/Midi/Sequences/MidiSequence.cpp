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
#include "ProjectNode.h"
#include "ProjectMetadata.h"
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
    eventDispatcher(dispatcher) {}

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
    this->getUndoStack()->undo();
}

void MidiSequence::undoCurrentTransactionOnly()
{
    this->getUndoStack()->undoCurrentTransactionOnly();
}

void MidiSequence::redo()
{
    this->getUndoStack()->redo();
}

void MidiSequence::clearUndoHistory()
{
    this->getUndoStack()->clearUndoHistory();
}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void MidiSequence::exportMidi(MidiMessageSequence &outSequence,
    const Clip &clip, const KeyboardMapping &keyMap,
    bool soloPlaybackMode, bool exportMetronome,
    double projectFirstBeat, double projectLastBeat,
    double timeFactor) const
{
    if (this->midiEvents.isEmpty() || clip.isMuted())
    {
        return;
    }

    // this will ignore soloPlaybackMode flag,
    // (which means there's at least one solo clip somewhere),
    // since not all sequence types are supposed to be soloed,
    // for example, automations should be exported all the time unless muted;

    // for now, PianoSequence overrides this method
    // to make sure it skips all no-solo clips, when soloPlaybackMode is true,
    // and TimeSignatureSequence overrides this method
    // to emit the "virtual" metronome track, if needed

    for (const auto *event : this->midiEvents)
    {
        event->exportMessages(outSequence, clip, keyMap, timeFactor);
    }

    outSequence.updateMatchedPairs();
}

float MidiSequence::midiTicksToBeats(double ticks, int timeFormat) noexcept
{
    const double secsPerQuarterNoteAt120BPM = 0.5;

    if (timeFormat < 0)
    {
        const double timeInSeconds = ticks / (-(timeFormat >> 8) * (timeFormat & 0xff));
        return float(timeInSeconds * secsPerQuarterNoteAt120BPM * Globals::beatsPerBar);
    }

    const auto tickLen = 1.0 / (timeFormat & 0x7fff);
    const auto secsPerTick = 0.5 * tickLen;
    const double timeInSeconds = ticks * secsPerTick;
    return float(timeInSeconds * secsPerQuarterNoteAt120BPM * Globals::beatsPerBar);
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

float MidiSequence::findFirstBeat() const noexcept
{
    return this->isEmpty() ? 0.f :
        this->midiEvents.getFirst()->getBeat();
}

float MidiSequence::findLastBeat() const noexcept
{
    return this->isEmpty() ? 0.f :
        this->midiEvents.getLast()->getBeat();
}

float MidiSequence::getLengthInBeats() const noexcept
{
    return this->getLastBeat() - this->getFirstBeat();
}

MidiTrack *MidiSequence::getTrack() const noexcept
{
    return &this->track;
}

// I hate these 3 getters but have very little idea how to do without:

ProjectNode *MidiSequence::getProject() const noexcept
{
    return this->eventDispatcher.getProject();
}

UndoStack *MidiSequence::getUndoStack() const noexcept
{
    return this->eventDispatcher.getProject()->getUndoStack();
}

int MidiSequence::getPeriodSize() const noexcept
{
    return this->eventDispatcher.getProject()->getProjectInfo()->
        getTemperament()->getPeriodSize();
}

//===----------------------------------------------------------------------===//
// Events change listener
//===----------------------------------------------------------------------===//

void MidiSequence::updateBeatRange(bool shouldNotifyIfChanged)
{
    const auto newStart = this->findFirstBeat();
    const auto newEnd = this->findLastBeat();

    if (this->sequenceStartBeat == newStart && this->sequenceEndBeat == newEnd)
    {
        return;
    }
    
    this->sequenceStartBeat = newStart;
    this->sequenceEndBeat = newEnd;
    
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

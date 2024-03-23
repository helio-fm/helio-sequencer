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
#include "TimeSignaturesSequence.h"
#include "TimeSignatureEventActions.h"
#include "SerializationKeys.h"
#include "ProjectNode.h"
#include "UndoStack.h"
#include "Meter.h"
#include "MetronomeSynth.h"
#include "Config.h"

TimeSignaturesSequence::TimeSignaturesSequence(MidiTrack &track,
    ProjectEventDispatcher &dispatcher) noexcept :
    MidiSequence(track, dispatcher) {}

//===----------------------------------------------------------------------===//
// Import/export
//===----------------------------------------------------------------------===//

void TimeSignaturesSequence::importMidi(const MidiMessageSequence &sequence,
    short timeFormat, Optional<int> customFilter)
{
    this->clearUndoHistory();
    this->checkpoint();

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        const auto &message = sequence.getEventPointer(i)->message;
        if (message.isTimeSignatureMetaEvent())
        {
            int numerator = 0;
            int denominator = 0;
            message.getTimeSignatureInfo(numerator, denominator);
            const float startBeat = MidiSequence::midiTicksToBeats(message.getTimeStamp(), timeFormat);
            const TimeSignatureEvent signature(this, startBeat, numerator, denominator);
            this->importMidiEvent<TimeSignatureEvent>(signature);
        }
    }

    this->updateBeatRange(false);
}

void TimeSignaturesSequence::exportMidi(MidiMessageSequence &outSequence,
    const Clip &clip, const KeyboardMapping &keyMap,
    GeneratedSequenceBuilder &generatedSequences,
    bool soloPlaybackMode, bool exportMetronome,
    float projectFirstBeat, float projectLastBeat,
    double timeFactor /*= 1.0*/) const
{
    // This method pretty much duplicates the base method, except for
    // emitting the "virtual" metronome track, if it's needed
    if (exportMetronome)
    {
        // I'm too lazy to take timeFactor into account here,
        // but it is only used when exporting to midi file (i.e. sets midi clock),
        // and we don't export the virtual metronome track to midi files:
        jassert(timeFactor == 1.0);

        const auto emitNextMetronomeEvent = [](MidiMessageSequence &outSequence,
            float beat, const MetronomeScheme &scheme, int &syllableIndex)
        {
            const auto currentSyllable = scheme.getSyllableAt(syllableIndex);
            const auto key = MetronomeSynth::getKeyForSyllable(currentSyllable);

            syllableIndex = ++syllableIndex % scheme.getSize();

            constexpr auto metronomeChannel = 1;    // doesn't matter which one
            constexpr auto metronomeVelocity = 1.f; // also will be ignored

            MidiMessage mentonomeNoteOn(MidiMessage::noteOn(metronomeChannel, key, metronomeVelocity));
            mentonomeNoteOn.setTimeStamp(beat);
            outSequence.addEvent(mentonomeNoteOn);

            // for simplicity, not emitting note-offs for the built-in metronome,
            // Synthesiser class automatically stops/starts the voices when the same note repeats
        };

        if (this->midiEvents.isEmpty())
        {
            int syllableIndex = 0;
            const MetronomeScheme defaultScheme;
            for (float beat = projectFirstBeat; beat <= projectLastBeat; beat += 1.f)
            {
                emitNextMetronomeEvent(outSequence, beat, defaultScheme, syllableIndex);
            }
        }
        else
        {
            const auto *firstEvent = static_cast<const TimeSignatureEvent *>(this->midiEvents.getFirst());
            jassert(firstEvent->getBeat() >= projectFirstBeat);

            {
                int syllableIndex = 0;
                const auto metronomeScheme = firstEvent->getMeter().getMetronome();
                jassert(metronomeScheme.isValid());

                for (float beat = projectFirstBeat; beat < firstEvent->getBeat();
                     beat += firstEvent->getDenominatorInBeats())
                {
                    emitNextMetronomeEvent(outSequence, beat, metronomeScheme, syllableIndex);
                }
            }

            for (int i = 0; i < this->midiEvents.size(); ++i)
            {
                const auto *event = static_cast<const TimeSignatureEvent *>(this->midiEvents.getUnchecked(i));
                const auto nextBeat = (i < this->midiEvents.size() - 1) ?
                    this->midiEvents.getUnchecked(i + 1)->getBeat() : projectLastBeat;

                int syllableIndex = 0;
                const auto metronomeScheme = event->getMeter().getMetronome();
                jassert(metronomeScheme.isValid());

                for (float beat = event->getBeat(); beat < nextBeat;
                     beat += event->getDenominatorInBeats())
                {
                    emitNextMetronomeEvent(outSequence, beat, metronomeScheme, syllableIndex);
                }
            }
        }
    }

    for (const auto *event : this->midiEvents)
    {
        event->exportMessages(outSequence, clip, keyMap, timeFactor);
    }

    outSequence.updateMatchedPairs();
}

//===----------------------------------------------------------------------===//
// Undoable track editing
//===----------------------------------------------------------------------===//

MidiEvent *TimeSignaturesSequence::insert(const TimeSignatureEvent &eventParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventInsertAction(*this->getProject(),
                this->getTrackId(), eventParams));
    }
    else
    {
        auto *ownedEvent = new TimeSignatureEvent(this, eventParams);
        this->midiEvents.addSorted(*ownedEvent, ownedEvent);
        this->eventDispatcher.dispatchAddEvent(*ownedEvent);
        this->updateBeatRange(true);
        return ownedEvent;
    }

    return nullptr;
}

MidiEvent *TimeSignaturesSequence::appendUnsafe(const TimeSignatureEvent &eventParams)
{
    auto *ownedEvent = new TimeSignatureEvent(this, eventParams);
    this->midiEvents.add(ownedEvent);
    this->eventDispatcher.dispatchAddEvent(*ownedEvent);
    this->updateBeatRange(true);
    return ownedEvent;
}

bool TimeSignaturesSequence::remove(const TimeSignatureEvent &signature, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventRemoveAction(*this->getProject(),
                this->getTrackId(), signature));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(signature, &signature);
        if (index >= 0)
        {
            auto *removedEvent = this->midiEvents.getUnchecked(index);
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

bool TimeSignaturesSequence::change(const TimeSignatureEvent &oldParams,
    const TimeSignatureEvent &newParams, bool undoable)
{
    if (undoable)
    {
        this->getUndoStack()->
            perform(new TimeSignatureEventChangeAction(*this->getProject(),
                this->getTrackId(), oldParams, newParams));
    }
    else
    {
        const int index = this->midiEvents.indexOfSorted(oldParams, &oldParams);
        if (index >= 0)
        {
            auto *changedEvent = static_cast<TimeSignatureEvent *>(this->midiEvents.getUnchecked(index));
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

//===----------------------------------------------------------------------===//
// Callbacks
//===----------------------------------------------------------------------===//

Function<void(const String &text)> TimeSignaturesSequence::getEventChangeCallback(const TimeSignatureEvent &event)
{
    return[this, event](const String &text)
    {
        int numerator;
        int denominator;
        Meter::parseString(text, numerator, denominator);
        this->checkpoint();
        this->change(event, event.withNumerator(numerator).withDenominator(denominator), true);

    };
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData TimeSignaturesSequence::serialize() const
{
    SerializedData tree(Serialization::Midi::timeSignatures);

    for (int i = 0; i < this->midiEvents.size(); ++i)
    {
        const MidiEvent *event = this->midiEvents.getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}

void TimeSignaturesSequence::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;

    const auto root =
        data.hasType(Serialization::Midi::timeSignatures) ?
        data : data.getChildWithName(Serialization::Midi::timeSignatures);

    if (!root.isValid())
    {
        return;
    }

    float lastBeat = 0;
    float firstBeat = 0;

    forEachChildWithType(root, e, Serialization::Midi::timeSignature)
    {
        TimeSignatureEvent *signature = new TimeSignatureEvent(this);
        signature->deserialize(e);
        
        this->midiEvents.add(signature); // sorted later

        lastBeat = jmax(lastBeat, signature->getBeat());
        firstBeat = jmin(firstBeat, signature->getBeat());

        this->usedEventIds.insert(signature->getId());
    }

    this->sort<TimeSignatureEvent>();
    this->updateBeatRange(false);
}

void TimeSignaturesSequence::reset()
{
    this->midiEvents.clear();
    this->usedEventIds.clear();
}

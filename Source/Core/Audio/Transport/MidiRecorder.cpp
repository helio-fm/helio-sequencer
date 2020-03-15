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
#include "MidiRecorder.h"

#include "UndoStack.h"
#include "MidiTrack.h"
#include "PianoSequence.h"
#include "Transport.h"

#include "App.h"
#include "Workspace.h"
#include "AudioCore.h"

// todo show some hints on where to expect new notes?
// in pattern mode, highlight the selected clip in red?

// no need for updating too often, I guess:
#define MIDI_RECORDER_UPDATE_TIME_HZ 15

MidiRecorder::MidiRecorder(WeakReference<Transport> transport,
    WeakReference<UndoStack> undoStack) :
    transport(transport),
    undoStack(undoStack)
{
    this->lastUpdateTime = Time::getMillisecondCounterHiRes();
    this->lastCorrectPosition = this->transport->getSeekBeat();

    this->transport->addTransportListener(this);
}

MidiRecorder::~MidiRecorder()
{
    this->transport->removeTransportListener(this);
}

void MidiRecorder::setSelectedScope(WeakReference<MidiTrack> track, const Clip &clip)
{
    if (this->activeTrack != track || this->activeClip != clip)
    {
        jassert(dynamic_cast<PianoSequence *>(track->getSequence()));

        if (this->activeTrack != nullptr)
        {
            this->finaliseAllHoldingNotes();
        }

        this->activeTrack = track;
        this->activeClip = clip;
        this->shouldCheckpoint = true;
    }
}

void MidiRecorder::onSeek(float beatPosition, double, double) noexcept
{
    this->lastCorrectPosition = beatPosition;
    this->lastUpdateTime = Time::getMillisecondCounterHiRes();
}

void MidiRecorder::onTempoChanged(double msPerQuarter) noexcept
{
    this->msPerQuarterNote = jmax(msPerQuarter, 0.01);
    this->lastUpdateTime = Time::getMillisecondCounterHiRes();
}

void MidiRecorder::onRecord()
{
    if (!this->isRecording.get())
    {
        this->isRecording = true;
        this->shouldCheckpoint = true;

        auto &device = App::Workspace().getAudioCore().getDevice();
        device.addMidiInputDeviceCallback({}, this);

        if (this->isPlaying.get())
        {
            this->startTimerHz(MIDI_RECORDER_UPDATE_TIME_HZ);
        }
    }
}

void MidiRecorder::onPlay() noexcept
{
    if (!this->isPlaying.get())
    {
        this->isPlaying = true;
        this->lastUpdateTime = Time::getMillisecondCounterHiRes();

        if (this->isRecording.get())
        {
            this->startTimerHz(MIDI_RECORDER_UPDATE_TIME_HZ);
        }
    }
}

void MidiRecorder::onStop()
{
    if (this->isRecording.get())
    {
        auto &device = App::Workspace().getAudioCore().getDevice();
        device.removeMidiInputDeviceCallback({}, this);

        this->isRecording = false;
    }

    // fixme handle buffers?
    jassert(this->noteOnsBuffer.isEmpty());
    jassert(this->noteOffsBuffer.isEmpty());

    this->stopTimer();

    if (this->activeTrack != nullptr)
    {
        this->finaliseAllHoldingNotes();
    }

    this->isPlaying = false;
    this->lastUpdateTime = 0.0;
}

// called from the message thread, so we can insert new midi events
// (note that the track selection may change during recording);
// the main recording logic goes here:
void MidiRecorder::handleAsyncUpdate()
{
    if (this->noteOnsBuffer.isEmpty() &&
        this->noteOffsBuffer.isEmpty())
    {
        // nothing to do
        return;
    }

    // if no track is selected, must checkpoint anyway
    jassert(this->activeTrack != nullptr || this->shouldCheckpoint.get());

    // at this point we surely have some actions to perform,
    // so first, let's manage undo actions properly;
    // we'll checkpoint every time the active track changes:
    if (this->shouldCheckpoint.get())
    {
        jassert(this->holdingNotes.size() == 0);
        this->undoStack->beginNewTransaction();
        this->shouldCheckpoint = false;
    }

    const auto currentBeat = this->getEstimatedPosition();

    // if something is selected (can be both rolls), simply insert messages,
    // if nothing is selected (pattern roll), first create a new track and select it
    // if multiple tracks are selected (also pattern roll) - same as ^
    if (this->activeTrack == nullptr)
    {
        // todo create one

        //this->activeTrack = track;
        //this->activeClip = clip;
    }

    //

    // handle note offs and fill unhandledNoteOffs
    while (!this->noteOffsBuffer.isEmpty())
    {
        const auto last = this->noteOffsBuffer.size() - 1;
        const auto i = this->noteOffsBuffer.removeAndReturn(last);
        const bool wasHandled = this->finaliseHoldingNote(i.getNoteNumber());
        if (!wasHandled)
        {
            this->unhandledNoteOffs.add(i);
        }
    }

    // handle note ons
    while (!this->noteOnsBuffer.isEmpty())
    {
        const auto last = this->noteOnsBuffer.size() - 1;
        const auto i = this->noteOnsBuffer.removeAndReturn(last);
        this->startHoldingNote(i);
    }

    // handle unhandledNoteOffs and clean
    for (const auto &i : this->unhandledNoteOffs)
    {
        const bool wasHandled = this->finaliseHoldingNote(i.getNoteNumber());
        jassert(wasHandled);
    }

    this->unhandledNoteOffs.clearQuick();

    // a neat helper: start playback, if still not playing,
    // yet have received some midi events
    if (!this->isPlaying.get())
    {
        this->transport->startPlayback();
    }
}

// called from the high-priority system thread:
void MidiRecorder::handleIncomingMidiMessage(MidiInput *, const MidiMessage &message)
{
    if (message.isNoteOn())
    {
        this->noteOnsBuffer.add(message.withTimeStamp(this->getEstimatedPosition()));
    }
    else if (message.isNoteOff())
    {
        this->noteOffsBuffer.add(message.withTimeStamp(this->getEstimatedPosition()));
    }

    this->triggerAsyncUpdate();
}

// current beat, estimated since the last known
// midi event, including the tempo change events:
double MidiRecorder::getEstimatedPosition() const
{
    if (!this->isPlaying.get())
    {
        return this->lastCorrectPosition.get();
    }

    const double timeOffsetMs = Time::getMillisecondCounterHiRes() - this->lastUpdateTime.get();
    const double positionOffset = timeOffsetMs / this->msPerQuarterNote.get();
    const double estimatedPosition = this->lastCorrectPosition.get() + positionOffset;
    return estimatedPosition;
}

void MidiRecorder::timerCallback()
{
    if (this->activeTrack != nullptr)
    {
        this->updateLengthsOfHoldingNotes();
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

#define STARTING_NOTE_LENGTH (1.f / TICKS_PER_BEAT)

void MidiRecorder::startHoldingNote(MidiMessage message)
{
    jassert(this->activeTrack != nullptr);

    const auto key = message.getNoteNumber();
    jassert(!this->holdingNotes.contains(key));

    const Note noteParams(this->activeTrack->getSequence(),
        key,
        roundBeat(float(message.getTimeStamp()) - this->activeClip.getBeat()),
        STARTING_NOTE_LENGTH,
        message.getVelocity() / 128.f);

    this->getPianoSequence()->insert(noteParams, true);
    this->holdingNotes[key] = noteParams;
}

void MidiRecorder::updateLengthsOfHoldingNotes() const
{
    jassert(this->activeTrack != nullptr);

    if (this->holdingNotes.size() == 0)
    {
        //DBG("Skip 1");
        return;
    }

    const auto currentBeat =
        float(this->getEstimatedPosition() - this->activeClip.getBeat());

    Array<Note> groupBefore;
    Array<Note> groupAfter;

    for (const auto i : this->holdingNotes)
    {
        const auto newLength = roundBeat(currentBeat - i.second.getBeat());
        if (i.second.getLength() == newLength)
        {
            //DBG("Skip 2");
            return;
        }

        groupBefore.add(i.second);
        groupAfter.add(i.second.withLength(newLength));
    }

    this->getPianoSequence()->changeGroup(groupBefore, groupAfter, true);
}

void MidiRecorder::finaliseAllHoldingNotes()
{
    jassert(this->activeTrack != nullptr);
    this->updateLengthsOfHoldingNotes();
    this->holdingNotes.clear();
}

bool MidiRecorder::finaliseHoldingNote(int key)
{
    jassert(this->activeTrack != nullptr);

    const auto currentBeat =
        float(this->getEstimatedPosition() - this->activeClip.getBeat());

    if (this->holdingNotes.contains(key))
    {
        const auto &note = this->holdingNotes[key];
        const auto newLength = roundBeat(currentBeat - note.getBeat());
        this->getPianoSequence()->change(note, note.withLength(newLength), true);
        this->holdingNotes.erase(key);
        return true;
    }

    return false;
}

PianoSequence *MidiRecorder::getPianoSequence() const
{
    return static_cast<PianoSequence *>(this->activeTrack->getSequence());
}

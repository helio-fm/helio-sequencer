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

#include "ProjectNode.h"
#include "Pattern.h"
#include "MidiTrack.h"
#include "PianoTrackNode.h"
#include "PianoSequence.h"

#include "PatternRoll.h"
#include "SequencerOperations.h"
#include "PianoTrackActions.h"
#include "UndoStack.h"

#include "Workspace.h"
#include "AudioCore.h"
#include "ColourIDs.h"

// todo for the future: handle pedal and more automation events

MidiRecorder::MidiRecorder(ProjectNode &project) :
    project(project)
{
    this->lastUpdateTime = Time::getMillisecondCounterHiRes();
    this->lastCorrectPosition = this->getTransport().getSeekBeat();

    this->getTransport().addTransportListener(this);
}

MidiRecorder::~MidiRecorder()
{
    this->getTransport().removeTransportListener(this);
}

void MidiRecorder::setTargetScope(const Clip *clip, const String &instrumentId)
{
    if (instrumentId.isNotEmpty())
    {
        this->lastValidInstrumentId = instrumentId;
    }

    auto *track = clip == nullptr ? nullptr : clip->getPattern()->getTrack();

    if (this->activeTrack != track || this->activeClip != clip)
    {
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
    // handle the loop rewind:
    if (this->isPlaying.get() &&
        beatPosition < this->lastCorrectPosition.get())
    {
        // this callback clearly comes from the player thread,
        // and we are going to do some changes, which the UI
        // will reflect immediately, so let's lock the message thread:
        MessageManagerLock mml(Thread::getCurrentThread());
        jassert(mml.lockWasGained());

        this->finaliseAllHoldingNotes();
    }

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
            this->startTimerHz(MidiRecorder::updateTimeHz);
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
            this->startTimerHz(MidiRecorder::updateTimeHz);
        }
    }
}

void MidiRecorder::onStop()
{
    if (this->isRecording.get())
    {
        this->stopTimer();

        auto &device = App::Workspace().getAudioCore().getDevice();
        device.removeMidiInputDeviceCallback({}, this);

        this->isRecording = false;

        // fixme handle buffers?
        jassert(this->noteOnsBuffer.isEmpty());
        jassert(this->noteOffsBuffer.isEmpty());

        this->holdingNotes.clear();
    }

    this->isPlaying = false;
    this->lastUpdateTime = 0.0;
    this->msPerQuarterNote = Globals::Defaults::msPerBeat;
}

static SerializedData createPianoTrackTemplate(const String &name,
    float startBeat, const String &instrumentId, String &outTrackId)
{
    auto newNode = make<PianoTrackNode>(name);

    // We need to have at least one clip on the pattern:
    const Clip clip(newNode->getPattern(), startBeat, 0);
    newNode->getPattern()->insert(clip, false);

    Random r;
    const auto colours = ColourIDs::getColoursList();
    const int ci = r.nextInt(colours.size());
    newNode->setTrackColour(colours[ci], dontSendNotification);
    newNode->setTrackInstrumentId(instrumentId, false);

    outTrackId = newNode->getTrackId();
    return newNode->serialize();
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

    // a neat helper: start playback, if still not playing,
    // yet have received some midi events;
    // we do it before inserting any events,
    // so that the first note doesn't sound twice
    if (!this->isPlaying.get())
    {
        this->getTransport().startPlayback();
    }

    // if no track is selected, must checkpoint anyway
    jassert(this->activeTrack != nullptr || this->shouldCheckpoint.get());

    // at this point we surely have some actions to perform,
    // so first, let's manage undo actions properly;
    // we'll checkpoint every time the active track changes:
    if (this->shouldCheckpoint.get())
    {
        jassert(this->holdingNotes.empty());
        this->project.checkpoint();
        this->shouldCheckpoint = false;
    }

    // if something is selected (can be both rolls), simply insert messages,
    // if nothing is selected (pattern roll), first create a new track and select it
    // if multiple tracks are selected (also pattern roll) - same as ^
    if (this->activeTrack == nullptr)
    {
        const auto newName =
            SequencerOperations::generateNextNameForNewTrack("Recording",
                this->project.getAllTrackNames());

        // lastValidInstrumentId may be empty at this point:
        if (this->lastValidInstrumentId.isEmpty())
        {
            auto instruments = App::Workspace().getAudioCore().getInstruments();
            if (!instruments.isEmpty())
            {
                this->lastValidInstrumentId = instruments.getFirst()->getIdAndHash();
            }
        }

        String outTrackId;
        const auto trackTemplate = createPianoTrackTemplate(newName,
            this->lastCorrectPosition.get(), this->lastValidInstrumentId, outTrackId);

        this->project.getUndoStack()->perform(
            new PianoTrackInsertAction(this->project,
                &this->project, trackTemplate, newName));

        this->activeTrack = this->project.findTrackById<MidiTrackNode>(outTrackId);
        this->activeClip = this->activeTrack->getPattern()->getUnchecked(0);
        this->shouldCheckpoint = false;
    }
    
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
        this->finaliseHoldingNote(i.getNoteNumber());
    }

    this->unhandledNoteOffs.clearQuick();
}

struct SortMidiMessagesByTimestamp final
{
    static int compareElements(const MidiMessage &first, const MidiMessage &second)
    {
        return int(first.getTimeStamp() - second.getTimeStamp());
    }
};

static SortMidiMessagesByTimestamp tsSort;

// called from the high-priority system thread:
void MidiRecorder::handleIncomingMidiMessage(MidiInput *, const MidiMessage &message)
{
    if (message.isNoteOn())
    {
        this->noteOnsBuffer.addSorted(tsSort,
            message.withTimeStamp(this->getEstimatedPosition()));
    }
    else if (message.isNoteOff())
    {
        this->noteOffsBuffer.addSorted(tsSort,
            message.withTimeStamp(this->getEstimatedPosition()));
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

void MidiRecorder::startHoldingNote(MidiMessage message)
{
    jassert(this->activeClip != nullptr);
    jassert(this->activeTrack != nullptr);

    const auto key = message.getNoteNumber();
    
    if (this->holdingNotes.contains(key))
    {
        DBG("Found weird note-on/note-off order");
        this->finaliseHoldingNote(key);
    }

    const Note noteParams(this->activeTrack->getSequence(),
        key - this->activeClip->getKey(),
        roundBeat(float(message.getTimeStamp()) - this->activeClip->getBeat()),
        Globals::minNoteLength,
        float(message.getVelocity()) / 128.f);

    this->getPianoSequence()->insert(noteParams, true);
    this->holdingNotes[key] = noteParams;
}

void MidiRecorder::updateLengthsOfHoldingNotes() const
{
    jassert(this->activeClip != nullptr);
    jassert(this->activeTrack != nullptr);

    if (this->holdingNotes.empty())
    {
        //DBG("Skip 1");
        return;
    }

    const auto currentBeat =
        float(this->getEstimatedPosition() - this->activeClip->getBeat());

    Array<Note> groupBefore;
    Array<Note> groupAfter;

    for (const auto &i : this->holdingNotes)
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
    if (this->activeTrack != nullptr) // a user cleared selection before hitting stop
    {
        this->updateLengthsOfHoldingNotes();
    }

    this->holdingNotes.clear();
}

bool MidiRecorder::finaliseHoldingNote(int key)
{
    jassert(this->activeClip != nullptr);
    jassert(this->activeTrack != nullptr);

    const auto currentBeat =
        float(this->getEstimatedPosition() - this->activeClip->getBeat());

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

Transport &MidiRecorder::getTransport() const noexcept
{
    return this->project.getTransport();
}

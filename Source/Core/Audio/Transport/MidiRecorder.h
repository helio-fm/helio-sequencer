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

#pragma once

class MidiTrack;
class PianoSequence;
class ProjectNode;
class Transport;

#include "Clip.h"
#include "Note.h"
#include "TransportListener.h"

class MidiRecorder final : public MidiInputCallback,
                           public TransportListener,
                           private AsyncUpdater,
                           private Timer
{
public:

    explicit MidiRecorder(ProjectNode &project);

    ~MidiRecorder() override;

    void setTargetScope(const Clip *clip, const String &instrumentId);

private:

    //===------------------------------------------------------------------===//
    // MidiInputCallback
    //===------------------------------------------------------------------===//

    void handleIncomingMidiMessage(MidiInput *source,
        const MidiMessage &message) override;

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onTempoChanged(double) noexcept override;
    void onTotalTimeChanged(double) noexcept override {}
    void onLoopModeChanged(bool, float, float) override {}

    void onSeek(float, double, double) noexcept override;
    void onPlay() noexcept override;
    void onRecord() override;
    void onStop() override;

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//

    void handleAsyncUpdate() override;

    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;

private:

    ProjectNode &project;
    Transport &getTransport() const noexcept;

    const Clip *activeClip = nullptr;
    WeakReference<MidiTrack> activeTrack;
    String lastValidInstrumentId;

    PianoSequence *getPianoSequence() const;

    Array<MidiMessage, CriticalSection> noteOnsBuffer;
    Array<MidiMessage, CriticalSection> noteOffsBuffer;
    Array<MidiMessage> unhandledNoteOffs;

    FlatHashMap<int, Note> holdingNotes;
    void startHoldingNote(MidiMessage message);
    void updateLengthsOfHoldingNotes() const;
    void finaliseAllHoldingNotes();
    bool finaliseHoldingNote(int key);

    double getEstimatedPosition() const;

    // no need for updating too often, I guess:
    static constexpr auto updateTimeHz = 15;

    Atomic<float> lastCorrectPosition = 0.f;
    Atomic<double> lastUpdateTime = 0.0;
    Atomic<double> msPerQuarterNote = Globals::Defaults::msPerBeat;

    Atomic<bool> isPlaying = false;
    Atomic<bool> isRecording = false;
    Atomic<bool> shouldCheckpoint = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};

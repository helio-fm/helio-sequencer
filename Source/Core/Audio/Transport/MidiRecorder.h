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

#pragma once

class Transport;
class UndoStack;
class MidiTrack;
class PianoSequence;

#include "Clip.h"
#include "Note.h"
#include "TransportListener.h"

class MidiRecorder final : public MidiInputCallback,
                           public TransportListener,
                           private AsyncUpdater,
                           private Timer
{
public:

    MidiRecorder(WeakReference<Transport> transport,
        WeakReference<UndoStack> undoStack);

    ~MidiRecorder() override;

    void setSelectedScope(WeakReference<MidiTrack> track, const Clip &clip);

private:

    //===------------------------------------------------------------------===//
    // MidiInputCallback
    //===------------------------------------------------------------------===//

    void handleIncomingMidiMessage(MidiInput *source,
        const MidiMessage &message) override;

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onTempoChanged(double) noexcept;
    void onTotalTimeChanged(double) noexcept {}
    void onSeek(float, double, double) noexcept;
    void onPlay() noexcept;
    void onRecord();
    void onStop();

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//

    void handleAsyncUpdate() override;

    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;

private:

    WeakReference<Transport> transport;
    WeakReference<UndoStack> undoStack;

    Clip activeClip;
    WeakReference<MidiTrack> activeTrack;
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

    Atomic<float> lastCorrectPosition = 0.f;
    Atomic<double> lastUpdateTime = 0.0;
    Atomic<double> msPerQuarterNote = 1.0;

    Atomic<bool> isPlaying = false;
    Atomic<bool> isRecording = false;
    Atomic<bool> shouldCheckpoint = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};

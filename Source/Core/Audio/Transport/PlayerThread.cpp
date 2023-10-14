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

#include "PlayerThread.h"

PlayerThread::PlayerThread(Transport &transport) :
    Thread("PlayerThread"),
    transport(transport) {}

PlayerThread::~PlayerThread()
{
    this->stopThread(PlayerThread::minStopCheckTimeMs * 2);
}

//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

void PlayerThread::startPlayback(Transport::PlaybackContext::Ptr context)
{
    this->context = context;
    this->sequences = this->transport.getPlaybackCache();
    this->startThread(10);
}

void PlayerThread::run()
{
    Array<Instrument *> uniqueInstruments;
    uniqueInstruments.addArray(this->sequences.getUniqueInstruments());

    auto broadcastSeek = [this](Atomic<float> &beat)
    {
        this->transport.broadcastSeek(beat.get(),
            this->context->startBeatTimeMs,
            this->context->totalTimeMs);
    };

    const bool isLooped = this->context->playbackLoopMode;

    this->sequences.seekToTime(this->context->startBeat);

    Atomic<float> previousEventBeat = this->context->startBeat;
    broadcastSeek(previousEventBeat);

    // This hack is here to keep track of still playing events
    // to be able to send noteOff's when playback interrupts.
    struct HoldingNote final
    {
        int key;
        int channel;
        MidiMessageCollector *listener;
    };
    // (some plugins just don't understand allNotesOff message)
    Array<HoldingNote> holdingNotes;
    
    // Some shorthands:
    auto sendMidiStart = [&uniqueInstruments]()
    {
        for (auto &instrument : uniqueInstruments)
        {
            MidiMessage startPlayback(MidiMessage::midiStart());
            startPlayback.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
            instrument->getProcessorPlayer().getMidiMessageCollector().addMessageToQueue(startPlayback);
        }
    };

    auto sendControllerStates = [&uniqueInstruments, this]()
    {
        for (auto &instrument : uniqueInstruments)
        {
            for (int cc = 0; cc < Transport::PlaybackContext::numCCs; ++cc)
            {
                for (int channel = 1; channel <= Globals::numChannels; ++channel)
                {
                    const auto state = this->context->ccStates[cc][channel - 1];
                    if (state < 0) // not present in any track for this channel
                    {
                        continue;
                    }

                    MidiMessage m(MidiMessage::controllerEvent(channel, cc, state));
                    m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
                    instrument->getProcessorPlayer().getMidiMessageCollector().addMessageToQueue(m);
                }
            }
        }
    };

    auto sendHoldingNotesOffAndMidiStop = [&holdingNotes, &uniqueInstruments]()
    {
        for (const auto &holding : holdingNotes)
        {
            MidiMessage noteOff(MidiMessage::noteOff(holding.channel, holding.key, 0.f));
            noteOff.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
            holding.listener->addMessageToQueue(noteOff);
        }
        
        MidiMessage stopPlayback(MidiMessage::midiStop());
        stopPlayback.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        
        for (auto &instrument : uniqueInstruments)
        {
            instrument->getProcessorPlayer()
                .getMidiMessageCollector().addMessageToQueue(stopPlayback);
        }
        
        // Wait until all plugins process the messages in their queues
        Thread::sleep(50);
    };
    
    auto sendTempoChangeToEverybody =
        [&uniqueInstruments](const MidiMessage &tempoEvent)
    {
        for (auto &instrument : uniqueInstruments)
        {
            instrument->getProcessorPlayer().
                getMidiMessageCollector().addMessageToQueue(tempoEvent);
        }
    };

    // And here we go.

    sendMidiStart();
    sendControllerStates();

    double nextEventTimeDelta = 0.0;
    auto currentTimeMs = this->context->startBeatTimeMs;
    Atomic<double> currentTempo = this->context->startBeatTempo;

    while (1)
    {
        CachedMidiMessage wrapper;

        // Handle playback from the last event to the end of track:
        if (!this->sequences.getNextMessage(wrapper))
        {
            nextEventTimeDelta = currentTempo.get() *
                (this->context->endBeat - previousEventBeat.get());

            const uint32 targetTime =
                Time::getMillisecondCounter() + uint32(nextEventTimeDelta);

            // Give thread a chance to exit by checking at least once a, say, second
            while (nextEventTimeDelta > PlayerThread::minStopCheckTimeMs)
            {
                nextEventTimeDelta -= PlayerThread::minStopCheckTimeMs;
                Thread::sleep(PlayerThread::minStopCheckTimeMs);
                if (this->threadShouldExit())
                {
                    sendHoldingNotesOffAndMidiStop();
                    return; // the transport have already stopped
                }
            }

            Time::waitForMillisecondCounter(targetTime);

            if (isLooped)
            {
                this->sequences.seekToTime(this->context->rewindBeat);
                previousEventBeat = this->context->rewindBeat;
                broadcastSeek(previousEventBeat);
                continue;
            }
            else
            {
                while (this->transport.isRecording() && !this->threadShouldExit())
                {
                    Thread::sleep(PlayerThread::minStopCheckTimeMs);
                }

                sendHoldingNotesOffAndMidiStop();

                if (this->threadShouldExit())
                {
                    return; // the transport have already stopped
                }

                this->transport.allNotesControllersAndSoundOff();
                this->transport.stopRecording();
                this->transport.stopPlayback();
                return;
            }
        }

        const auto messageBeat = wrapper.message.getTimeStamp();

        const bool shouldRewind =
            (isLooped && (messageBeat > this->context->endBeat));

        const auto nextEventBeat =
            float(shouldRewind ? this->context->endBeat : messageBeat);

        nextEventTimeDelta = currentTempo.get() * (nextEventBeat - previousEventBeat.get());
        currentTimeMs += nextEventTimeDelta;
        
        jassert(previousEventBeat.get() <= nextEventBeat);
        previousEventBeat = nextEventBeat;

        // Zero-delay check (we're playing a chord or so)
        if (uint32(nextEventTimeDelta) != 0)
        {
            const uint32 targetTime = Time::getMillisecondCounter() + uint32(nextEventTimeDelta);
            while (nextEventTimeDelta > PlayerThread::minStopCheckTimeMs)
            {
                nextEventTimeDelta -= PlayerThread::minStopCheckTimeMs;
                Thread::sleep(PlayerThread::minStopCheckTimeMs);
                if (this->threadShouldExit())
                {
                    sendHoldingNotesOffAndMidiStop();
                    return;
                }
            }

            Time::waitForMillisecondCounter(targetTime);

            if (this->threadShouldExit())
            {
                sendHoldingNotesOffAndMidiStop();
                return;
            }

            broadcastSeek(previousEventBeat);
        }
        
        if (shouldRewind)
        {
            this->sequences.seekToTime(this->context->rewindBeat);
            previousEventBeat = this->context->rewindBeat;
            broadcastSeek(previousEventBeat);
        }
        else
        {
            const int key = wrapper.message.getNoteNumber();
            const int channel = wrapper.message.getChannel();
            wrapper.message.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
            
            // Master tempo event is sent to everybody
            if (wrapper.message.isTempoMetaEvent())
            {
                currentTempo = wrapper.message.getTempoSecondsPerQuarterNote() * 1000.f;
                this->transport.broadcastTempoChanged(currentTempo.get());

                // Sends this to everybody (need to do that for drum-machines) - TODO test
                sendTempoChangeToEverybody(wrapper.message);
            }
            else
            {
                wrapper.listener->addMessageToQueue(wrapper.message);
            }
            
            if (wrapper.message.isNoteOn())
            {
                holdingNotes.add({ key, channel, wrapper.listener });
            }
            
            if (wrapper.message.isNoteOff())
            {
                for (int i = 0; i < holdingNotes.size(); ++i)
                {
                    if (holdingNotes[i].key == key &&
                        holdingNotes[i].channel == channel &&
                        holdingNotes[i].listener == wrapper.listener)
                    {
                        holdingNotes.remove(i);
                        break;
                    }
                }
            }
        }
    }
    
    jassertfalse;
}

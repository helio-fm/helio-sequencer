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

#include "PlayerThread.h"
#include "Instrument.h"
#include "MidiSequence.h"

#define MINIMUM_STOP_CHECK_TIME_MS 1000

PlayerThread::PlayerThread(Transport &transport) :
    Thread("PlayerThread"),
    transport(transport) {}

PlayerThread::~PlayerThread()
{
    this->stopThread(MINIMUM_STOP_CHECK_TIME_MS * 2);
}

//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

void PlayerThread::startPlayback(bool shouldBroadcastTransportEvents /*= true*/)
{
    this->broadcastMode = shouldBroadcastTransportEvents;
    this->startThread(10);
}

void PlayerThread::run()
{
    ProjectSequences sequences = this->transport.getSequences();
    Array<Instrument *> uniqueInstruments(sequences.getUniqueInstruments());
    
    double TPQN = MS_PER_BEAT; // ticks-per-quarter-note
    double nextEventTimeDelta = 0.0;
    
    double tempoAtTheEndOfTrack = 0.0;
    double totalTimeMs = 0.0;
    this->transport.calcTimeAndTempoAt(1.0, totalTimeMs, tempoAtTheEndOfTrack);
    
    const double absStartPosition = this->transport.isLooped() ? this->transport.getLoopStart() : this->transport.getSeekPosition();
    const double absEndPosition = this->transport.isLooped() ? this->transport.getLoopEnd() : 1.0;
    
    double msPerTick = 500.0 / TPQN; // default 120 BPM
    double currentTimeMs = 0.0;
    this->transport.calcTimeAndTempoAt(absStartPosition,
                                       currentTimeMs,
                                       msPerTick);
    
    if (this->broadcastMode)
    {
        this->transport.broadcastTempoChanged(msPerTick);
    }
    
    const double totalTime = this->transport.getTotalTime();
    const double startPositionInTime = round(absStartPosition * totalTime);
    const double endPositionInTime = round(absEndPosition * totalTime);
    
    sequences.seekToTime(startPositionInTime);
    double prevTimeStamp = startPositionInTime;
    
    // This hack is here to keep track of still playing events
    // to be able to send noteOff's when playback interrupts.
    struct HoldingNote
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
            instrument->getProcessorPlayer().getMidiMessageCollector().addMessageToQueue(stopPlayback);
        }
        
        // Wait until all plugins process the messages in their queues
        Thread::sleep(50);
    };
    
    auto sendTempoChangeToEverybody = [&uniqueInstruments](const MidiMessage &tempoEvent)
    {
        for (auto &instrument : uniqueInstruments)
        {
            instrument->getProcessorPlayer().getMidiMessageCollector().addMessageToQueue(tempoEvent);
        }
    };
    
    // And here we go.
    sendMidiStart();
    
    while (1)
    {
        MessageWrapper wrapper;

        // Handle playback from the last event to the end of track:
        if (!sequences.getNextMessage(wrapper))
        {
            nextEventTimeDelta = msPerTick * (endPositionInTime - prevTimeStamp);
            const uint32 targetTime = Time::getMillisecondCounter() + uint32(nextEventTimeDelta);

            // Give thread a chance to exit by checking at least once a, say, second
            while (nextEventTimeDelta > MINIMUM_STOP_CHECK_TIME_MS)
            {
                nextEventTimeDelta -= MINIMUM_STOP_CHECK_TIME_MS;
                Thread::sleep(MINIMUM_STOP_CHECK_TIME_MS);
                if (this->threadShouldExit())
                {
                    sendHoldingNotesOffAndMidiStop();
                    return;
                }
            }

            Time::waitForMillisecondCounter(targetTime);

            if (this->transport.isLooped())
            {
                //Logger::writeToLog("Seek to time " + String(startPositionInTime));
                sequences.seekToTime(startPositionInTime);
                prevTimeStamp = startPositionInTime;
                continue;
            }
            else
            {
                //Logger::writeToLog("Track finished");
                sendHoldingNotesOffAndMidiStop();
                this->transport.allNotesControllersAndSoundOff();

                if (this->broadcastMode)
                {
                    this->transport.seekToPosition(this->transport.getSeekPosition());
                    this->transport.broadcastStop();
                }
                return;
            }
        }

        const bool shouldRewind =
            (this->transport.isLooped() &&
            (wrapper.message.getTimeStamp() > endPositionInTime));

        const double nextEventTimeStamp =
            shouldRewind ? endPositionInTime : wrapper.message.getTimeStamp();

        nextEventTimeDelta = msPerTick * (nextEventTimeStamp - prevTimeStamp);
        currentTimeMs += nextEventTimeDelta;
        prevTimeStamp = nextEventTimeStamp;

        // Zero-delay check (we're playing a chord or so)
        if (uint32(nextEventTimeDelta) != 0)
        {
            const uint32 targetTime = Time::getMillisecondCounter() + uint32(nextEventTimeDelta);
            while (nextEventTimeDelta > MINIMUM_STOP_CHECK_TIME_MS)
            {
                nextEventTimeDelta -= MINIMUM_STOP_CHECK_TIME_MS;
                Thread::sleep(MINIMUM_STOP_CHECK_TIME_MS);
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

            if (this->broadcastMode)
            {
                this->transport.broadcastSeek(prevTimeStamp / totalTime, currentTimeMs, totalTimeMs);
            }
        }
        
        if (shouldRewind)
        {
            sequences.seekToTime(startPositionInTime);
            prevTimeStamp = startPositionInTime;
        }
        else
        {
            const int key = wrapper.message.getNoteNumber();
            const int channel = wrapper.message.getChannel();
            wrapper.message.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
            
            // Master tempo event is sent to everybody
            if (wrapper.message.isTempoMetaEvent())
            {
                msPerTick = wrapper.message.getTempoSecondsPerQuarterNote() * 1000.f / TPQN;

                if (this->broadcastMode)
                {
                    this->transport.broadcastTempoChanged(msPerTick);
                }

                // Sends this to everybody (need to do that for drum-machines) - TODO test
                sendTempoChangeToEverybody(wrapper.message);
            }
            else
            {
                //Logger::writeToLog(String(wrapper.message.getNoteNumber()));
                wrapper.listener->addMessageToQueue(wrapper.message);
            }
            
            if (wrapper.message.isNoteOn())
            {
                holdingNotes.add(HoldingNote({key, channel, wrapper.listener}));
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
    
    Logger::writeToLog("Never happens.");
}

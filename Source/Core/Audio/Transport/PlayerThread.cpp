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
    this->speedMultiplier = 1.f;
    this->speedMultiplierChanged = false;
    this->startThread(10);
}

void PlayerThread::setSpeedMultiplier(float multiplier)
{
    if (this->speedMultiplier.get() != multiplier)
    {
        this->speedMultiplier = multiplier;
        this->speedMultiplierChanged = true;
    }
}

void PlayerThread::run()
{
    Array<Instrument *> uniqueInstruments;
    uniqueInstruments.addArray(this->sequences.getUniqueInstruments());

    auto broadcastSeekAndTempo = [this](float beat)
    {
        this->transport.broadcastSeek(beat);

        if (this->speedMultiplierChanged.get())
        {
            this->speedMultiplierChanged = false;
            this->transport.broadcastCurrentTempoChanged(
                this->currentTempo.get() / this->speedMultiplier.get());
        }
    };

    const bool isLooped = this->context->playbackLoopMode;

    this->sequences.seekToTime(this->context->startBeat);

    Atomic<float> previousEventBeat = this->context->startBeat;
    broadcastSeekAndTempo(previousEventBeat.get());

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
    double deltaTimePassed = 0.0;
    this->currentTempo = this->context->startBeatTempo;

    while (1)
    {
        CachedMidiMessage wrapper;

        // Handle playback from the last event to the end of the track:
        if (!this->sequences.getNextMessage(wrapper))
        {
            const auto previousEventTime = Time::getMillisecondCounter();
            const auto beatDelta = this->context->endBeat - previousEventBeat.get();

            nextEventTimeDelta = beatDelta *
                (this->currentTempo.get() / this->speedMultiplier.get());

            deltaTimePassed = 0.0;
            while (deltaTimePassed < (nextEventTimeDelta - PlayerThread::minStopCheckTimeMs))
            {
                const auto a = Time::getMillisecondCounter();
                Thread::sleep(PlayerThread::minStopCheckTimeMs);
                const auto shouldExit = this->threadShouldExit();
                const auto b = Time::getMillisecondCounter();
                deltaTimePassed += double(b - a);

                if (shouldExit)
                {
                    sendHoldingNotesOffAndMidiStop();
                    return; // the transport has already stopped
                }

                if (this->speedMultiplierChanged.get())
                {
                    const auto beatsPassed = beatDelta * float(deltaTimePassed / nextEventTimeDelta);
                    broadcastSeekAndTempo(previousEventBeat.get() + beatsPassed);
                    nextEventTimeDelta = deltaTimePassed + ((beatDelta - beatsPassed) *
                        (this->currentTempo.get() / this->speedMultiplier.get()));
                }
            }

            Time::waitForMillisecondCounter(previousEventTime + uint32(nextEventTimeDelta));

            if (isLooped)
            {
                this->sequences.seekToTime(this->context->rewindBeat);
                previousEventBeat = this->context->rewindBeat;
                broadcastSeekAndTempo(previousEventBeat.get());
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
                    return; // the transport has already stopped
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

        jassert(previousEventBeat.get() <= nextEventBeat);
        const auto beatDelta = nextEventBeat - previousEventBeat.get();

        nextEventTimeDelta = beatDelta *
            (this->currentTempo.get() / this->speedMultiplier.get());

        // Zero-delay check (we're playing a chord or so)
        if (uint32(nextEventTimeDelta) != 0)
        {
            const auto previousEventTime = Time::getMillisecondCounter();

            deltaTimePassed = 0.0;
            while (deltaTimePassed < (nextEventTimeDelta - PlayerThread::minStopCheckTimeMs))
            {
                const auto a = Time::getMillisecondCounter();
                Thread::sleep(PlayerThread::minStopCheckTimeMs);
                const auto shouldExit = this->threadShouldExit();
                const auto b = Time::getMillisecondCounter();
                deltaTimePassed += double(b - a);

                if (shouldExit)
                {
                    sendHoldingNotesOffAndMidiStop();
                    return;
                }

                if (this->speedMultiplierChanged.get())
                {
                    const auto beatsPassed = beatDelta * float(deltaTimePassed / nextEventTimeDelta);
                    broadcastSeekAndTempo(previousEventBeat.get() + beatsPassed);
                    nextEventTimeDelta = deltaTimePassed + ((beatDelta - beatsPassed) *
                        (this->currentTempo.get() / this->speedMultiplier.get()));
                }
            }

            Time::waitForMillisecondCounter(previousEventTime + uint32(nextEventTimeDelta));

            if (this->threadShouldExit())
            {
                sendHoldingNotesOffAndMidiStop();
                return;
            }

            broadcastSeekAndTempo(nextEventBeat);
        }
        
        if (shouldRewind)
        {
            this->sequences.seekToTime(this->context->rewindBeat);
            previousEventBeat = this->context->rewindBeat;
            broadcastSeekAndTempo(previousEventBeat.get());
        }
        else
        {
            previousEventBeat = nextEventBeat;
     
            const int key = wrapper.message.getNoteNumber();
            const int channel = wrapper.message.getChannel();
            wrapper.message.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
            
            // Master tempo event is sent to everybody
            if (wrapper.message.isTempoMetaEvent())
            {
                this->currentTempo = wrapper.message.getTempoSecondsPerQuarterNote() * 1000.f;
                this->transport.broadcastCurrentTempoChanged(this->currentTempo.get() / this->speedMultiplier.get());

                // Sends this to everybody (need to do that for drum-machines) - TODO test
                sendTempoChangeToEverybody(wrapper.message);
            }
            else
            {
                wrapper.listener->addMessageToQueue(wrapper.message);
            }
            
            // todo automating individual plugin node parameters
            //wrapper.instrument->getNodeForId(node id)->getProcessor()->getParameters()[param index]->setValue()

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

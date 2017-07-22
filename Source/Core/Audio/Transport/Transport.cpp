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
#include "Transport.h"
#include "Instrument.h"
#include "OrchestraPit.h"
#include "PlayerThread.h"
#include "RendererThread.h"
#include "MidiLayer.h"
#include "MidiEvent.h"

#include "App.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "HybridRoll.h"

#if PLAYER_THREAD_SENDS_SEEK_EVENTS
#   define PLAYER_THREAD_STOP_TIME_MS 1500
#else
#   define PLAYER_THREAD_STOP_TIME_MS 100
#endif

Transport::Transport(OrchestraPit &orchestraPit) :
    orchestra(orchestraPit),
    seekPosition(0.0),
    trackStartMs(0.0),
    trackEndMs(0.0),
    sequencesAreOutdated(true),
    totalTime(Transport::millisecondsPerBeat * 8),
    loopedMode(false),
    loopStart(0.0),
    loopEnd(0.0),
    projectFirstBeat(0.f),
    projectLastBeat(DEFAULT_NUM_BARS * NUM_BEATS_IN_BAR)
{
    this->player = new PlayerThread(*this);
    this->renderer = new RendererThread(*this);

    this->orchestra.addOrchestraListener(this);
}

Transport::~Transport()
{
    this->orchestra.removeOrchestraListener(this);
    
    if (this->player->isThreadRunning())
    {
        this->player->stopThread(500);
    }
    
    if (this->renderer->isRecording())
    {
        this->renderer->stop();
    }
    
    this->transportListeners.clear();
}

String Transport::getTimeString(double timeMs, bool includeMilliseconds)
{
    RelativeTime timeSec(timeMs / 1000.0);
    return Transport::getTimeString(timeSec, includeMilliseconds);
}

String Transport::getTimeString(const RelativeTime &relTime, bool includeMilliseconds)
{
    String res;
    
    //if (relTime < RelativeTime(0))
    if (relTime.inSeconds() <= -1.0) // because '-0.0' is no cool
    {
        //return "0:0";
        res = res + "-";
    }
    
    
    //int n = std::abs(int(time.inHours()));
    
    //if (n > 0)
    //{
    //    res = res + String(n);
    //}
    
    int n = std::abs(int(relTime.inMinutes())) /*% 60*/;
    
    //if (n > 0)
    {
        res = res + String(n);
    }
    
    n = std::abs(int(relTime.inSeconds())) % 60;
    //res = res + (res.isEmpty() ? "" : "'") + String(n) + "''";
    res = res + (res.isEmpty() ? "" : ":") + String(n);
    
    if (includeMilliseconds)
    {
        n = std::abs(int(relTime.inMilliseconds())) % 1000;
        
        if (n > 0)
        {
            res = res + (res.isEmpty() ? "" : ":") + String(n);
        }
    }
    
    return res;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

double Transport::getSeekPosition() const
{
    ScopedReadLock lock(this->seekPositionLock);
    return this->seekPosition;
}

void Transport::setSeekPosition(const double absPosition)
{
    ScopedWriteLock lock(this->seekPositionLock);
    this->seekPosition = absPosition;
}

double Transport::getTotalTime() const
{
    ScopedReadLock lock(this->totalTimeLock);
    return this->totalTime;
}

void Transport::setTotalTime(const double val)
{
    ScopedWriteLock lock(this->totalTimeLock);
    this->totalTime = val;
}


//===----------------------------------------------------------------------===//
// Transport
//===----------------------------------------------------------------------===//

void Transport::rebuildSequencesInRealtime()
{
    // todo sequences write lock
    //this->rebuildSequencesIfNeeded();
    //this->sequences.seekToTime(this->lastSeekTimeStamp);
}

void Transport::seekToPosition(double absPosition)
{
    double timeMs = 0.0;
    double realLengthMs = 0.0;
    double tempo = 0.0;
    this->calcTimeAndTempoAt(absPosition, timeMs, tempo);
    this->calcTimeAndTempoAt(1.0, realLengthMs, tempo);
    
    //Logger::writeToLog("absPosition " + String(absPosition));
    this->setSeekPosition(absPosition);
    this->broadcastSeek(absPosition, timeMs, realLengthMs);
}

void Transport::probeSoundAt(double absTrackPosition, const MidiLayer *limitToLayer)
{
    this->rebuildSequencesIfNeeded();
    
    const double targetFlatTime = round(this->getTotalTime() * absTrackPosition);
    const auto sequencesToProbe(this->sequences.getAllFor(limitToLayer));
    
    for (auto && i : sequencesToProbe)
    {
        SequenceWrapper::Ptr seq(i);

        for (int j = 0; j < seq->sequence.getNumEvents(); ++j)
        {
            MidiMessageSequence::MidiEventHolder *noteOnHolder = seq->sequence.getEventPointer(j);
            
            if (MidiMessageSequence::MidiEventHolder *noteOffHolder = noteOnHolder->noteOffObject)
            {
                const double noteOn(noteOnHolder->message.getTimeStamp());
                const double noteOff(noteOffHolder->message.getTimeStamp());
                
                if (noteOn <= targetFlatTime && noteOff > targetFlatTime)
                {
                    MidiMessage messageTimestampedAsNow(noteOnHolder->message);
                    messageTimestampedAsNow.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
                    seq->listener->addMessageToQueue(messageTimestampedAsNow);
                }
            }
        }
    }
}

void Transport::startPlayback()
{
    this->rebuildSequencesIfNeeded();

    if (this->player->isThreadRunning() &&
        !this->player->threadShouldExit())
    {
        this->player->stopThread(PLAYER_THREAD_STOP_TIME_MS);
        this->allNotesControllersAndSoundOff();
    }
    
    this->loopedMode = false;
    
    this->player->startThread(10);
    this->broadcastPlay();
}

void Transport::startPlaybackLooped(double absLoopStart, double absLoopEnd)
{
    this->rebuildSequencesIfNeeded();
    
    if (this->player->isThreadRunning() &&
        !this->player->threadShouldExit())
    {
        this->player->stopThread(PLAYER_THREAD_STOP_TIME_MS);
        this->allNotesControllersAndSoundOff();
    }
    
    this->loopedMode = true;
    this->loopStart = jmax(0.0, absLoopStart);
    this->loopEnd = jmin(1.0, absLoopEnd);
    
    this->player->startThread(10);
    this->broadcastPlay();
}

void Transport::stopPlayback()
{
    if (this->player->isThreadRunning() &&
        !this->player->threadShouldExit())
    {
        this->player->stopThread(PLAYER_THREAD_STOP_TIME_MS);
        this->allNotesControllersAndSoundOff();
        this->loopedMode = false;
        this->seekToPosition(this->getSeekPosition());
        this->broadcastStop();
    }
}

bool Transport::isPlaying() const
{
    return this->player->isThreadRunning();
}

bool Transport::isLooped() const
{
    return this->loopedMode;
}

double Transport::getLoopStart() const
{
    return this->loopStart;
}

double Transport::getLoopEnd() const
{
    return this->loopEnd;
}


void Transport::startRender(const String &fileName)
{
    if (this->renderer->isRecording())
    {
        return;
    }
    
    App::Workspace().getAudioCore().mute();
    
    File file(File::getCurrentWorkingDirectory().getChildFile(fileName));
    this->renderer->startRecording(file);
}

void Transport::stopRender()
{
    if (! this->renderer->isRecording())
    {
        return;
    }
    
    this->renderer->stop();
    
    // a dirty hack
    App::Workspace().getAudioCore().unmute();
    //this->allNotesControllersAndSoundOff();
    App::Workspace().getAudioCore().unmute();
}

bool Transport::isRendering() const
{
    return this->renderer->isRecording();
}

float Transport::getRenderingPercentsComplete() const
{
    return this->renderer->getPercentsComplete();
}


//===----------------------------------------------------------------------===//
// Sending messages at realtime
//===----------------------------------------------------------------------===//

void Transport::sendMidiMessage(const String &layerId, const MidiMessage &message) const
{
    MidiMessage messageTimestampedAsNow(message);
    
#if HELIO_MOBILE
    // iSEM tends to hang >_< if too many messages are send simultaniously
    messageTimestampedAsNow.setTimeStamp(Time::getMillisecondCounter() + (rand() % 50));
#elif HELIO_DESKTOP
    messageTimestampedAsNow.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
#endif
    
    MidiMessageCollector *collector =
    &this->linksCache[layerId]->getProcessorPlayer().getMidiMessageCollector();
    
    collector->addMessageToQueue(messageTimestampedAsNow);
}

void Transport::allNotesAndControllersOff() const
{
	const int c = 1;
    //for (int c = 1; c <= 16; ++c)
    {
        const MidiMessage notesOff(MidiMessage::allNotesOff(c));
        const MidiMessage controllersOff(MidiMessage::allControllersOff(c));
        
        Array<MidiMessageCollector *>duplicateCollectors;
        
        for (int l = 0; l < this->layersCache.size(); ++l)
        {
            const String &layerId =
            this->layersCache.getUnchecked(l)->getLayerIdAsString();
            
            MidiMessageCollector *collector =
            &this->linksCache[layerId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! duplicateCollectors.contains(collector))
            {
                this->sendMidiMessage(layerId, notesOff);
                this->sendMidiMessage(layerId, controllersOff);
                duplicateCollectors.add(collector);
            }
        }
    }
}

void Transport::allNotesControllersAndSoundOff() const
{
	const int c = 1;
	//for (int c = 1; c <= 16; ++c)
	{
        const MidiMessage notesOff(MidiMessage::allNotesOff(c));
        const MidiMessage soundOff(MidiMessage::allSoundOff(c));
        const MidiMessage controllersOff(MidiMessage::allControllersOff(c));
        
        Array<MidiMessageCollector *>duplicateCollectors;
        
        for (int l = 0; l < this->layersCache.size(); ++l)
        {
            const String &layerId =
            this->layersCache.getUnchecked(l)->getLayerIdAsString();
            
            MidiMessageCollector *collector =
            &this->linksCache[layerId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! duplicateCollectors.contains(collector))
            {
                this->sendMidiMessage(layerId, notesOff);
                this->sendMidiMessage(layerId, controllersOff);
                this->sendMidiMessage(layerId, soundOff);
                duplicateCollectors.add(collector);
            }
        }
    }
}


//===----------------------------------------------------------------------===//
// OrchestraListener
//===----------------------------------------------------------------------===//

void Transport::instrumentAdded(Instrument *instrument)
{
    this->stopPlayback();
    
    // invalidate sequences as they use pointers to the players too
    this->sequencesAreOutdated = true;

    for (int i = 0; i < this->layersCache.size(); ++i)
    {
        this->updateLinkForLayer(this->layersCache.getUnchecked(i));
    }
}

void Transport::instrumentRemoved(Instrument *instrument)
{
    // the instrument stack have still not changed here,
    // so just stop the playback before it's too late
    this->stopPlayback();
}

void Transport::instrumentRemovedPostAction()
{
    this->sequencesAreOutdated = true;

    for (int i = 0; i < this->layersCache.size(); ++i)
    {
        this->updateLinkForLayer(this->layersCache.getUnchecked(i));
    }
}


//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void Transport::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    // todo stop playback only if the event is in future and getControllerNumber == 0 (not an automation)
    
    if (this->player->isThreadRunning())
    { this->stopPlayback(); }
    
    // a hack
    if (newEvent.getLayer()->getControllerNumber() == MidiLayer::tempoController)
    {
        this->seekToPosition(this->getSeekPosition());
    }
    
    this->sequencesAreOutdated = true;
}

void Transport::onAddMidiEvent(const MidiEvent &event)
{
    // todo stop playback only if the event is in future and getControllerNumber == 0 (not an automation)

    if (this->player->isThreadRunning())
    { this->stopPlayback(); }
    
    // a hack
    if (event.getLayer()->getControllerNumber() == MidiLayer::tempoController)
    {
        this->seekToPosition(this->getSeekPosition());
    }
    
    this->sequencesAreOutdated = true;
}

void Transport::onRemoveMidiEvent(const MidiEvent &event)
{
    // todo stop playback only if the event is in future and getControllerNumber == 0 (not an automation)

    if (this->player->isThreadRunning())
    { this->stopPlayback(); }
    
    this->sequencesAreOutdated = true;
}

void Transport::onPostRemoveMidiEvent(MidiLayer *const layer)
{
    if (this->player->isThreadRunning())
    { this->stopPlayback(); }
    
    // a hack to re-calculate length and current time
    if (layer->getControllerNumber() == MidiLayer::tempoController)
    {
        this->seekToPosition(this->getSeekPosition());
    }
    
    this->sequencesAreOutdated = true;
}

void Transport::onChangeTrack(MidiLayer *const layer, Pattern *const pattern /*= nullptr*/)
{
    if (this->player->isThreadRunning())
    { this->stopPlayback(); }
    
    this->sequencesAreOutdated = true;
    this->updateLinkForLayer(layer);
}

void Transport::onAddTrack(MidiLayer *const layer, Pattern *const pattern /*= nullptr*/)
{
    if (this->player->isThreadRunning())
    {this->stopPlayback(); }
    
    this->sequencesAreOutdated = true;
    this->layersCache.addIfNotAlreadyThere(layer);
    this->updateLinkForLayer(layer);
}

void Transport::onRemoveTrack(MidiLayer *const layer, Pattern *const pattern /*= nullptr*/)
{
    if (this->player->isThreadRunning())
    {this->stopPlayback(); }
    
    this->sequencesAreOutdated = true;
    this->layersCache.removeAllInstancesOf(layer);
    this->removeLinkForLayer(layer);
}

void Transport::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    if (this->player->isThreadRunning())
    {
        this->stopPlayback();
    }
    
    const double lastSeekPosition = float(this->getSeekPosition());
    const double seekBeat = this->projectFirstBeat + ((this->projectLastBeat - this->projectFirstBeat) * lastSeekPosition); // may be 0
    //const double roundSeekBeat = round(seekBeat * 1000.0) / 1000.0;
    const double newBeatRange = (lastBeat - firstBeat); // may also be 0
    const double newSeekPosition = ((newBeatRange == 0.0) ? 0.0 : ((seekBeat - firstBeat) / newBeatRange));
    
    //===------------------------------------------------------------------===//
    //          |----------- 0.7 ----|
    // |--------+----------- 0.5 ----+---------------|
    //===------------------------------------------------------------------===//
    //  1. calc seek position as beat
    //  2. calc (seekBeat - newFirstBeat) / (newLastBeat - newFirstBeat)
    //===------------------------------------------------------------------===//
    
    this->trackStartMs = firstBeat * Transport::millisecondsPerBeat;
    this->trackEndMs = lastBeat * Transport::millisecondsPerBeat;
    this->setTotalTime(this->trackEndMs - this->trackStartMs);
    
    // real track total time changed
    double tempo = 0.0;
    double realLengthMs = 0.0;
    this->calcTimeAndTempoAt(1.0, realLengthMs, tempo);
    this->broadcastTotalTimeChanged(realLengthMs);
    
    //Logger::writeToLog("newSeekPosition = " + String(newSeekPosition));
    
    // seek also changed
    this->seekToPosition(newSeekPosition);
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;
}


//===----------------------------------------------------------------------===//
// Real track length calc
//===----------------------------------------------------------------------===//

void Transport::calcTimeAndTempoAt(const double targetAbsPosition,
                                   double &outTimeMs, double &outTempo)
{
    this->rebuildSequencesIfNeeded();
    this->sequences.seekToZeroIndexes();
    
    const double TPQN = Transport::millisecondsPerBeat; // ticks-per-quarter-note
    const double targetTime = round(targetAbsPosition * this->getTotalTime());
    
    outTimeMs = 0.0;
    outTempo = 250.0 / TPQN; // default 240 BPM
    
    double prevTimestamp = 0.0;
    double nextEventTimeDelta = 0.0;
    bool foundFirstTempoEvent = false;
    
    MessageWrapper wrapper;
    
    while (this->sequences.getNextMessage(wrapper))
    {
        const double nextAbsPosition = (wrapper.message.getTimeStamp() / this->getTotalTime());
        
        // foundFirstTempoEvent нужен для того, чтоб темп до первого события был равен темпу на первом событии
        if (nextAbsPosition > targetAbsPosition &&
            foundFirstTempoEvent)
        {
            break;
        }
        
        nextEventTimeDelta = outTempo * (wrapper.message.getTimeStamp() - prevTimestamp);
        outTimeMs += nextEventTimeDelta;
        prevTimestamp = wrapper.message.getTimeStamp();
        
        if (wrapper.message.isTempoMetaEvent())
        {
            outTempo = wrapper.message.getTempoSecondsPerQuarterNote() * 1000.f / TPQN;
            foundFirstTempoEvent = true;
        }
    }
    
    nextEventTimeDelta = outTempo * (targetTime - prevTimestamp);
    outTimeMs += nextEventTimeDelta;
}

MidiMessage Transport::findFirstTempoEvent()
{
    this->rebuildSequencesIfNeeded();
    this->sequences.seekToZeroIndexes();
    
    MessageWrapper wrapper;
    
    while (this->sequences.getNextMessage(wrapper))
    {
        if (wrapper.message.isTempoMetaEvent())
        {
            return wrapper.message;
        }
    }
    
    return MidiMessage::tempoMetaEvent(Transport::millisecondsPerBeat * 1000);
}


//===----------------------------------------------------------------------===//
// Sequences management
//===----------------------------------------------------------------------===//

void Transport::rebuildSequencesIfNeeded()
{
    if (this->sequencesAreOutdated)
    {
        this->sequences.clear();
        
        for (int i = 0; i < this->layersCache.size(); ++i)
        {
            const MidiLayer *layer = this->layersCache.getUnchecked(i);
            MidiMessageSequence sequence(layer->exportMidi());
            sequence.addTimeToMessages(-this->trackStartMs);
            
            if (sequence.getNumEvents() > 0)
            {
                Instrument *targetInstrument = this->linksCache[layer->getLayerId().toString()];
                auto wrapper = new SequenceWrapper();
                wrapper->layer = layer;
                wrapper->sequence = sequence;
                wrapper->currentIndex = 0;
                wrapper->instrument = targetInstrument;
                wrapper->listener = &targetInstrument->getProcessorPlayer().getMidiMessageCollector();
                this->sequences.addWrapper(wrapper);
            }
        }
        
        this->sequencesAreOutdated = false;
    }
}

ProjectSequences Transport::getSequences()
{
    // todo add lock
    return this->sequences;
}

void Transport::updateLinkForLayer(const MidiLayer *layer)
{
//    Instrument *targetInstrument = this->orchestra.findInstrumentById(layer->getInstrumentId());
//    
//    if (targetInstrument != nullptr)
//    {
//        this->linksCache.set(layer->getLayerId().toString(), targetInstrument);
//        return;
//    }
    
    const Array<Instrument *> instruments = this->orchestra.getInstruments();
    
    // check by ids
    for (int i = 1; i < instruments.size(); ++i)
    {
        Instrument *instrument = instruments.getUnchecked(i);
        
        if (layer->getInstrumentId().contains(instrument->getInstrumentID()))
        {
            // corresponding node already exists, lets addd
            this->linksCache.set(layer->getLayerId().toString(), instrument);
            return;
        }
    }
    
    // check by hashes
    for (int i = 1; i < instruments.size(); ++i)
    {
        Instrument *instrument = instruments.getUnchecked(i);
        
        if (layer->getInstrumentId().contains(instrument->getInstrumentHash()))
        {
            this->linksCache.set(layer->getLayerId().toString(), instrument);
            return;
        }
    }
    
    // set default instrument, if none found
    this->linksCache.set(layer->getLayerId().toString(), this->orchestra.getInstruments()[0]);
}

void Transport::removeLinkForLayer(const MidiLayer *layer)
{
    this->linksCache.remove(layer->getLayerId().toString());
}


//===----------------------------------------------------------------------===//
// Transport Listeners
//===----------------------------------------------------------------------===//

void Transport::addTransportListener(TransportListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->transportListeners.add(listener);
}

void Transport::removeTransportListener(TransportListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->transportListeners.remove(listener);
}

void Transport::broadcastSeek(const double newPosition,
                              const double currentTimeMs, const double totalTimeMs)
{
    // todo remember last seek position
    this->transportListeners.call(&TransportListener::onSeek, newPosition, currentTimeMs, totalTimeMs);
}

void Transport::broadcastTempoChanged(const double newTempo)
{
    this->transportListeners.call(&TransportListener::onTempoChanged, newTempo);
}

void Transport::broadcastTotalTimeChanged(const double timeMs)
{
    this->transportListeners.call(&TransportListener::onTotalTimeChanged, timeMs);
}

void Transport::broadcastPlay()
{
    this->transportListeners.call(&TransportListener::onPlay);
}

void Transport::broadcastStop()
{
    this->transportListeners.call(&TransportListener::onStop);
}

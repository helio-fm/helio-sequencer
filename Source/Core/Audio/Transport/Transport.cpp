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
#include "MidiSequence.h"
#include "MidiEvent.h"
#include "MidiTrack.h"
#include "Clip.h"
#include "Pattern.h"
#include "App.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "HybridRoll.h"
#include "SerializationKeys.h"

class PlayerThreadPool final
{
public:

    PlayerThreadPool(Transport &transport, int poolSize = 5) :
        transport(transport),
        minPoolSize(poolSize)
    {
        for (int i = 0; i < poolSize; ++i)
        {
            this->players.add(new PlayerThread(transport));
        }

        this->currentPlayer = this->findNextFreePlayer();
    }

    ~PlayerThreadPool()
    {
        // Send exit signal to all threads before they are stopped forcefully,
        // so that we don't have to wait for each one separately.
        for (int i = 0; i < this->players.size(); ++i)
        {
            this->players.getUnchecked(i)->signalThreadShouldExit();
        }
    }

    void startPlayback(bool shouldBroadcastTransportEvents = true)
    {
        if (this->currentPlayer->isThreadRunning())
        {
            this->currentPlayer->signalThreadShouldExit();
            this->currentPlayer = findNextFreePlayer();
        }

        this->currentPlayer->startPlayback(shouldBroadcastTransportEvents);
    }
    
    void stopPlayback()
    {
        if (this->currentPlayer->isThreadRunning())
        {
            // Just signal player to stop:
            // it might be waiting for the next midi event, so it won't stop immediately
            this->currentPlayer->signalThreadShouldExit();
        }
    }

    bool isPlaying() const
    {
        return (this->currentPlayer->isThreadRunning() &&
            !this->currentPlayer->threadShouldExit());
    }

private:

    PlayerThread *findNextFreePlayer()
    {
        this->cleanup();

        for (const auto player : this->players)
        {
            if (!player->isThreadRunning())
            {
                return player;
            }
        }

        Logger::writeToLog("Warning: all playback threads are busy, adding one");
        return this->players.add(new PlayerThread(transport));
    }

    void cleanup()
    {
        // Since all new players are added last,
        // first ones are most likely to be stopped,
        // so simply try to cleanup from the beginning until we meet a busy one:
        while (this->players.size() > this->minPoolSize)
        {
            if (this->players.getFirst() == this->currentPlayer ||
                this->players.getFirst()->isThreadRunning())
            {
                return;
            }

            Logger::writeToLog("Removing a stale playback thread");
            this->players.remove(0);
        }
    }

    Transport &transport;
    int minPoolSize;

    OwnedArray<PlayerThread> players;
    PlayerThread *currentPlayer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerThreadPool)
};

Transport::Transport(OrchestraPit &orchestraPit) :
    orchestra(orchestraPit),
    seekPosition(0.0),
    trackStartMs(0.0),
    trackEndMs(0.0),
    sequencesAreOutdated(true),
    totalTime(MS_PER_BEAT * 8.0),
    loopedMode(false),
    loopStart(0.0),
    loopEnd(0.0),
    projectFirstBeat(0.f),
    projectLastBeat(DEFAULT_NUM_BARS * BEATS_PER_BAR)
{
    this->player = new PlayerThreadPool(*this);
    this->renderer = new RendererThread(*this);
    this->orchestra.addOrchestraListener(this);
}

Transport::~Transport()
{
    this->orchestra.removeOrchestraListener(this);
    this->renderer = nullptr;
    this->player = nullptr;
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

double Transport::getSeekPosition() const noexcept
{
    return this->seekPosition.get();
}

void Transport::setSeekPosition(const double absPosition)
{
    this->seekPosition = absPosition;
}

double Transport::getTotalTime() const noexcept
{
    return this->totalTime.get();
}

void Transport::setTotalTime(const double val)
{
    this->totalTime = val;
}

//===----------------------------------------------------------------------===//
// Transport
//===----------------------------------------------------------------------===//

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

void Transport::probeSoundAt(double absTrackPosition, const MidiSequence *limitToLayer)
{
    this->rebuildSequencesIfNeeded();
    
    const double targetFlatTime = round(this->getTotalTime() * absTrackPosition);
    const auto sequencesToProbe(this->sequences.getAllFor(limitToLayer));
    
    for (const auto &seq : sequencesToProbe)
    {
        for (int j = 0; j < seq->midiMessages.getNumEvents(); ++j)
        {
            MidiMessageSequence::MidiEventHolder *noteOnHolder = seq->midiMessages.getEventPointer(j);
            
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

// Only used in a key signature dialog to test how scales sound
void Transport::probeSequence(const MidiMessageSequence &sequence)
{
    this->sequences.clear();
    this->sequencesAreOutdated = true; // will update on the next playback
    this->loopedMode = false;

    MidiMessageSequence fixedSequence(sequence);
    const double startPositionInTime = round(this->getSeekPosition() * this->getTotalTime());
    fixedSequence.addTimeToMessages(startPositionInTime);

    // using the last instrument (TODO something more clever in the future)
    Instrument *targetInstrument = this->orchestra.getInstruments().getLast();
    auto wrapper = new SequenceWrapper();
    wrapper->track = nullptr;
    wrapper->midiMessages = fixedSequence;
    wrapper->currentIndex = 0;
    wrapper->instrument = targetInstrument;
    wrapper->listener = &targetInstrument->getProcessorPlayer().getMidiMessageCollector();
    this->sequences.addWrapper(wrapper);

    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
    }

    this->player->startPlayback(false);
}

void Transport::startPlayback()
{
    this->rebuildSequencesIfNeeded();
    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
    }
    
    this->loopedMode = false;
    this->player->startPlayback();
    this->broadcastPlay();
}

void Transport::startPlaybackLooped(double absLoopStart, double absLoopEnd)
{
    this->rebuildSequencesIfNeeded();
    
    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
    }
    
    this->loopedMode = true;
    this->loopStart = jmax(0.0, absLoopStart);
    this->loopEnd = jmin(1.0, absLoopEnd);
    
    this->player->startPlayback();
    this->broadcastPlay();
}

void Transport::stopPlayback()
{
    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
        this->loopedMode = false;
        this->seekToPosition(this->getSeekPosition());
        this->broadcastStop();
    }
}

void Transport::toggleStatStopPlayback()
{
    this->isPlaying() ? this->stopPlayback() : this->startPlayback();
}

bool Transport::isPlaying() const
{
    return this->player->isPlaying();
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
// Sending messages at real-time
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
        
        for (int l = 0; l < this->tracksCache.size(); ++l)
        {
            const String &trackId = this->tracksCache.getUnchecked(l)->getTrackId();
            
            MidiMessageCollector *collector =
                &this->linksCache[trackId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! duplicateCollectors.contains(collector))
            {
                this->sendMidiMessage(trackId, notesOff);
                this->sendMidiMessage(trackId, controllersOff);
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
        
        for (int l = 0; l < this->tracksCache.size(); ++l)
        {
            const String &trackId = this->tracksCache.getUnchecked(l)->getTrackId();
            
            MidiMessageCollector *collector =
               &this->linksCache[trackId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! duplicateCollectors.contains(collector))
            {
                this->sendMidiMessage(trackId, notesOff);
                this->sendMidiMessage(trackId, controllersOff);
                this->sendMidiMessage(trackId, soundOff);
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

    for (int i = 0; i < this->tracksCache.size(); ++i)
    {
        this->updateLinkForTrack(this->tracksCache.getUnchecked(i));
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

    for (int i = 0; i < this->tracksCache.size(); ++i)
    {
        this->updateLinkForTrack(this->tracksCache.getUnchecked(i));
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

// FIXME: need to do something more reasonable than this workaround:
#define updateLengthAndTimeIfNeeded(track) \
    if (track->getTrackControllerNumber() == MidiTrack::tempoController) \
    { \
        this->seekToPosition(this->getSeekPosition()); \
    }

void Transport::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    // todo stop playback only if the event is in future
    // and getTrackControllerNumber == 0 (not an automation)
    this->stopPlayback();
    updateLengthAndTimeIfNeeded((&newEvent));
    this->sequencesAreOutdated = true;
}

void Transport::onAddMidiEvent(const MidiEvent &event)
{
    // todo stop playback only if the event is in future
    // and getTrackControllerNumber == 0 (not an automation)
    this->stopPlayback();
    updateLengthAndTimeIfNeeded((&event));
    this->sequencesAreOutdated = true;
}

void Transport::onRemoveMidiEvent(const MidiEvent &event) {}
void Transport::onPostRemoveMidiEvent(MidiSequence *const sequence)
{
    this->stopPlayback();
    updateLengthAndTimeIfNeeded(sequence->getTrack());
    this->sequencesAreOutdated = true;
}

void Transport::onAddClip(const Clip &clip)
{
    this->stopPlayback();
    updateLengthAndTimeIfNeeded((&clip));
    this->sequencesAreOutdated = true;
}

void Transport::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->stopPlayback();
    updateLengthAndTimeIfNeeded((&newClip));
    this->sequencesAreOutdated = true;
}

void Transport::onRemoveClip(const Clip &clip) {}
void Transport::onPostRemoveClip(Pattern *const pattern)
{
    this->stopPlayback();
    updateLengthAndTimeIfNeeded(pattern->getTrack());
    this->sequencesAreOutdated = true;
}

void Transport::onChangeTrackProperties(MidiTrack *const track)
{
    // Stop playback only when instrument changes:
    const auto &trackId = track->getTrackId();
    if (!linksCache.contains(trackId) ||
        this->linksCache[trackId]->getInstrumentID() != track->getTrackInstrumentId())
    {
        this->stopPlayback();
        this->sequencesAreOutdated = true;
        this->updateLinkForTrack(track);
    }
}

void Transport::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->stopPlayback();
    this->sequencesAreOutdated = true;
    for (const auto &track : tracks)
    {
        this->updateLinkForTrack(track);
    }
}

void Transport::onAddTrack(MidiTrack *const track)
{
    this->stopPlayback();
    
    this->sequencesAreOutdated = true;
    this->tracksCache.addIfNotAlreadyThere(track);
    this->updateLinkForTrack(track);
}

void Transport::onRemoveTrack(MidiTrack *const track)
{
    this->stopPlayback();
    
    this->sequencesAreOutdated = true;
    this->tracksCache.removeAllInstancesOf(track);
    this->removeLinkForTrack(track);
}

void Transport::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->stopPlayback();
    
    const double seekBeat = double(this->projectFirstBeat.get()) +
        double(this->projectLastBeat.get() - this->projectFirstBeat.get()) * this->seekPosition.get(); // may be 0

    //const double roundSeekBeat = round(seekBeat * 1000.0) / 1000.0;
    const double newBeatRange = (lastBeat - firstBeat); // may also be 0
    const double newSeekPosition = ((newBeatRange == 0.0) ? 0.0 : ((seekBeat - firstBeat) / newBeatRange));
    
    //
    //          |----------- 0.7 ----|
    // |--------+----------- 0.5 ----+---------------|
    //
    //  1. compute seek position as beat
    //  2. compute (seekBeat - newFirstBeat) / (newLastBeat - newFirstBeat)
    //
    
    this->trackStartMs = double(firstBeat) * MS_PER_BEAT;
    this->trackEndMs = double(lastBeat) * MS_PER_BEAT;
    this->setTotalTime(this->trackEndMs.get() - this->trackStartMs.get());
    
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
    
    const double TPQN = MS_PER_BEAT; // ticks-per-quarter-note
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
    
    return MidiMessage::tempoMetaEvent(int(MS_PER_BEAT) * 1000);
}


//===----------------------------------------------------------------------===//
// Sequences management
//===----------------------------------------------------------------------===//

void Transport::rebuildSequencesIfNeeded()
{
    if (this->sequencesAreOutdated)
    {
        this->sequences.clear();
        
        for (int i = 0; i < this->tracksCache.size(); ++i)
        {
            const auto *track = this->tracksCache.getUnchecked(i);
            MidiMessageSequence midiMessages;

            if (track->getPattern() != nullptr)
            {
                for (const auto *clip : track->getPattern()->getClips())
                {
                    const double clipOffset = round(double(clip->getBeat()) * MS_PER_BEAT);
                    midiMessages.addSequence(track->getSequence()->exportMidi(),
                        clipOffset - this->trackStartMs.get());
                }
            }
            else
            {
                midiMessages.addSequence(track->getSequence()->exportMidi(),
                    -this->trackStartMs.get());
            }

            if (midiMessages.getNumEvents() > 0)
            {
                Instrument *instrument = this->linksCache[track->getTrackId()];
                jassert(instrument != nullptr);
                auto wrapper = new SequenceWrapper();
                wrapper->track = track->getSequence();
                wrapper->midiMessages = midiMessages;
                wrapper->currentIndex = 0;
                wrapper->instrument = instrument;
                wrapper->listener = &instrument->getProcessorPlayer().getMidiMessageCollector();
                this->sequences.addWrapper(wrapper);
            }
        }
        
        this->sequencesAreOutdated = false;
    }
}

ProjectSequences Transport::getSequences()
{
    const SpinLock::ScopedLockType l(this->sequencesLock);
    return this->sequences;
}

void Transport::updateLinkForTrack(const MidiTrack *track)
{
    const Array<Instrument *> instruments = this->orchestra.getInstruments();
    
    // check by ids
    for (int i = 1; i < instruments.size(); ++i)
    {
        Instrument *instrument = instruments.getUnchecked(i);
        
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentID()))
        {
            // corresponding node already exists, lets add
            this->linksCache.set(track->getTrackId(), instrument);
            return;
        }
    }
    
    // check by hashes
    for (int i = 1; i < instruments.size(); ++i)
    {
        Instrument *instrument = instruments.getUnchecked(i);
        
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentHash()))
        {
            this->linksCache.set(track->getTrackId(), instrument);
            return;
        }
    }
    
    // set default instrument, if none found
    this->linksCache.set(track->getTrackId(),
        this->orchestra.getInstruments().getFirst());
}

void Transport::removeLinkForTrack(const MidiTrack *track)
{
    this->linksCache.remove(track->getTrackId());
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
    this->transportListeners.call(&TransportListener::onSeek,
        newPosition, currentTimeMs, totalTimeMs);
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

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Transport::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Audio::transport);
    tree.setProperty(Audio::transportSeekPosition, this->getSeekPosition(), nullptr);
    return tree;
}

void Transport::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;
    const auto root = tree.hasType(Audio::transport) ?
        tree : tree.getChildWithName(Audio::transport);
    const float seek = root.getProperty(Audio::transportSeekPosition, 0.f);
    this->seekToPosition(seek);
}

void Transport::reset() {}

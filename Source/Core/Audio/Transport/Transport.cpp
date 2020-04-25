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
#include "OrchestraPit.h"
#include "PlayerThread.h"
#include "RendererThread.h"
#include "MidiSequence.h"
#include "MidiEvent.h"
#include "MidiTrack.h"
#include "Clip.h"
#include "Pattern.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "HybridRoll.h"
#include "SerializationKeys.h"
#include "PlayerThreadPool.h"

#define TIME_NOW (Time::getMillisecondCounterHiRes() * 0.001)
#define SOUND_SLEEP_DELAY_MS (60000)

Transport::Transport(OrchestraPit &orchestraPit, SleepTimer &sleepTimer) :
    orchestra(orchestraPit),
    sleepTimer(sleepTimer)
{
    this->player = makeUnique<PlayerThreadPool>(*this);
    this->renderer = makeUnique<RendererThread>(*this);
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
    if (relTime.inSeconds() <= -1.0) // because '-0.0' is no cool
    {
        res = res + "-";
    }

    int n = std::abs(int(relTime.inMinutes())) /*% 60*/;
    res = res + String(n);
    
    n = std::abs(int(relTime.inSeconds())) % 60;
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

static const float kMaxMsPerQuarter = 250.f; // is 240 bpm (=250 ms-per-quarter) enough?

int Transport::getTempoByControllerValue(float controllerValue) noexcept
{
    const float safeCV = jlimit(0.00005f, 0.99999f, controllerValue);
    return int((1.f - AudioCore::fastLog2(safeCV)) * kMaxMsPerQuarter * 1000);
}

float Transport::getControllerValueByTempo(double secondsPerQuarterNote) noexcept
{
    const double cv = 1.0 - secondsPerQuarterNote / double(kMaxMsPerQuarter) * 1000.0;
    return jlimit(0.f, 1.f, powf(2.f, float(cv)));
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

float Transport::getSeekBeat() const noexcept
{
    return this->seekBeat.get();
}

void Transport::setSeekBeat(float beatPosition)
{
    this->seekBeat = beatPosition;
}

float Transport::getTotalTime() const noexcept
{
    return this->totalTime.get();
}

void Transport::setTotalTime(float val)
{
    this->totalTime = val;
}

//===----------------------------------------------------------------------===//
// Transport
//===----------------------------------------------------------------------===//

void Transport::seekToBeat(float beatPosition)
{
    double timeMs = 0.0;
    double realLengthMs = 0.0;
    double tempo = 0.0;
    this->findTimeAndTempoAt(this->projectFirstBeat.get(), timeMs, tempo);
    this->findTimeAndTempoAt(this->projectLastBeat.get(), realLengthMs, tempo);
    
    this->setSeekBeat(beatPosition);
    this->broadcastSeek(beatPosition - this->projectFirstBeat.get(), timeMs, realLengthMs);
}

void Transport::probeSoundAtBeat(float beatPosition, const MidiSequence *limitToLayer)
{
    this->sleepTimer.setAwake();
    this->recacheIfNeeded();
    
    const auto targetRelBeat = beatPosition - this->projectFirstBeat.get();
    const auto sequencesToProbe(this->playbackCache.getAllFor(limitToLayer));
    
    for (const auto &seq : sequencesToProbe)
    {
        for (int j = 0; j < seq->midiMessages.getNumEvents(); ++j)
        {
            auto *noteOnHolder = seq->midiMessages.getEventPointer(j);
            
            if (auto *noteOffHolder = noteOnHolder->noteOffObject)
            {
                const double noteOn(noteOnHolder->message.getTimeStamp());
                const double noteOff(noteOffHolder->message.getTimeStamp());
                
                if (noteOn <= targetRelBeat && noteOff > targetRelBeat)
                {
                    MidiMessage messageTimestampedAsNow(noteOnHolder->message);
                    messageTimestampedAsNow.setTimeStamp(TIME_NOW);
                    seq->listener->addMessageToQueue(messageTimestampedAsNow);
                }
            }
        }
    }

    this->sleepTimer.setCanSleepAfter(SOUND_SLEEP_DELAY_MS);
}

// Only used in a key signature dialog to test how scales sound
void Transport::probeSequence(const MidiMessageSequence &sequence)
{
    this->playbackCache.clear();
    this->playbackCacheIsOutdated = false; // temporary

    // using the last instrument (TODO something more clever in the future)
    auto *instrument = this->orchestra.getInstruments().getLast();
    auto cached = CachedMidiSequence::createFrom(instrument);
    cached->midiMessages = MidiMessageSequence(sequence);
    cached->midiMessages.addTimeToMessages(this->getSeekBeat());

    this->playbackCache.addWrapper(cached);

    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
    }

    this->player->startPlayback(true);
    this->playbackCacheIsOutdated = true; // will update on the next playback
}

//===----------------------------------------------------------------------===//
// Playback control
//===----------------------------------------------------------------------===//

void Transport::startPlayback()
{
    this->sleepTimer.setAwake();
    this->recacheIfNeeded();

    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
    }

    this->player->startPlayback(false);
    this->broadcastPlay();
}

void Transport::startPlaybackFragment(float startBeat, float endBeat, bool looped)
{
    this->sleepTimer.setAwake();
    this->recacheIfNeeded();
    
    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
    }

    this->player->startPlayback(startBeat, endBeat, looped, false);
    this->broadcastPlay();
}

void Transport::stopPlayback()
{
    if (this->player->isPlaying())
    {
        this->allNotesControllersAndSoundOff();
        this->seekToBeat(this->getSeekBeat());
        this->broadcastStop();
        this->sleepTimer.setCanSleepAfter(SOUND_SLEEP_DELAY_MS);
        this->player->stopPlayback(); // after broadcastStop so that MM can be locked
    }
}

void Transport::toggleStartStopPlayback()
{
    this->isPlaying() ? this->stopPlayback() : this->startPlayback();
}

bool Transport::isPlaying() const
{
    return this->player->isPlaying();
}

//===----------------------------------------------------------------------===//
// Recording MIDI
//===----------------------------------------------------------------------===//

void Transport::startRecording()
{
    if (!this->player->isPlaying())
    {
        this->sleepTimer.setAwake();
        this->recacheIfNeeded();
    }

    // canRecord == we have exactly 1 device available and enabled
    const bool canRecord = App::Workspace().getAudioCore().autodetectMidiDeviceSetup();
    if (canRecord)
    {
        this->midiRecordingMode = true;
        this->broadcastRecord();
        return;
    }

    const auto allDevices = MidiInput::getAvailableDevices();
    this->broadcastRecordFailed(allDevices);
}

bool Transport::isRecording() const
{
    return this->midiRecordingMode.get();
}

void Transport::stopRecording()
{
    if (this->isRecording())
    {
        this->midiRecordingMode = false;

        if (this->isPlaying())
        {
            this->stopPlayback(); // will broadcastStop
        }
        else
        {
            this->broadcastStop();
        }
    }
}

//===----------------------------------------------------------------------===//
// Playback loop, mostly used together with MIDI recording
//===----------------------------------------------------------------------===//

void Transport::toggleLoopPlayback(float startBeat, float endBeat)
{
    if (this->loopMode.get() &&
        this->loopStartBeat.get() == startBeat &&
        this->loopEndBeat.get() == endBeat)
    {
        this->disableLoopPlayback();
        return;
    }

    this->loopMode = true;
    this->loopStartBeat = startBeat;
    this->loopEndBeat = endBeat;

    this->broadcastLoopModeChanged(this->loopMode.get(),
        this->loopStartBeat.get(), this->loopEndBeat.get());
}

void Transport::disableLoopPlayback()
{
    if (this->loopMode.get())
    {
        this->loopMode = false;
        this->broadcastLoopModeChanged(false, 0.f, 0.f);
    }
}

//===----------------------------------------------------------------------===//
// Rendering
//===----------------------------------------------------------------------===//

void Transport::startRender(const String &fileName)
{
    if (this->renderer->isRecording())
    {
        return;
    }
    
    this->sleepTimer.setCanSleepAfter(0);

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
    
    this->sleepTimer.setAwake();
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

Transport::MidiMessageDelayedPreview::~MidiMessageDelayedPreview()
{
    this->cancelPendingPreview();
}

void Transport::MidiMessageDelayedPreview::cancelPendingPreview()
{
    this->stopTimer();
    this->messages.clearQuick();
    this->instruments.clearQuick();
}

void Transport::MidiMessageDelayedPreview::previewMessage(const MidiMessage &message,
    WeakReference<Instrument> instrument)
{
    this->messages.add(message);
    this->instruments.add(instrument);
    if (!this->isTimerRunning())
    {
        this->startTimer(50);
    }
}

void Transport::MidiMessageDelayedPreview::timerCallback()
{
    this->stopTimer();

#if HELIO_MOBILE
    // iSEM tends to hang >_< if too many messages are send simultaniously
    const auto time = TIME_NOW + float(rand() % 50) * 0.01;
#elif HELIO_DESKTOP
    const auto time = TIME_NOW;
#endif

    for (int i = 0; i < this->messages.size(); ++i)
    {
        if (Instrument *instrument = this->instruments.getUnchecked(i))
        {
            auto &message = this->messages.getReference(i);
            message.setTimeStamp(time);
            instrument->getProcessorPlayer().getMidiMessageCollector().addMessageToQueue(message);
        }
    }

    this->messages.clearQuick();
    this->instruments.clearQuick();
}

void Transport::previewMidiMessage(const String &trackId, const MidiMessage &message) const
{
    this->sleepTimer.setAwake();
    this->messagePreviewQueue.previewMessage(message, this->linksCache[trackId]);
    this->sleepTimer.setCanSleepAfter(SOUND_SLEEP_DELAY_MS);
}

static void stopSoundForInstrument(Instrument *instrument)
{
    auto &collector = instrument->getProcessorPlayer().getMidiMessageCollector();
    collector.addMessageToQueue(MidiMessage::allControllersOff(1).withTimeStamp(TIME_NOW));
    collector.addMessageToQueue(MidiMessage::allNotesOff(1).withTimeStamp(TIME_NOW));
    collector.addMessageToQueue(MidiMessage::allSoundOff(1).withTimeStamp(TIME_NOW));
}

void Transport::stopSound(const String &trackId) const
{
    this->sleepTimer.setAwake();
    this->messagePreviewQueue.cancelPendingPreview();

    if (Instrument *instrument = this->linksCache[trackId])
    {
        stopSoundForInstrument(instrument);
    }
    else
    {
        for (const auto &link : this->linksCache)
        {
            if (nullptr != link.second)
            {
                stopSoundForInstrument(link.second);
            }
        }
    }

    this->sleepTimer.setCanSleepAfter(SOUND_SLEEP_DELAY_MS);
}

void Transport::allNotesControllersAndSoundOff() const
{
    this->sleepTimer.setAwake();
    this->messagePreviewQueue.cancelPendingPreview();

    static const int c = 1;
    //for (int c = 1; c <= 16; ++c)
    {
        const MidiMessage notesOff(MidiMessage::allNotesOff(c).withTimeStamp(TIME_NOW));
        const MidiMessage soundOff(MidiMessage::allSoundOff(c).withTimeStamp(TIME_NOW));
        const MidiMessage controllersOff(MidiMessage::allControllersOff(c).withTimeStamp(TIME_NOW));
        
        Array<const MidiMessageCollector *> duplicateCollectors;
        
        for (int l = 0; l < this->tracksCache.size(); ++l)
        {
            const auto &trackId = this->tracksCache.getUnchecked(l)->getTrackId();
            auto *collector = &this->linksCache[trackId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! duplicateCollectors.contains(collector))
            {
                collector->addMessageToQueue(notesOff);
                collector->addMessageToQueue(controllersOff);
                collector->addMessageToQueue(soundOff);
                duplicateCollectors.add(collector);
            }
        }
    }

    this->sleepTimer.setCanSleepAfter(SOUND_SLEEP_DELAY_MS);
}


//===----------------------------------------------------------------------===//
// OrchestraListener
//===----------------------------------------------------------------------===//

void Transport::instrumentAdded(Instrument *instrument)
{
    this->stopPlayback();
    
    // invalidate cache as is uses pointers to the players too
    this->playbackCacheIsOutdated = true;

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
    this->playbackCacheIsOutdated = true;

    for (int i = 0; i < this->tracksCache.size(); ++i)
    {
        this->updateLinkForTrack(this->tracksCache.getUnchecked(i));
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

// FIXME: need to do something more reasonable than this workaround:
#define updateLengthAndTimeIfNeeded(event) \
    if (event->getTrackControllerNumber() == MidiTrack::tempoController) \
    { \
        this->seekToBeat(this->getSeekBeat()); \
    }

void Transport::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    // relying on isRecording() sucks,
    // but I don't see any easy workaround at the moment:
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    updateLengthAndTimeIfNeeded((&newEvent));
    this->playbackCacheIsOutdated = true;
}

void Transport::onAddMidiEvent(const MidiEvent &event)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    updateLengthAndTimeIfNeeded((&event));
    this->playbackCacheIsOutdated = true;
}

void Transport::onRemoveMidiEvent(const MidiEvent &event) {}
void Transport::onPostRemoveMidiEvent(MidiSequence *const sequence)
{
    this->stopPlayback();
    updateLengthAndTimeIfNeeded(sequence->getTrack());
    this->playbackCacheIsOutdated = true;
}

void Transport::onAddClip(const Clip &clip)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    updateLengthAndTimeIfNeeded((&clip));
    this->playbackCacheIsOutdated = true;
}

void Transport::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->stopPlayback();
    updateLengthAndTimeIfNeeded((&newClip));
    this->playbackCacheIsOutdated = true;
}

void Transport::onRemoveClip(const Clip &clip) {}
void Transport::onPostRemoveClip(Pattern *const pattern)
{
    this->stopPlayback();
    updateLengthAndTimeIfNeeded(pattern->getTrack());
    this->playbackCacheIsOutdated = true;
}

void Transport::onChangeTrackProperties(MidiTrack *const track)
{
    // Stop playback only when instrument changes:
    const auto &trackId = track->getTrackId();
    if (!linksCache.contains(trackId) ||
        this->linksCache[trackId]->getInstrumentId() != track->getTrackInstrumentId())
    {
        if (!this->isRecording())
        {
            this->stopPlayback();
        }

        this->playbackCacheIsOutdated = true;
        this->updateLinkForTrack(track);
    }
}

void Transport::onReloadProjectContent(const Array<MidiTrack *> &tracks)
{
    this->playbackCacheIsOutdated = true;

    this->tracksCache.clearQuick();
    this->linksCache.clear();

    this->tracksCache.addArray(tracks);
    for (const auto &track : tracks)
    {
        this->updateLinkForTrack(track);
    }

    this->stopPlayback();
}

void Transport::onAddTrack(MidiTrack *const track)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }
    
    this->playbackCacheIsOutdated = true;
    this->tracksCache.addIfNotAlreadyThere(track);
    this->updateLinkForTrack(track);
}

void Transport::onRemoveTrack(MidiTrack *const track)
{
    this->stopPlayback();
    
    this->playbackCacheIsOutdated = true;
    this->tracksCache.removeAllInstancesOf(track);
    this->removeLinkForTrack(track);
}

void Transport::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    const auto newBeatRange = (lastBeat - firstBeat); // may also be 0
    const auto newSeekPosition = ((newBeatRange == 0.f) ? 0.f : ((this->seekBeat.get() - firstBeat) / newBeatRange));
    
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    this->setTotalTime(lastBeat - firstBeat);
    
    // real track total time changed
    double tempo = 0.0;
    double realLengthMs = 0.0;
    this->findTimeAndTempoAt(lastBeat, realLengthMs, tempo);
    this->broadcastTotalTimeChanged(realLengthMs);
    
    // seek has also changed
    if (!this->isRecording())
    {
        this->seekToBeat(newSeekPosition);
    }
}

//===----------------------------------------------------------------------===//
// Real track length calc
//===----------------------------------------------------------------------===//

void Transport::findTimeAndTempoAt(float beat, double &outTimeMs, double &outTempo) const
{
    this->recacheIfNeeded();
    this->playbackCache.seekToZeroIndexes();
    
    const auto targetRelativeBeat = beat - this->projectFirstBeat.get();
    
    outTimeMs = 0.0;
    outTempo = DEFAULT_MS_PER_QN;
    
    double prevTimestamp = 0.0;
    double nextEventTimeDelta = 0.0;
    bool foundFirstTempoEvent = false;
    
    CachedMidiMessage cached;
    
    while (this->playbackCache.getNextMessage(cached))
    {
        const auto nextPosition = cached.message.getTimeStamp();
        
        // first tempo event sets the tempo all the way before it
        if (nextPosition > targetRelativeBeat && foundFirstTempoEvent)
        {
            break;
        }
        
        nextEventTimeDelta = outTempo * (cached.message.getTimeStamp() - prevTimestamp);
        outTimeMs += nextEventTimeDelta;
        prevTimestamp = cached.message.getTimeStamp();
        
        if (cached.message.isTempoMetaEvent())
        {
            outTempo = cached.message.getTempoSecondsPerQuarterNote() * 1000.f;
            foundFirstTempoEvent = true;
        }
    }
    
    nextEventTimeDelta = outTempo * (targetRelativeBeat - prevTimestamp);
    outTimeMs += nextEventTimeDelta;
}

MidiMessage Transport::findFirstTempoEvent()
{
    this->recacheIfNeeded();
    this->playbackCache.seekToZeroIndexes();
    
    CachedMidiMessage wrapper;
    
    while (this->playbackCache.getNextMessage(wrapper))
    {
        if (wrapper.message.isTempoMetaEvent())
        {
            return wrapper.message;
        }
    }
    
    // return default 120 bpm (== 500 ms per quarter note)
    return MidiMessage::tempoMetaEvent(DEFAULT_MS_PER_QN * 1000);
}

//===----------------------------------------------------------------------===//
// Playback cache management
//===----------------------------------------------------------------------===//

void Transport::recacheIfNeeded() const
{
    if (this->playbackCacheIsOutdated.get())
    {
        //DBG("Transport::recache");
        this->playbackCache.clear();
        static Clip noTransform;
        const double offset = -this->projectFirstBeat.get();

        // Find solo clips, if any
        bool hasSoloClips = false;
        for (const auto *track : this->tracksCache)
        {
            if (track->getPattern() != nullptr &&
                track->getPattern()->hasSoloClips())
            {
                hasSoloClips = true;
                break;
            }
        }

        for (const auto *track : this->tracksCache)
        {
            const auto instrument = this->linksCache[track->getTrackId()];
            auto cached = CachedMidiSequence::createFrom(instrument, track->getSequence());

            if (track->getPattern() != nullptr)
            {
                for (const auto *clip : track->getPattern()->getClips())
                {
                    cached->track->exportMidi(cached->midiMessages, *clip, hasSoloClips, offset, 1.0);
                }
            }
            else
            {
                cached->track->exportMidi(cached->midiMessages, noTransform, hasSoloClips, offset, 1.0);
            }

            this->playbackCache.addWrapper(cached);
        }
        
        this->playbackCacheIsOutdated = false;
    }
}

TransportPlaybackCache Transport::getPlaybackCache()
{
    return this->playbackCache;
}

void Transport::updateLinkForTrack(const MidiTrack *track)
{
    const Array<Instrument *> instruments = this->orchestra.getInstruments();
    
    // check by ids
    for (int i = 1; i < instruments.size(); ++i)
    {
        Instrument *instrument = instruments.getUnchecked(i);
        
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentId()))
        {
            // corresponding node already exists, lets add
            this->linksCache[track->getTrackId()] = instrument;
            return;
        }
    }
    
    // check by hashes
    for (int i = 1; i < instruments.size(); ++i)
    {
        Instrument *instrument = instruments.getUnchecked(i);
        
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentHash()))
        {
            this->linksCache[track->getTrackId()] = instrument;
            return;
        }
    }
    
    // set default instrument, if none found
    this->linksCache[track->getTrackId()] =  this->orchestra.getInstruments().getFirst();
}

void Transport::removeLinkForTrack(const MidiTrack *track)
{
    this->linksCache.erase(track->getTrackId());
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

void Transport::broadcastSeek(float relativeBeat,
    double currentTimeMs, double totalTimeMs)
{
    // relativeBeat is relative to the project's first beat
    this->transportListeners.call(&TransportListener::onSeek,
        relativeBeat + this->projectFirstBeat.get(), currentTimeMs, totalTimeMs);
}

void Transport::broadcastTempoChanged(double newTempo)
{
    this->transportListeners.call(&TransportListener::onTempoChanged, newTempo);
}

void Transport::broadcastTotalTimeChanged(double timeMs)
{
    this->transportListeners.call(&TransportListener::onTotalTimeChanged, timeMs);
}

void Transport::broadcastLoopModeChanged(bool hasLoop, float startBeat, float endBeat)
{
    this->transportListeners.call(&TransportListener::onLoopModeChanged, hasLoop, startBeat, endBeat);
}

void Transport::broadcastPlay()
{
    MessageManagerLock mml(Thread::getCurrentThread());
    jassert(mml.lockWasGained());

    if (mml.lockWasGained())
    {
        this->transportListeners.call(&TransportListener::onPlay);
    }
}

void Transport::broadcastRecord()
{
    MessageManagerLock mml(Thread::getCurrentThread());
    jassert(mml.lockWasGained());

    if (mml.lockWasGained())
    {
        this->transportListeners.call(&TransportListener::onRecord);
    }
}

void Transport::broadcastRecordFailed(const Array<MidiDeviceInfo> &devices)
{
    MessageManagerLock mml(Thread::getCurrentThread());
    jassert(mml.lockWasGained());

    if (mml.lockWasGained())
    {
        this->transportListeners.call(&TransportListener::onRecordFailed, devices);
    }
}

void Transport::broadcastStop()
{
    MessageManagerLock mml(Thread::getCurrentThread());
    jassert(mml.lockWasGained());

    if (mml.lockWasGained())
    {
        this->transportListeners.call(&TransportListener::onStop);
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Transport::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Audio::transport);
    tree.setProperty(Audio::transportSeekBeat, this->getSeekBeat());
    return tree;
}

void Transport::deserialize(const SerializedData &data)
{
    this->reset();

    using namespace Serialization;
    const auto root = data.hasType(Audio::transport) ?
        data : data.getChildWithName(Audio::transport);

    const float seek = root.getProperty(Audio::transportSeekBeat, 0.f);
    this->seekToBeat(seek);
}

void Transport::reset() {}

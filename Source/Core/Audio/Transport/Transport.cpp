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
#include "RendererThread.h"
#include "PlayerThread.h"
#include "PlayerThreadPool.h"
#include "MidiSequence.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "RollBase.h"
#include "KeyboardMapping.h"
#include "ProjectMetadata.h"
#include "BuiltInSynthAudioPlugin.h"

#define TIME_NOW (Time::getMillisecondCounterHiRes() * 0.001)

Transport::Transport(OrchestraPit &orchestraPit, SleepTimer &sleepTimer) :
    orchestra(orchestraPit),
    sleepTimer(sleepTimer)
{
    this->player = make<PlayerThreadPool>(*this);
    this->renderer = make<RendererThread>(*this);
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

int Transport::getTempoByControllerValue(float controllerValue) noexcept
{
    const double safeCV = jlimit(0.00005, 0.99999, double(controllerValue));
    return int((1.0 - log2(safeCV)) * Globals::maxMsPerBeat * 1000);
}

float Transport::getControllerValueByTempo(double secondsPerQuarterNote) noexcept
{
    const double cv = 1.0 - secondsPerQuarterNote / Globals::maxMsPerBeat * 1000.0;
    const double value = pow(2.0, cv);
    return jlimit(0.f, 1.f, float(value));
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
    const auto currentTimeMs = this->findTimeAt(this->projectFirstBeat.get());
    const auto totalTimeMs = this->findTimeAt(this->projectLastBeat.get());
    this->setSeekBeat(beatPosition);
    this->broadcastSeek(beatPosition, currentTimeMs, totalTimeMs);
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

    this->sleepTimer.setCanSleepAfter(Transport::soundSleepDelayMs);
}

//===----------------------------------------------------------------------===//
// Playback control
//===----------------------------------------------------------------------===//

void Transport::startPlayback()
{
    this->startPlayback(this->getSeekBeat());
}

void Transport::startPlayback(float start)
{
    this->sleepTimer.setAwake();
    this->recacheIfNeeded();

    this->stopPlayback();

    if (this->loopMode.get())
    {
        const auto loopStart = this->loopStartBeat.get();
        const auto end = this->loopEndBeat.get();

        this->player->startPlayback((start >= end) ? loopStart : start,
            loopStart, end, true);
    }
    else
    {
        this->player->startPlayback(start,
            this->getSeekBeat(), this->getProjectLastBeat(), false);
    }

    this->broadcastPlay();
}

void Transport::startPlaybackFragment(float startBeat, float endBeat, bool looped)
{
    this->sleepTimer.setAwake();
    this->recacheIfNeeded();
    
    this->stopPlayback();

    this->player->startPlayback(startBeat, startBeat, endBeat, looped);
    this->broadcastPlay();
}

void Transport::stopPlayback()
{
    if (this->player->isPlaying())
    {
        this->broadcastStop();
        this->player->stopPlayback(); // after broadcastStop so that MM can be locked
        this->allNotesControllersAndSoundOff();
        this->seekToBeat(this->getSeekBeat());
        this->sleepTimer.setCanSleepAfter(Transport::soundSleepDelayMs);
    }
}

void Transport::toggleStartStopPlayback()
{
    this->isPlaying() ? this->stopPlaybackAndRecording() : this->startPlayback();
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
    if (!this->isPlaying())
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

void Transport::stopPlaybackAndRecording()
{
    this->stopRecording();
    this->stopPlayback();
}

//===----------------------------------------------------------------------===//
// Playback loop, mostly used together with MIDI recording
//===----------------------------------------------------------------------===//

void Transport::toggleLoopPlayback(float startBeat, float endBeat)
{
    if (this->isPlaying())
    {
        this->stopPlaybackAndRecording();
    }

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

void Transport::startRender(const URL &renderTarget, RenderFormat format)
{
    if (this->renderer->isRendering())
    {
        return;
    }
    
    this->sleepTimer.setCanSleepAfter(0);
    this->renderer->startRendering(renderTarget, format,
        this->fillPlaybackContextAt(this->getProjectFirstBeat()));
}

void Transport::stopRender()
{
    if (! this->renderer->isRendering())
    {
        return;
    }
    
    this->renderer->stop();
    
    this->sleepTimer.setAwake();
}

bool Transport::isRendering() const
{
    return this->renderer->isRendering();
}

float Transport::getRenderingPercentsComplete() const
{
    return this->renderer->getPercentsComplete();
}

//===----------------------------------------------------------------------===//
// Sending messages at real-time
//===----------------------------------------------------------------------===//

/*
    The purpose of this class is to help previewing messages interactively in the sequencer.
    It will simply send note-on events after a delay, and keep track of note-off events which
    needs to be sent after a while; all those previews are cancelled by transport on stopSound;
    The delay before the note-on is needed because some plugins (e.g. Kontakt in my case)
    are sometimes processing play/stop messages out of order for whatever reason, which is weird -
    note the word `queue` in addMessageToQueue() method name - but it still happens in some cases,
    for example, when the user drags some notes around quickly.
*/

Transport::NotePreviewTimer::~NotePreviewTimer()
{
    this->cancelAllPendingPreviews();
}

void Transport::NotePreviewTimer::cancelAllPendingPreviews()
{
    if (this->isTimerRunning())
    {
        this->stopTimer();
        for (int key = 0; key < NotePreviewTimer::numPreviewedKeys; ++key)
        {
            this->previews[key].instrument = nullptr;
            this->previews[key].noteOnTimeoutMs = 0;
            this->previews[key].noteOffTimeoutMs = 0;
            this->previews[key].volume = 0;
        }
    }
}

void Transport::NotePreviewTimer::previewNote(WeakReference<Instrument> instrument,
    int key, float volume, int16 noteOffTimeoutMs)
{
    jassert(key >= 0 && key < NotePreviewTimer::numPreviewedKeys);

    auto &preview = this->previews[key];

    if (preview.noteOffTimeoutMs > 0 &&
        preview.instrument != instrument &&
        preview.instrument != nullptr)
    {
        //DBG("! noteOff " + String(key));
        const auto mapped = preview.instrument->getKeyboardMapping()->map(key);
        MidiMessage message(MidiMessage::noteOff(mapped.channel, mapped.key));
        message.setTimeStamp(TIME_NOW);
        preview.instrument->getProcessorPlayer()
            .getMidiMessageCollector().addMessageToQueue(message);
    }

    preview.volume = volume;
    preview.noteOnTimeoutMs = NotePreviewTimer::timerTickMs;
    preview.noteOffTimeoutMs = noteOffTimeoutMs;
    preview.instrument = instrument;

    if (!this->isTimerRunning())
    {
        this->startTimer(NotePreviewTimer::timerTickMs);
    }
}

void Transport::NotePreviewTimer::timerCallback()
{
#if PLATFORM_MOBILE
    // iSEM tends to hang >_< if too many messages are send simultaniously
    const auto time = TIME_NOW + float(rand() % 50) * 0.01;
#elif PLATFORM_DESKTOP
    const auto time = TIME_NOW;
#endif

    bool canStop = true;
    for (int key = 0; key < NotePreviewTimer::numPreviewedKeys; ++key)
    {
        auto &preview = this->previews[key];
        if (preview.noteOnTimeoutMs > 0)
        {
            canStop = false;
            preview.noteOnTimeoutMs -= NotePreviewTimer::timerTickMs;

            if (preview.noteOnTimeoutMs <= 0 && preview.instrument != nullptr)
            {
                //DBG("noteOn " + String(key));
                const auto mapped = preview.instrument->getKeyboardMapping()->map(key);
                MidiMessage message(MidiMessage::noteOn(mapped.channel, mapped.key, preview.volume));
                message.setTimeStamp(time);
                preview.instrument->getProcessorPlayer()
                    .getMidiMessageCollector().addMessageToQueue(message);
            }
        }
        else if (preview.noteOffTimeoutMs > 0)
        {
            canStop = false;
            preview.noteOffTimeoutMs -= NotePreviewTimer::timerTickMs;

            if (preview.noteOffTimeoutMs <= 0 && preview.instrument != nullptr)
            {
                //DBG("noteOff " + String(key));
                const auto mapped = preview.instrument->getKeyboardMapping()->map(key);
                MidiMessage message(MidiMessage::noteOff(mapped.channel, mapped.key));
                message.setTimeStamp(time);
                preview.instrument->getProcessorPlayer()
                    .getMidiMessageCollector().addMessageToQueue(message);
            }
        }
    }

    if (canStop)
    {
        this->stopTimer();
    }
}

void Transport::previewKey(const String &trackId, int key,
    float volume, float lengthInBeats) const
{
    this->sleepTimer.setAwake();

    const auto foundLink = this->linksCache.find(trackId);
    const bool useDefaultInstrument =
        trackId.isEmpty() || foundLink == this->linksCache.end();

    auto *instrument = useDefaultInstrument ?
        this->orchestra.getInstruments().getLast() :
        foundLink.value().get();

    jassert(instrument != nullptr);

    // to calculate the note-off timeout interval, lets just use
    // the default 120 BPM for simplicity - instead of finding the tempo
    // at the note position, which I think is a bit of an overkill:
    const auto noteOffTimeoutMs = int16(Globals::Defaults::msPerBeat * lengthInBeats);
    this->notePreviewTimer.previewNote(instrument, key, volume, noteOffTimeoutMs);

    this->sleepTimer.setCanSleepAfter(Transport::soundSleepDelayMs + noteOffTimeoutMs * 2);
}

static void stopSoundForInstrument(Instrument *instrument)
{
    auto &collector = instrument->getProcessorPlayer().getMidiMessageCollector();

    for (int i = 1; i < Globals::numChannels; ++i)
    {
        collector.addMessageToQueue(MidiMessage::allControllersOff(i).withTimeStamp(TIME_NOW));
        collector.addMessageToQueue(MidiMessage::allNotesOff(i).withTimeStamp(TIME_NOW));
        collector.addMessageToQueue(MidiMessage::allSoundOff(i).withTimeStamp(TIME_NOW));
    }
}

void Transport::stopSound(const String &trackId) const
{
    this->sleepTimer.setAwake();
    this->notePreviewTimer.cancelAllPendingPreviews();

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

    this->sleepTimer.setCanSleepAfter(Transport::soundSleepDelayMs);
}

void Transport::allNotesControllersAndSoundOff() const
{
    this->sleepTimer.setAwake();
    this->notePreviewTimer.cancelAllPendingPreviews();

    for (int i = 1; i < Globals::numChannels; ++i)
    {
        const MidiMessage notesOff(MidiMessage::allNotesOff(i).withTimeStamp(TIME_NOW));
        const MidiMessage soundOff(MidiMessage::allSoundOff(i).withTimeStamp(TIME_NOW));
        const MidiMessage controllersOff(MidiMessage::allControllersOff(i).withTimeStamp(TIME_NOW));
        
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

    this->sleepTimer.setCanSleepAfter(Transport::soundSleepDelayMs);
}


//===----------------------------------------------------------------------===//
// OrchestraListener
//===----------------------------------------------------------------------===//

void Transport::instrumentAdded(Instrument *instrument)
{
    this->stopPlaybackAndRecording();

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
    this->stopPlaybackAndRecording();
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
    this->stopPlaybackAndRecording();
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
    this->stopPlaybackAndRecording();
    updateLengthAndTimeIfNeeded((&newClip));
    this->playbackCacheIsOutdated = true;
}

void Transport::onRemoveClip(const Clip &clip) {}
void Transport::onPostRemoveClip(Pattern *const pattern)
{
    this->stopPlaybackAndRecording();
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

void Transport::updateTemperamentInfoForBuiltInSynth(int periodSize) const
{
    const auto *defaultInstrument = this->orchestra.getDefaultInstrument();
    if (auto mainNode = defaultInstrument->findMainPluginNode())
    {
        if (auto *synth = dynamic_cast<BuiltInSynthAudioPlugin *>(mainNode->getProcessor()))
        {
            synth->setPeriodSize(periodSize);
        }
    }
}

void Transport::onDeactivateProjectSubtree(const ProjectMetadata *meta)
{
    this->stopPlaybackAndRecording();
}

void Transport::onActivateProjectSubtree(const ProjectMetadata *meta)
{
    this->updateTemperamentInfoForBuiltInSynth(meta->getPeriodSize());

    // let's reset midi caches, just in case some instrument's keyboard mapping
    // has changed in the meanwhile (no idea how to observe kbm changes in transport)
    this->playbackCacheIsOutdated = true;
}

void Transport::onChangeProjectInfo(const ProjectMetadata *meta)
{
    this->updateTemperamentInfoForBuiltInSynth(meta->getPeriodSize());
}

void Transport::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->playbackCacheIsOutdated = true;

    this->tracksCache.clearQuick();
    this->linksCache.clear();

    this->tracksCache.addArray(tracks);
    for (const auto &track : tracks)
    {
        this->updateLinkForTrack(track);
    }

    this->stopPlaybackAndRecording();

    this->updateTemperamentInfoForBuiltInSynth(meta->getPeriodSize());
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
    this->stopPlaybackAndRecording();

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

    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    this->setTotalTime(lastBeat - firstBeat);
    
    // real track total time changed
    const auto realLengthMs = this->findTimeAt(lastBeat);
    this->broadcastTotalTimeChanged(realLengthMs);
}

//===----------------------------------------------------------------------===//
// Track time calculations
//===----------------------------------------------------------------------===//

double Transport::findTimeAt(float beat) const
{
    double resultTimeMs = 0.0;

    this->recacheIfNeeded();
    this->playbackCache.seekToZeroIndexes();

    const auto targetRelativeBeat = beat - this->projectFirstBeat.get();

    double tempo = Globals::Defaults::msPerBeat;
    double prevTimestamp = 0.0;

    CachedMidiMessage cached;
    while (this->playbackCache.getNextMessage(cached))
    {
        const auto nextTimestamp = cached.message.getTimeStamp();

        if (nextTimestamp > targetRelativeBeat)
        {
            break;
        }

        const auto nextEventTimeDelta = tempo * (nextTimestamp - prevTimestamp);
        resultTimeMs += nextEventTimeDelta;
        prevTimestamp = nextTimestamp;

        if (cached.message.isTempoMetaEvent())
        {
            tempo = cached.message.getTempoSecondsPerQuarterNote() * 1000.f;
        }
    }

    // add remainder, the time from the last event to the given beat:
    resultTimeMs += (tempo * (targetRelativeBeat - prevTimestamp));

    return resultTimeMs;
}

Transport::PlaybackContext::Ptr Transport::fillPlaybackContextAt(float beat) const
{
    this->recacheIfNeeded();
    this->playbackCache.seekToZeroIndexes();

    double tempo = Globals::Defaults::msPerBeat;

    Transport::PlaybackContext::Ptr context(new Transport::PlaybackContext());
    context->projectFirstBeat = this->projectFirstBeat.get();
    context->projectLastBeat = this->projectLastBeat.get();

    context->startBeat = beat;

    context->totalTimeMs = 0.0;
    context->startBeatTimeMs = 0.0;
    context->startBeatTempo = tempo;

    context->sampleRate = this->playbackCache.getSampleRate();
    context->numOutputChannels = this->playbackCache.getNumOutputChannels();
    
    const auto relativeTargetBeat = context->startBeat - context->projectFirstBeat;
    const auto relativeEndBeat = context->projectLastBeat - context->projectFirstBeat;

    double prevTimestamp = 0.0;
    bool startBeatPassed = false;

    CachedMidiMessage cached;
    while (this->playbackCache.getNextMessage(cached))
    {
        const auto nextTimestamp = cached.message.getTimeStamp();
        const auto nextEventTimeDelta = tempo * (nextTimestamp - prevTimestamp);

        if (nextTimestamp > relativeTargetBeat)
        {
            // the time from the last event to the given beat:
            context->startBeatTimeMs += (tempo * (relativeTargetBeat - prevTimestamp));
            startBeatPassed = true;
        }

        if (!startBeatPassed)
        {
            context->startBeatTimeMs += nextEventTimeDelta;
        }

        context->totalTimeMs += nextEventTimeDelta;
        prevTimestamp = nextTimestamp;

        if (cached.message.isTempoMetaEvent())
        {
            tempo = cached.message.getTempoSecondsPerQuarterNote() * 1000.f;
        }
        else if (!startBeatPassed &&
            cached.message.isController() &&
            cached.message.getControllerNumber() <= PlaybackContext::numCCs)
        {
            context->ccStates[cached.message.getControllerNumber()] =
                cached.message.getControllerValue();
        }

        if (!startBeatPassed)
        {
            context->startBeatTempo = tempo;
        }
    }

    // the remainder
    context->totalTimeMs += (tempo * (relativeEndBeat - prevTimestamp));
    //jassert(context->totalTimeMs == this->findTimeAt(this->projectLastBeat.get()));

    return context;
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
            const auto &keyMap = *instrument->getKeyboardMapping();

            auto cached = CachedMidiSequence::createFrom(instrument, track->getSequence());

            if (track->getPattern() != nullptr)
            {
                for (const auto *clip : track->getPattern()->getClips())
                {
                    cached->track->exportMidi(cached->midiMessages, *clip,
                        keyMap, hasSoloClips, offset, 1.0);
                }
            }
            else
            {
                cached->track->exportMidi(cached->midiMessages, noTransform,
                    keyMap, hasSoloClips, offset, 1.0);
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
    const auto instruments = this->orchestra.getInstruments();
    
    // check by ids
    for (int i = 0; i < instruments.size(); ++i)
    {
        auto *instrument = instruments.getUnchecked(i);
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentId()))
        {
            // corresponding node already exists, lets add
            this->linksCache[track->getTrackId()] = instrument;
            return;
        }
    }
    
    // check by hashes
    for (int i = 0; i < instruments.size(); ++i)
    {
        auto *instrument = instruments.getUnchecked(i);
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentHash()))
        {
            this->linksCache[track->getTrackId()] = instrument;
            return;
        }
    }
    
    // set default instrument, if none found
    this->linksCache[track->getTrackId()] = this->orchestra.getDefaultInstrument();
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

void Transport::broadcastSeek(float beat, double currentTimeMs, double totalTimeMs)
{
    // relativeBeat is relative to the project's first beat
    this->transportListeners.call(&TransportListener::onSeek,
        beat, currentTimeMs, totalTimeMs);
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
    jassert(!this->isRecording());

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

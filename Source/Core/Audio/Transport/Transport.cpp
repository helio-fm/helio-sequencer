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
#include "Transport.h"
#include "OrchestraPit.h"
#include "RendererThread.h"
#include "PlayerThread.h"
#include "PlayerThreadPool.h"
#include "MidiSequence.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "Workspace.h"
#include "RollBase.h"
#include "KeyboardMapping.h"
#include "ProjectMetadata.h"
#include "ProjectTimeline.h"
#include "DefaultSynthAudioPlugin.h"

#define TIME_NOW (Time::getMillisecondCounterHiRes() * 0.001)

Transport::Transport(ProjectNode &project, OrchestraPit &orchestraPit) :
    project(project),
    orchestra(orchestraPit)
{
    this->player = make<PlayerThreadPool>(*this);
    this->renderer = make<RendererThread>(*this);

    this->project.addListener(this);
    this->project.getTimeline()->getTimeSignaturesAggregator()->addListener(this);
    this->orchestra.addOrchestraListener(this);

    App::Config().getUiFlags()->addListener(this);
}

Transport::~Transport()
{
    App::Config().getUiFlags()->removeListener(this);

    this->orchestra.removeOrchestraListener(this);
    this->project.getTimeline()->getTimeSignaturesAggregator()->removeListener(this);
    this->project.removeListener(this);

    this->renderer = nullptr;
    this->player = nullptr;

    this->transportListeners.clear();
}

String Transport::getTimeString(double timeMs, bool includeMilliseconds)
{
    return Transport::getTimeString(RelativeTime(timeMs / 1000.0), includeMilliseconds);
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
    const float safeCV = jlimit(0.00005f, 0.99999f, controllerValue);
    return int((1.f - log2f(safeCV)) * Globals::maxMsPerBeat * 1000.f);
}

float Transport::getControllerValueByTempo(double secondsPerQuarterNote) noexcept
{
    const float cv = 1.f - float(secondsPerQuarterNote / Globals::maxMsPerBeat * 1000.0);
    const float value = powf(2.f, cv);
    return jlimit(0.f, 1.f, value);
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

//===----------------------------------------------------------------------===//
// Transport
//===----------------------------------------------------------------------===//

void Transport::seekToBeat(float beatPosition)
{
    this->setSeekBeat(beatPosition);
    this->broadcastSeek(beatPosition);
}

void Transport::probeSoundAtBeat(float targetBeat, const MidiSequence *limitToSequence)
{
    this->rebuildPlaybackCacheIfNeeded();
    
    const auto sequencesToProbe = this->playbackCache.getAllFor(limitToSequence);
    
    for (const auto &seq : sequencesToProbe)
    {
        for (int j = 0; j < seq->midiMessages.getNumEvents(); ++j)
        {
            auto *noteOnHolder = seq->midiMessages.getEventPointer(j);
            
            if (auto *noteOffHolder = noteOnHolder->noteOffObject)
            {
                const auto noteOnBeat = noteOnHolder->message.getTimeStamp();
                const auto noteOffBeat = noteOffHolder->message.getTimeStamp();
                
                if (noteOnBeat <= targetBeat && noteOffBeat > targetBeat)
                {
                    MidiMessage messageTimestampedAsNow(noteOnHolder->message);
                    messageTimestampedAsNow.setTimeStamp(TIME_NOW);
                    seq->listener->addMessageToQueue(messageTimestampedAsNow);
                }
            }
        }
    }
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
    this->rebuildPlaybackCacheIfNeeded();

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
    this->rebuildPlaybackCacheIfNeeded();
    
    this->stopPlayback();

    this->player->startPlayback(startBeat, startBeat, endBeat, looped);
    this->broadcastPlay();
}

void Transport::setPlaybackSpeedMultiplier(float multiplier)
{
    jassert(this->isPlaying());
    jassert(multiplier > 0.5f && multiplier < 5.f);
    this->player->setPlaybackSpeedMultiplier(multiplier);
}

void Transport::stopPlayback()
{
    if (this->player->isPlaying())
    {
        this->broadcastStop();
        this->player->stopPlayback(); // after broadcastStop so that MM can be locked
        this->allNotesControllersAndSoundOff();
        this->seekToBeat(this->getSeekBeat());
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

bool Transport::isPlayingAndRecording() const
{
    return this->isPlaying() && this->isRecording();
}

//===----------------------------------------------------------------------===//
// Recording MIDI
//===----------------------------------------------------------------------===//

void Transport::startRecording()
{
    if (!this->isPlaying())
    {
        this->rebuildPlaybackCacheIfNeeded();
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

void Transport::togglePlaybackLoop(float startBeat, float endBeat)
{
    if (this->loopMode.get())
    {
        this->disablePlaybackLoop();
        return;
    }

    this->setPlaybackLoop(startBeat, endBeat);
}

void Transport::setPlaybackLoop(float startBeat, float endBeat)
{
    if (this->isPlaying())
    {
        this->stopPlaybackAndRecording();
    }

    this->loopMode = true;
    this->loopStartBeat = startBeat;
    // length is at least one beat:
    this->loopEndBeat = jmax(endBeat, startBeat + 1);

    this->broadcastLoopModeChanged(this->loopMode.get(),
        this->loopStartBeat.get(), this->loopEndBeat.get());
}

void Transport::disablePlaybackLoop()
{
    if (!this->loopMode.get())
    {
        return;
    }

    if (this->isPlaying())
    {
        this->stopPlaybackAndRecording();
    }

    this->loopMode = false;
    this->broadcastLoopModeChanged(false, 0.f, 0.f);
}

float Transport::getPlaybackLoopStart() const noexcept
{
    return this->loopStartBeat.get();
}

float Transport::getPlaybackLoopEnd() const noexcept
{
    return this->loopEndBeat.get();
}

//===----------------------------------------------------------------------===//
// Rendering
//===----------------------------------------------------------------------===//

bool Transport::startRender(const URL &renderTarget,
    RenderFormat format, int thumbnailResolution)
{
    if (this->renderer->isRendering())
    {
        return false;
    }
    
    return this->renderer->startRendering(renderTarget, format,
        this->fillPlaybackContextAt(this->getProjectFirstBeat()),
        thumbnailResolution);
}

void Transport::stopRender()
{
    if (! this->renderer->isRendering())
    {
        return;
    }
    
    this->renderer->stop();
}

bool Transport::isRendering() const
{
    return this->renderer->isRendering();
}

float Transport::getRenderingPercentsComplete() const
{
    return this->renderer->getPercentsComplete();
}

const Array<float, CriticalSection> &Transport::getRenderingWaveformThumbnail() const
{
    return this->renderer->getWaveformThumbnail();
}

//===----------------------------------------------------------------------===//
// Sending messages at real-time
//===----------------------------------------------------------------------===//

/*
    The purpose of this class is to help previewing messages interactively in the sequencer.
    It will send note-on events after a delay and keep track of note-off events which need
    to be sent later. All those previews will be cancelled by transport on stopSound.
    The delay before the note-on event is needed because some plugins (e.g. Kontakt in my case)
    are sometimes processing play/stop messages out of order for whatever reason, which is weird -
    note the word `queue` in the addMessageToQueue() method name - but it still happens
    in some scenarios, such as when the user drags some notes around quickly.
*/

Transport::NotePreviewTimer::~NotePreviewTimer()
{
    this->cancelAllPendingPreviews(false);
}

void Transport::NotePreviewTimer::cancelAllPendingPreviews(bool sendRemainingNoteOffs)
{
    if (this->isTimerRunning())
    {
        this->stopTimer();

        // can be accessed from a separate thread, like scale/metronome preview threads:
        const CriticalSection::ScopedLockType lock(this->previews.getLock());

        for (const auto &preview : this->previews)
        {
            if (sendRemainingNoteOffs &&
                preview.noteOnTimeoutMs <= 0 && // has sent the note-on message
                preview.noteOffTimeoutMs > 0 && // but hasn't sent the note-off message yet
                preview.instrument != nullptr)
            {
                const auto mapped = preview.instrument->getKeyboardMapping()->map(preview.key, preview.channel);
                MidiMessage message(MidiMessage::noteOff(mapped.channel, mapped.key));
                message.setTimeStamp(TIME_NOW);
                preview.instrument->getProcessorPlayer()
                    .getMidiMessageCollector().addMessageToQueue(message);
            }
        }

        this->previews.clearQuick();
    }
}

void Transport::NotePreviewTimer::previewNote(WeakReference<Instrument> instrument,
    int channel, int key, float volume, int16 noteOffTimeoutMs)
{
    jassert(key >= 0 && key < Globals::maxKeyboardSize);
    jassert(channel > 0 && channel <= Globals::numChannels);

    const CriticalSection::ScopedLockType lock(this->previews.getLock());

    for (auto &preview : this->previews)
    {
        if (preview.key == key && preview.channel == channel)
        {
            jassert(preview.instrument == instrument);

            if (preview.noteOnTimeoutMs <= 0 && 
                preview.noteOffTimeoutMs > 0 &&
                preview.instrument != nullptr)
            {
                const auto mapped = preview.instrument->getKeyboardMapping()->map(key, channel);
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

            return;
        }
    }

    Transport::NotePreviewTimer::KeyPreviewState preview;
    preview.key = key;
    preview.channel = channel;
    preview.volume = volume;
    preview.noteOnTimeoutMs = NotePreviewTimer::timerTickMs;
    preview.noteOffTimeoutMs = noteOffTimeoutMs;
    preview.instrument = instrument;
    this->previews.add(move(preview));

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

    const CriticalSection::ScopedLockType lock(this->previews.getLock());

    bool canStop = true;
    for (auto &preview : this->previews)
    {
        if (preview.noteOnTimeoutMs > 0)
        {
            canStop = false;
            preview.noteOnTimeoutMs -= NotePreviewTimer::timerTickMs;

            if (preview.noteOnTimeoutMs <= 0 && preview.instrument != nullptr)
            {
                const auto mapped = preview.instrument->getKeyboardMapping()->map(preview.key, preview.channel);
                auto message = MidiMessage::noteOn(mapped.channel, mapped.key, preview.volume);
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
                const auto mapped = preview.instrument->getKeyboardMapping()->map(preview.key, preview.channel);
                auto message = MidiMessage::noteOff(mapped.channel, mapped.key);
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

void Transport::previewKey(const String &trackId, int channel,
    int key, float volume, float lengthInBeats) const
{
    const auto foundLink = this->instrumentLinks.find(trackId);

    // in some cases we need to preview notes which are not tied to any
    // particular track/instrument, e.g. scale preview in the scale editor;
    // in these cases we'll just pick the default instrument
    // (might need to come up with a better solution in future though)
    const bool useDefaultInstrument = trackId.isEmpty() ||
        foundLink == this->instrumentLinks.end();

    auto *instrument = useDefaultInstrument ?
        this->orchestra.getDefaultInstrument() :
        foundLink.value().get();

    this->previewKey(instrument, channel, key, volume, lengthInBeats);
}

void Transport::previewKey(WeakReference<Instrument> instrument,
    int channel, int key, float volume, float lengthInBeats) const
{
    jassert(instrument != nullptr);

    // to calculate the note-off timeout interval, let's just use
    // the default 120 BPM for simplicity - instead of finding the tempo
    // at the note position, which I think is a bit of an overkill:
    const auto noteOffTimeoutMs = int16(Globals::Defaults::msPerBeat * lengthInBeats);
    this->notePreviewTimer.previewNote(instrument, channel, key, volume, noteOffTimeoutMs);
}

static void stopSoundForInstrument(Instrument *instrument)
{
    auto &collector = instrument->getProcessorPlayer().getMidiMessageCollector();

    for (int i = 1; i <= Globals::numChannels; ++i)
    {
        collector.addMessageToQueue(MidiMessage::allControllersOff(i).withTimeStamp(TIME_NOW));
        collector.addMessageToQueue(MidiMessage::allNotesOff(i).withTimeStamp(TIME_NOW));
        collector.addMessageToQueue(MidiMessage::allSoundOff(i).withTimeStamp(TIME_NOW));
    }
}

void Transport::stopSound(const String &trackId) const
{
    this->notePreviewTimer.cancelAllPendingPreviews(true);

    if (Instrument *instrument = this->instrumentLinks[trackId])
    {
        stopSoundForInstrument(instrument);
    }
    else
    {
        this->allNotesControllersAndSoundOff();
    }
}

void Transport::allNotesControllersAndSoundOff() const
{
    this->notePreviewTimer.cancelAllPendingPreviews(true);

    for (int i = 1; i <= Globals::numChannels; ++i)
    {
        const MidiMessage notesOff(MidiMessage::allNotesOff(i).withTimeStamp(TIME_NOW));
        const MidiMessage soundOff(MidiMessage::allSoundOff(i).withTimeStamp(TIME_NOW));
        const MidiMessage controllersOff(MidiMessage::allControllersOff(i).withTimeStamp(TIME_NOW));
        
        Array<const MidiMessageCollector *> uniqueMessageCollectors;
        
        for (int l = 0; l < this->tracksCache.size(); ++l)
        {
            const auto &trackId = this->tracksCache.getUnchecked(l)->getTrackId();
            auto *collector = &this->instrumentLinks[trackId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! uniqueMessageCollectors.contains(collector))
            {
                collector->addMessageToQueue(notesOff);
                collector->addMessageToQueue(controllersOff);
                collector->addMessageToQueue(soundOff);
                uniqueMessageCollectors.add(collector);
            }
        }
    }
}

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void Transport::onMetronomeFlagChanged(bool enabled)
{
    // metronome events are the part of playback cache,
    // so we will have to rebuild it when the playback starts:
    this->isMetronomeEnabled = enabled;
    this->stopPlaybackAndRecording();
    this->playbackCacheIsOutdated = true;
}

//===----------------------------------------------------------------------===//
// TimeSignaturesAggregator::Listener
//===----------------------------------------------------------------------===//

void Transport::onTimeSignaturesUpdated()
{
    // almost same logic as in onMetronomeFlagChanged:
    if (this->isMetronomeEnabled)
    {
        // this->stopPlaybackAndRecording(); // that's kinda too intrusive
        this->playbackCacheIsOutdated = true;
    }
}

//===----------------------------------------------------------------------===//
// OrchestraListener
//===----------------------------------------------------------------------===//

void Transport::onAddInstrument(Instrument *instrument)
{
    this->stopPlaybackAndRecording();

    // invalidate cache as is uses pointers to the players too
    this->playbackCacheIsOutdated = true;

    for (int i = 0; i < this->tracksCache.size(); ++i)
    {
        this->updateInstrumentLinkForTrack(this->tracksCache.getUnchecked(i));
    }
}

void Transport::onRemoveInstrument(Instrument *instrument)
{
    // the instrument stack have still not changed here,
    // so just stop the playback before it's too late
    this->stopPlaybackAndRecording();
}

void Transport::onPostRemoveInstrument()
{
    this->playbackCacheIsOutdated = true;

    for (int i = 0; i < this->tracksCache.size(); ++i)
    {
        this->updateInstrumentLinkForTrack(this->tracksCache.getUnchecked(i));
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void Transport::handlePossibleTempoChange(int trackControllerNumber)
{
    if (trackControllerNumber == MidiTrack::tempoController)
    {
        const auto totalTimeMs = this->findTimeAt(this->projectLastBeat.get());
        this->broadcastTotalTimeChanged(totalTimeMs);
    }
}

void Transport::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    // relying on isRecording() sucks,
    // but I don't see any easy workaround at the moment:
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    this->playbackCacheIsOutdated = true;
    this->handlePossibleTempoChange(newEvent.getTrackControllerNumber());
}

void Transport::onAddMidiEvent(const MidiEvent &event)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    this->playbackCacheIsOutdated = true;
    this->handlePossibleTempoChange(event.getTrackControllerNumber());
}

void Transport::onRemoveMidiEvent(const MidiEvent &event) {}
void Transport::onPostRemoveMidiEvent(MidiSequence *const sequence)
{
    this->stopPlaybackAndRecording();
    this->playbackCacheIsOutdated = true;
    this->handlePossibleTempoChange(sequence->getTrack()->getTrackControllerNumber());
}

void Transport::onAddClip(const Clip &clip)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    this->playbackCacheIsOutdated = true;
    this->handlePossibleTempoChange(clip.getTrackControllerNumber());

    if (clip.isSoloed())
    {
        this->hasSoloClipsCache = this->findSoloClipFlagIfAny();
    }
}

void Transport::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->stopPlaybackAndRecording();
    this->playbackCacheIsOutdated = true;
    this->handlePossibleTempoChange(newClip.getTrackControllerNumber());

    if (oldClip.isSoloed() != newClip.isSoloed())
    {
        this->hasSoloClipsCache = this->findSoloClipFlagIfAny();
    }
}

void Transport::onRemoveClip(const Clip &clip) {}
void Transport::onPostRemoveClip(Pattern *const pattern)
{
    this->stopPlaybackAndRecording();
    this->playbackCacheIsOutdated = true;
    this->handlePossibleTempoChange(pattern->getTrack()->getTrackControllerNumber());
    this->hasSoloClipsCache = this->findSoloClipFlagIfAny();
}

void Transport::onChangeTrackProperties(MidiTrack *const track)
{
    // Stop playback only when instrument changes:
    if (!instrumentLinks.contains(track->getTrackId()) ||
        this->instrumentLinks[track->getTrackId()]->getInstrumentId() != track->getTrackInstrumentId())
    {
        if (!this->isRecording())
        {
            this->stopPlayback();
        }

        this->playbackCacheIsOutdated = true;
        this->updateInstrumentLinkForTrack(track);
    }
}

void Transport::updateTemperamentForBuiltInSynth(Temperament::Ptr temperament) const
{
    const auto *defaultInstrument = this->orchestra.getDefaultInstrument();
    if (auto mainNode = defaultInstrument->findMainPluginNode())
    {
        if (auto *synth = dynamic_cast<DefaultSynthAudioPlugin *>(mainNode->getProcessor()))
        {
            synth->setTemperament(temperament);
        }
    }
}

void Transport::onDeactivateProjectSubtree(const ProjectMetadata *meta)
{
    this->stopPlaybackAndRecording();
}

void Transport::onActivateProjectSubtree(const ProjectMetadata *meta)
{
    this->updateTemperamentForBuiltInSynth(meta->getTemperament());

    // let's reset midi caches, just in case some instrument's keyboard mapping
    // has changed in the meanwhile (no idea how to observe kbm changes in transport)
    this->playbackCacheIsOutdated = true;
}

void Transport::onChangeProjectInfo(const ProjectMetadata *meta)
{
    this->updateTemperamentForBuiltInSynth(meta->getTemperament());
}

void Transport::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->playbackCacheIsOutdated = true;

    this->tracksCache.clearQuick();
    this->instrumentLinks.clear();

    this->tracksCache.addArray(tracks);
    for (const auto &track : tracks)
    {
        this->updateInstrumentLinkForTrack(track);
    }

    this->hasSoloClipsCache = this->findSoloClipFlagIfAny();

    this->stopPlaybackAndRecording();

    this->updateTemperamentForBuiltInSynth(meta->getTemperament());
}

void Transport::onAddTrack(MidiTrack *const track)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    this->playbackCacheIsOutdated = true;
    this->tracksCache.addIfNotAlreadyThere(track);
    this->updateInstrumentLinkForTrack(track);
    this->hasSoloClipsCache = this->findSoloClipFlagIfAny();
}

void Transport::onRemoveTrack(MidiTrack *const track)
{
    this->stopPlaybackAndRecording();

    this->playbackCacheIsOutdated = true;
    this->tracksCache.removeAllInstancesOf(track);
    this->clearInstrumentLinkForTrack(track);
    this->hasSoloClipsCache = this->findSoloClipFlagIfAny();
}

void Transport::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    if (!this->isRecording())
    {
        this->stopPlayback();
    }

    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;
    
    // real track total time changed
    const auto realLengthMs = this->findTimeAt(lastBeat);
    this->broadcastTotalTimeChanged(realLengthMs);
}

//===----------------------------------------------------------------------===//
// Track time calculations
//===----------------------------------------------------------------------===//

double Transport::findTimeAt(float targetBeat) const
{
    double resultTimeMs = 0.0;

    this->rebuildPlaybackCacheIfNeeded();

    CachedMidiMessage cached;

    // try to find the initial value for global tempo
    // by picking the very first tempo automation event, if present
    double tempo = Globals::Defaults::msPerBeat;

    this->playbackCache.seekToStart();
    while (this->playbackCache.getNextMessage(cached))
    {
        if (cached.message.isTempoMetaEvent())
        {
            tempo = cached.message.getTempoSecondsPerQuarterNote() * 1000.f;
            break;
        }
    }

    double prevTimestamp = this->projectFirstBeat.get();

    this->playbackCache.seekToStart();
    while (this->playbackCache.getNextMessage(cached))
    {
        const auto nextTimestamp = cached.message.getTimeStamp();

        if (nextTimestamp > targetBeat)
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
    resultTimeMs += (tempo * (targetBeat - prevTimestamp));

    return resultTimeMs;
}

Transport::PlaybackContext::Ptr Transport::fillPlaybackContextAt(float targetBeat) const
{
    this->rebuildPlaybackCacheIfNeeded();

    CachedMidiMessage cached;

    // try to find the initial value for global tempo
    // by picking the very first tempo automation event, if present
    double tempo = Globals::Defaults::msPerBeat;

    this->playbackCache.seekToStart();
    while (this->playbackCache.getNextMessage(cached))
    {
        if (cached.message.isTempoMetaEvent())
        {
            tempo = cached.message.getTempoSecondsPerQuarterNote() * 1000.f;
            break;
        }
    }

    Transport::PlaybackContext::Ptr context(new Transport::PlaybackContext());
    context->startBeat = targetBeat;

    context->totalTimeMs = 0.0;
    context->startBeatTimeMs = 0.0;
    context->startBeatTempo = tempo;

    context->sampleRate = this->playbackCache.getSampleRate();
    context->numOutputChannels = this->playbackCache.getNumOutputChannels();

    double prevTimestamp = this->projectFirstBeat.get();
    bool startBeatPassed = false;

    this->playbackCache.seekToStart();
    while (this->playbackCache.getNextMessage(cached))
    {
        const auto nextTimestamp = cached.message.getTimeStamp();
        const auto nextEventTimeDelta = tempo * (nextTimestamp - prevTimestamp);

        if (nextTimestamp > context->startBeat)
        {
            // the time from the last event to the given beat:
            context->startBeatTimeMs += (tempo * (context->startBeat - prevTimestamp));
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
            const auto channel = jlimit(0, Globals::numChannels - 1, cached.message.getChannel() - 1);
            context->ccStates[cached.message.getControllerNumber()][channel] =
                cached.message.getControllerValue();
        }

        if (!startBeatPassed)
        {
            context->startBeatTempo = tempo;
        }
    }

    // the remainder
    context->totalTimeMs += (tempo * (this->projectLastBeat.get() - prevTimestamp));
    //jassert(context->totalTimeMs == this->findTimeAt(this->projectLastBeat.get()));

    return context;
}

//===----------------------------------------------------------------------===//
// Playback cache management
//===----------------------------------------------------------------------===//

bool Transport::findSoloClipFlagIfAny() const
{
    for (const auto *track : this->tracksCache)
    {
        if (track->getPattern() != nullptr &&
            track->getPattern()->hasSoloClips())
        {
            return true;
        }
    }

    return false;
}

void Transport::rebuildPlaybackCacheIfNeeded() const
{
    if (this->playbackCacheIsOutdated.get())
    {
        this->playbackCache = this->buildPlaybackCache(this->isMetronomeEnabled);
        this->playbackCacheIsOutdated = false;
    }
}

TransportPlaybackCache Transport::buildPlaybackCache(bool withMetronome) const
{
    TransportPlaybackCache result;
    
    this->hasSoloClipsCache = this->findSoloClipFlagIfAny();
    auto &generatedSequences = *this->project.getGeneratedSequences();

    for (const auto *track : this->tracksCache)
    {
        const auto instrument = this->instrumentLinks[track->getTrackId()];
        jassert(instrument != nullptr);
        const auto &keyMapping = *instrument->getKeyboardMapping();

        auto cached = CachedMidiSequence::createFrom(instrument, track->getSequence());

        if (track->getPattern() != nullptr)
        {
            for (const auto *clip : track->getPattern()->getClips())
            {
                cached->track->exportMidi(cached->midiMessages, *clip,
                    keyMapping, generatedSequences,
                    this->hasSoloClipsCache, withMetronome,
                    this->projectFirstBeat.get(), this->projectLastBeat.get());
            }
        }
        else
        {
            static Clip noTransform;
            cached->track->exportMidi(cached->midiMessages, noTransform,
                keyMapping, generatedSequences,
                this->hasSoloClipsCache, withMetronome,
                this->projectFirstBeat.get(), this->projectLastBeat.get());
        }

        result.addWrapper(cached);
    }

    return result;
}

// returning by value, because it will be used by (possibly many) player threads,
// so we'd rather play safe and just let them deal with their own copy of it;
// internally, the data is refcounted anyway and protected by critical sections
TransportPlaybackCache Transport::getPlaybackCache()
{
    return this->playbackCache;
}

void Transport::updateInstrumentLinkForTrack(const MidiTrack *track)
{
    const auto instruments = this->orchestra.getInstruments();
    
    // check by ids
    for (int i = 0; i < instruments.size(); ++i)
    {
        auto *instrument = instruments.getUnchecked(i);
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentId()))
        {
            // corresponding node already exists, lets add
            this->instrumentLinks[track->getTrackId()] = instrument;
            return;
        }
    }
    
    // check by hashes
    for (int i = 0; i < instruments.size(); ++i)
    {
        auto *instrument = instruments.getUnchecked(i);
        if (track->getTrackInstrumentId().contains(instrument->getInstrumentHash()))
        {
            this->instrumentLinks[track->getTrackId()] = instrument;
            return;
        }
    }
    
    // set default instrument, if none found
    this->instrumentLinks[track->getTrackId()] = this->orchestra.getDefaultInstrument();
}

void Transport::clearInstrumentLinkForTrack(const MidiTrack *track)
{
    this->instrumentLinks.erase(track->getTrackId());
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

void Transport::broadcastSeek(float beat)
{
    this->transportListeners.call(&TransportListener::onSeek, beat);
}

void Transport::broadcastCurrentTempoChanged(double newTempo)
{
    this->transportListeners.call(&TransportListener::onCurrentTempoChanged, newTempo);
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

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

Transport::Transport(OrchestraPit &orchestraPit) :
    orchestra(orchestraPit),
    seekPosition(0.0),
    trackStartMs(0.0),
    trackEndMs(0.0),
    sequencesAreOutdated(true),
    totalTime(500.0 * 8.0),
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

int Transport::getTempoByCV(float controllerValue) noexcept
{
    const float maxMsPerQuarter = 250.f; // is 240 bpm (=250 ms-per-quarter) enough?
    const float safeCV = jlimit(0.00005f, 0.99999f, controllerValue);
    return int((1.f - AudioCore::fastLog2(safeCV)) * maxMsPerQuarter * 1000);
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
    
    this->setSeekPosition(absPosition);
    this->broadcastSeek(absPosition, timeMs, realLengthMs);
}

void Transport::probeSoundAt(double absTrackPosition, const MidiSequence *limitToLayer)
{
    this->rebuildSequencesIfNeeded();
    
    const double targetFlatTime = this->getTotalTime() * absTrackPosition;
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
                    messageTimestampedAsNow.setTimeStamp(TIME_NOW);
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

    MidiMessageSequence fixedSequence(sequence);
    const double startPositionInTime = this->getSeekPosition() * this->getTotalTime();
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
    
    this->player->startPlayback();
    this->broadcastPlay();
}

void Transport::startPlaybackFragment(double absLoopStart, double absLoopEnd, bool looped)
{
    this->rebuildSequencesIfNeeded();
    
    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
    }
        
    this->player->startPlayback(absLoopStart, absLoopEnd, looped);
    this->broadcastPlay();
}

void Transport::stopPlayback()
{
    if (this->player->isPlaying())
    {
        this->player->stopPlayback();
        this->allNotesControllersAndSoundOff();
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

void Transport::sendMidiMessage(const String &trackId, const MidiMessage &message) const
{
    MidiMessage messageTimestampedAsNow(message);
    
#if HELIO_MOBILE
    // iSEM tends to hang >_< if too many messages are send simultaniously
    messageTimestampedAsNow.setTimeStamp(TIME_NOW + float(rand() % 50) * 0.01);
#elif HELIO_DESKTOP
    messageTimestampedAsNow.setTimeStamp(TIME_NOW);
#endif
    
    auto &collector = this->linksCache[trackId]->getProcessorPlayer().getMidiMessageCollector();
    collector.addMessageToQueue(messageTimestampedAsNow);
}

void Transport::allNotesAndControllersOff() const
{
    static const int c = 1;
    //for (int c = 1; c <= 16; ++c)
    {
        MidiMessage notesOff(MidiMessage::allNotesOff(c));
        MidiMessage controllersOff(MidiMessage::allControllersOff(c));
        
        Array<const MidiMessageCollector *> duplicateCollectors;
        
        for (int l = 0; l < this->tracksCache.size(); ++l)
        {
            const String &trackId = this->tracksCache.getUnchecked(l)->getTrackId();
            auto *collector = &this->linksCache[trackId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! duplicateCollectors.contains(collector))
            {
                duplicateCollectors.add(collector);

                notesOff.setTimeStamp(TIME_NOW);
                collector->addMessageToQueue(notesOff);

                controllersOff.setTimeStamp(TIME_NOW);
                collector->addMessageToQueue(controllersOff);
            }
        }
    }
}

void Transport::allNotesControllersAndSoundOff() const
{
    static const int c = 1;
    //for (int c = 1; c <= 16; ++c)
    {
        MidiMessage notesOff(MidiMessage::allNotesOff(c));
        MidiMessage soundOff(MidiMessage::allSoundOff(c));
        MidiMessage controllersOff(MidiMessage::allControllersOff(c));
        
        Array<const MidiMessageCollector *> duplicateCollectors;
        
        for (int l = 0; l < this->tracksCache.size(); ++l)
        {
            const auto &trackId = this->tracksCache.getUnchecked(l)->getTrackId();
            auto *collector = &this->linksCache[trackId]->getProcessorPlayer().getMidiMessageCollector();
            
            if (! duplicateCollectors.contains(collector))
            {
                notesOff.setTimeStamp(TIME_NOW);
                collector->addMessageToQueue(notesOff);

                controllersOff.setTimeStamp(TIME_NOW);
                collector->addMessageToQueue(controllersOff);

                soundOff.setTimeStamp(TIME_NOW);
                collector->addMessageToQueue(soundOff);

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
    this->sequencesAreOutdated = true;

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

    const double newBeatRange = (lastBeat - firstBeat); // may also be 0
    const double newSeekPosition = ((newBeatRange == 0.0) ? 0.0 : ((seekBeat - firstBeat) / newBeatRange));
    
    //
    //          |----------- 0.7 ----|
    // |--------+----------- 0.5 ----+---------------|
    //
    //  1. compute seek position as beat
    //  2. compute (seekBeat - newFirstBeat) / (newLastBeat - newFirstBeat)
    //
    
    this->trackStartMs = double(firstBeat);
    this->trackEndMs = double(lastBeat);
    this->setTotalTime(this->trackEndMs.get() - this->trackStartMs.get());
    
    // real track total time changed
    double tempo = 0.0;
    double realLengthMs = 0.0;
    this->calcTimeAndTempoAt(1.0, realLengthMs, tempo);
    this->broadcastTotalTimeChanged(realLengthMs);
    
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
    
    const double targetTime = targetAbsPosition * this->getTotalTime();
    
    outTimeMs = 0.0;
    outTempo = 500.0; // default 120 BPM
    
    double prevTimestamp = 0.0;
    double nextEventTimeDelta = 0.0;
    bool foundFirstTempoEvent = false;
    
    MessageWrapper wrapper;
    
    while (this->sequences.getNextMessage(wrapper))
    {
        const double nextAbsPosition = wrapper.message.getTimeStamp() / this->getTotalTime();
        
        // first tempo event sets the tempo all the way before it
        if (nextAbsPosition > targetAbsPosition && foundFirstTempoEvent) { break; }
        
        nextEventTimeDelta = outTempo * (wrapper.message.getTimeStamp() - prevTimestamp);
        outTimeMs += nextEventTimeDelta;
        prevTimestamp = wrapper.message.getTimeStamp();
        
        if (wrapper.message.isTempoMetaEvent())
        {
            outTempo = wrapper.message.getTempoSecondsPerQuarterNote() * 1000.f;
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
    
    // return default 120 bpm (== 500 ms per quarter note)
    return MidiMessage::tempoMetaEvent(500 * 1000);
}

//===----------------------------------------------------------------------===//
// Sequences management
//===----------------------------------------------------------------------===//

void Transport::rebuildSequencesIfNeeded()
{
    if (this->sequencesAreOutdated)
    {
        this->sequences.clear();
        static Clip noTransform;
        const double offset = -this->trackStartMs.get();

        for (const auto *track : this->tracksCache)
        {
            auto instrument = this->linksCache[track->getTrackId()];
            jassert(instrument != nullptr);

            ScopedPointer<SequenceWrapper> wrapper(new SequenceWrapper());
            wrapper->track = track->getSequence();
            wrapper->currentIndex = 0;
            wrapper->instrument = instrument;
            wrapper->listener = &instrument->getProcessorPlayer().getMidiMessageCollector();

            if (track->getPattern() != nullptr)
            {
                for (const auto *clip : track->getPattern()->getClips())
                {
                    wrapper->track->exportMidi(wrapper->midiMessages, *clip, offset, 1.0);
                }
            }
            else
            {
                wrapper->track->exportMidi(wrapper->midiMessages, noTransform, offset, 1.0);
            }

            if (wrapper->midiMessages.getNumEvents() > 0)
            {
                this->sequences.addWrapper(wrapper.release());
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

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

#pragma once

class ProjectNode;
class OrchestraPit;
class PlayerThread;
class PlayerThreadPool;
class RendererThread;

#include "TransportListener.h"
#include "TransportPlaybackCache.h"
#include "TimeSignaturesAggregator.h"
#include "OrchestraListener.h"
#include "ProjectListener.h"
#include "RenderFormat.h"
#include "Instrument.h"
#include "Temperament.h"
#include "UserInterfaceFlags.h"
#include "Config.h"

class Transport final : public Serializable,
    public ProjectListener,
    public OrchestraListener,
    public TimeSignaturesAggregator::Listener,
    public UserInterfaceFlags::Listener // needs the metronome on/off flag changes
{
public:

    Transport(ProjectNode &project, OrchestraPit &orchestraPit);
    ~Transport() override;
    
    static String getTimeString(double timeMs, bool includeMilliseconds = false);
    static String getTimeString(const RelativeTime &relTime, bool includeMilliseconds = false);

    // Returns microseconds per quarter note
    static int getTempoByControllerValue(float controllerValue) noexcept;
    static float getControllerValueByTempo(double secondsPerQuarterNote) noexcept;

    //===------------------------------------------------------------------===//
    // Transport
    //===------------------------------------------------------------------===//
    
    float getSeekBeat() const noexcept;
    void seekToBeat(float beatPosition);
    
    void probeSoundAtBeat(float beatPosition,
        const MidiSequence *limitedTo = nullptr);

    void startPlayback();
    void startPlayback(float startBeatOverride);
    void startPlaybackFragment(float startBeat, float endBeat, bool looped);

    bool isPlaying() const;
    void stopPlayback();
    void toggleStartStopPlayback();

    void startRecording();
    bool isRecording() const;
    void stopRecording();

    bool isPlayingAndRecording() const;
    void stopPlaybackAndRecording();

    bool startRender(const URL &renderTarget, RenderFormat format, int thumbnailResolution);
    bool isRendering() const;
    void stopRender();
    
    void togglePlaybackLoop(float startBeat, float endBeat);
    void setPlaybackLoop(float startBeat, float endBeat);
    void disablePlaybackLoop();
    float getPlaybackLoopStart() const noexcept;
    float getPlaybackLoopEnd() const noexcept;

    float getRenderingPercentsComplete() const;
    const Array<float, CriticalSection> &getRenderingWaveformThumbnail() const;

    //===------------------------------------------------------------------===//
    // Playback context and caches
    //===------------------------------------------------------------------===//

    double findTimeAt(float beat) const;

    struct PlaybackContext final : public ReferenceCountedObject
    {
        using Ptr = ReferenceCountedObjectPtr<PlaybackContext>;

        PlaybackContext()
        {
            memset(this->ccStates, -1, sizeof(this->ccStates));
        }

        float startBeat = 0.f;
        float rewindBeat = 0.f;
        float endBeat = 0.f;

        double startBeatTempo = 0.0; // ms per beat (or per quarter-note)
        double startBeatTimeMs = 0.0;
        double totalTimeMs = 0.0;

        double sampleRate = 0.0;
        int numOutputChannels = 0;

        bool playbackLoopMode = false;

        // computed CC values: -1 if not found in any track,
        // otherwise, the controller value at the time of playback start;
        // CC numbers 102–119 are undefined, and numbers 120-127 are
        // reserved for channel mode messages, which we will ignore
        static constexpr auto numCCs = 101;
        int ccStates[numCCs + 1][Globals::numChannels];
    };

    PlaybackContext::Ptr fillPlaybackContextAt(float beat) const;

    TransportPlaybackCache getPlaybackCache();

    float getProjectFirstBeat() const noexcept
    {
        return this->projectFirstBeat.get();
    }

    float getProjectLastBeat() const noexcept
    {
        return this->projectLastBeat.get();
    }

    bool hasSoloClips() const noexcept
    {
        return this->hasSoloClipsCache;
    }

    //===------------------------------------------------------------------===//
    // Sending messages in real-time
    //===------------------------------------------------------------------===//
    
    void previewKey(const String &trackId, int channel,
        int key, float volume, float lengthInBeats) const;
    void previewKey(WeakReference<Instrument> instrument, int channel,
        int key, float volume, float lengthInBeats) const;

    void stopSound(const String &trackId = "") const;
    void allNotesControllersAndSoundOff() const;

    //===------------------------------------------------------------------===//
    // UserInterfaceFlags::Listener
    //===------------------------------------------------------------------===//

    void onMetronomeFlagChanged(bool enabled) override;

    //===------------------------------------------------------------------===//
    // TimeSignaturesAggregator::Listener
    //===------------------------------------------------------------------===//

    void onTimeSignaturesUpdated() override;

    //===------------------------------------------------------------------===//
    // OrchestraListener
    //===------------------------------------------------------------------===//

    void onAddInstrument(Instrument *instrument) override;
    void onRemoveInstrument(Instrument *instrument) override;
    void onPostRemoveInstrument() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//
    
    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;
    void onPostRemoveMidiEvent(MidiSequence *const layer) override;

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;
    void onPostRemoveClip(Pattern *const pattern) override;

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override {}
    void onChangeProjectInfo(const ProjectMetadata *info) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

    void onActivateProjectSubtree(const ProjectMetadata *meta) override;
    void onDeactivateProjectSubtree(const ProjectMetadata *meta) override;

    //===------------------------------------------------------------------===//
    // Listeners management
    //===------------------------------------------------------------------===//

    void addTransportListener(TransportListener *listener);
    void removeTransportListener(TransportListener *listener);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    void setSeekBeat(float beatPosition);

    void broadcastPlay();
    void broadcastStop();
    void broadcastRecord();
    void broadcastRecordFailed(const Array<MidiDeviceInfo> &devices);
    void broadcastTotalTimeChanged(double timeMs);
    void broadcastLoopModeChanged(bool hasLoop, float startBeat, float endBeat);

    void broadcastSeek(float newBeat, double currentTimeMs);
    void broadcastCurrentTempoChanged(double newTempo);

    friend class PlayerThread;
    friend class PlayerThreadPool;
    friend class RendererThread;

private:
    
    ProjectNode &project;
    OrchestraPit &orchestra;

    UniquePointer<PlayerThreadPool> player;
    UniquePointer<RendererThread> renderer;

private:

    mutable TransportPlaybackCache playbackCache;
    mutable Atomic<bool> playbackCacheIsOutdated = true;
    void rebuildPlaybackCacheIfNeeded() const;
    TransportPlaybackCache buildPlaybackCache(bool withMetronome) const;

    mutable bool hasSoloClipsCache = false;
    bool findSoloClipFlagIfAny() const;

    // <track id : instrument>
    mutable Array<const MidiTrack *> tracksCache;
    mutable FlatHashMap<String, WeakReference<Instrument>, StringHash> instrumentLinks;
    
    void updateInstrumentLinkForTrack(const MidiTrack *track);
    void clearInstrumentLinkForTrack(const MidiTrack *track);
    
    // a nasty hack, see the description in DefaultSynth.h:
    void updateTemperamentForBuiltInSynth(Temperament::Ptr temperament) const;

    inline void handlePossibleTempoChange(int trackControllerNumber);

private:

    class NotePreviewTimer final : private Timer
    {
    public:

        NotePreviewTimer() = default;
        ~NotePreviewTimer();

        void cancelAllPendingPreviews(bool sendRemainingNoteOffs);
        void previewNote(WeakReference<Instrument> instrument,
            int channel, int key, float volume, int16 noteOffTimeoutMs);

    private:

        void timerCallback() override;

        struct KeyPreviewState final
        {
            int key = 0;
            int channel = 0;
            float volume = 0.f;
            int noteOnTimeoutMs = 0;
            int noteOffTimeoutMs = 0;
            WeakReference<Instrument> instrument;
        };

        static constexpr auto timerTickMs = 50;

        Array<KeyPreviewState, CriticalSection> previews;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotePreviewTimer)
    };

    mutable NotePreviewTimer notePreviewTimer;

private:

    Atomic<float> seekBeat = 0.0;
    Atomic<float> projectFirstBeat = 0.f;
    Atomic<float> projectLastBeat = Globals::Defaults::projectLength;

    // just a flag, all logic resides in MidiRecorder class:
    Atomic<bool> midiRecordingMode = false;

    Atomic<bool> loopMode = false;
    Atomic<float> loopStartBeat = 0.f;
    Atomic<float> loopEndBeat = Globals::Defaults::projectLength;

    bool isMetronomeEnabled = App::Config().getUiFlags()->isMetronomeEnabled();

    ListenerList<TransportListener> transportListeners;

    JUCE_DECLARE_WEAK_REFERENCEABLE(Transport)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transport)
};

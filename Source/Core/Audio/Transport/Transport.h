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

#pragma once

class Instrument;
class OrchestraPit;
class PlayerThread;
class RendererThread;

#include "TransportListener.h"
#include "ProjectSequencesWrapper.h"
#include "ProjectListener.h"
#include "OrchestraListener.h"

class Transport : public ProjectListener, private OrchestraListener
{
public:

    explicit Transport(OrchestraPit &orchestraPit);

    ~Transport() override;

    static const int millisecondsPerBeat = 500;
    
    static String getTimeString(double timeMs, bool includeMilliseconds = false);

    static String getTimeString(const RelativeTime &relTime, bool includeMilliseconds = false);
    
    
    //===------------------------------------------------------------------===//
    // Transport
    //===------------------------------------------------------------------===//
    
    double getSeekPosition() const;
    double getTotalTime() const;
    void seekToPosition(double absPosition);
    
    void probeSoundAt(double absTrackPosition,
                      const MidiLayer *limitToLayer = nullptr);

    
    void startPlaybackLooped(double absLoopStart, double absLoopEnd);
    bool isLooped() const;
    double getLoopStart() const;
    double getLoopEnd() const;
    
    void startPlayback();
    bool isPlaying() const;
    void stopPlayback();
    
    void startRender(const String &filename);
    bool isRendering() const;
    void stopRender();
    
    float getRenderingPercentsComplete() const;
    
    void calcTimeAndTempoAt(const double absPosition,
                            double &outTimeMs,
                            double &outTempo);

    MidiMessage findFirstTempoEvent();

    void rebuildSequencesInRealtime();

    
    //===------------------------------------------------------------------===//
    // Sending messages at realtime
    //===------------------------------------------------------------------===//
    
    void sendMidiMessage(const String &layerId, const MidiMessage &message) const;
    
    void allNotesAndControllersOff() const;

    void allNotesControllersAndSoundOff() const;

    
    //===------------------------------------------------------------------===//
    // OrchestraListener
    //===------------------------------------------------------------------===//

    void instrumentAdded(Instrument *instrument) override;
    
    void instrumentRemoved(Instrument *instrument) override;

    void instrumentRemovedPostAction() override;
    
    
    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//
    
    void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    
    void onAddMidiEvent(const MidiEvent &event) override;
    
    void onRemoveMidiEvent(const MidiEvent &event) override;
    
    void onPostRemoveMidiEvent(const MidiLayer *layer) override;

    void onChangeMidiLayer(const MidiLayer *layer) override;
    
    void onAddMidiLayer(const MidiLayer *layer) override;
    
    void onRemoveMidiLayer(const MidiLayer *layer) override; // ���������� ����� ����� ��������� ����
    
    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    

    //===------------------------------------------------------------------===//
    // Listeners management
    //===------------------------------------------------------------------===//

    void addTransportListener(TransportListener *listener);

    void removeTransportListener(TransportListener *listener);

protected:

    void setTotalTime(const double val);
    void setSeekPosition(const double absPosition);

    void broadcastTempoChanged(const double newTempo);
    void broadcastTotalTimeChanged(const double timeMs);
    void broadcastPlay();
    void broadcastStop();
    void broadcastSeek(const double newPosition,
                       const double currentTimeMs,
                       const double totalTimeMs);

private:
    
    OrchestraPit &orchestra;

    ScopedPointer<PlayerThread> player;
    ScopedPointer<RendererThread> renderer;
    
    friend class PlayerThread;
    friend class RendererThread;

private:

    ProjectSequences getSequences();
    void rebuildSequencesIfNeeded();
    
    ProjectSequences sequences;
    bool sequencesAreOutdated;
    
    Array<const MidiLayer *> layersCache;
    HashMap<String, Instrument *> linksCache; // layer id : instrument
    
    void updateLinkForLayer(const MidiLayer *layer);
    void removeLinkForLayer(const MidiLayer *layer);
    
private:
    
    bool loopedMode;
    double loopStart;
    double loopEnd;
    
private:

    ReadWriteLock seekPositionLock;
    double seekPosition;

    ReadWriteLock totalTimeLock;
    double totalTime;
    
    double trackStartMs;
    double trackEndMs;
    
    float projectFirstBeat;
    float projectLastBeat;

    ListenerList<TransportListener> transportListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transport)
};

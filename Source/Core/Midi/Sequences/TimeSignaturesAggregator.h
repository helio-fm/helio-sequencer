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
class MidiSequence;
class TimeSignatureEvent;
class TimeSignaturesSequence;
class DummyProjectEventDispatcher;

#include "MidiTrack.h"
#include "ProjectListener.h"

// A class responsible for maintaining an ordered list of
// all currently used time signatures: the ones that are coming
// from the timeline are replaced by the ones of the selected track(s).

// It is used by RollBase to determine where to draw the grid lines,
// and by TimeSignaturesProjectMap for displaying the time signatures.

// It is also a "virtual" MidiTrack, which allows us to use it
// as a drop-in replacement for the timeline's time signatures track.

class TimeSignaturesAggregator final : public VirtualMidiTrack, public ProjectListener
{
public:

    TimeSignaturesAggregator(ProjectNode &parentProject,
        MidiSequence &timelineSignatures);

    ~TimeSignaturesAggregator() override;

    // should be called by the rolls:
    void setActiveScope(Array<WeakReference<MidiTrack>> selectedTracks,
        bool forceRebuildAll = false);

    // time signature aggregator may decide that the default grid
    // should display something else than 4/4 at the project start
    // (e.g. when the timeline has no time signatures and no clips selected,
    // it may tell to use the time signature of the last selected clip,
    // or one of the most frequently used signatures, or something like that);
    // all passed params are expected to be initialized:
    void updateGridDefaultsIfNeeded(int &numerator,
        int &denominator, float &startBeat) const noexcept;

    //===------------------------------------------------------------------===//
    // Listeners management
    //===------------------------------------------------------------------===//

    struct Listener
    {
        Listener() = default;
        virtual ~Listener() = default;
        virtual void onTimeSignaturesUpdated() {}
    };

    void addListener(Listener *listener);
    void removeListener(Listener *listener);
    void removeAllListeners();

    //===------------------------------------------------------------------===//
    // VirtualMidiTrack
    //===------------------------------------------------------------------===//

    String getTrackInstrumentId() const noexcept override;
    MidiSequence *getSequence() const noexcept override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onPostRemoveMidiEvent(MidiSequence *const sequence) override;

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;

    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;
    void onChangeTrackBeatRange(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

private:

    ProjectNode &project;
    MidiSequence &timelineSignatures;

    Array<WeakReference<MidiTrack>> selectedTracks;

    void rebuildAll();
    bool isAggregatingTimeSignatureOverrides() const noexcept;

    UniquePointer<DummyProjectEventDispatcher> dummyEventDispatcher;
    UniquePointer<TimeSignaturesSequence> orderedEvents;

    int defaultGridNumerator = Globals::Defaults::timeSignatureNumerator;
    int defaultGridDenominator = Globals::Defaults::timeSignatureDenominator;
    float defaultGridStart = 0.f;

    ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignaturesAggregator)
    JUCE_DECLARE_WEAK_REFERENCEABLE(TimeSignaturesAggregator)
};

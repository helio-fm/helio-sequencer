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

class ProjectNode;
class TimeSignatureEvent;
class TimeSignaturesSequence;

#include "MidiTrack.h"
#include "ProjectListener.h"

// A class responsible for maintaining an ordered list of
// all currently used time signatures: the ones that are coming
// from the timeline are replaced by the ones of the selected track,
// if a single track is selected and if it has a time signature.

// It is used by RollBase to determine where to draw the grid lines,
// and by TimeSignaturesProjectMap for displaying the time signatures.

class TimeSignaturesAggregator final : public ProjectListener
{
public:

    TimeSignaturesAggregator(ProjectNode &parentProject,
        TimeSignaturesSequence &timelineSignatures);

    ~TimeSignaturesAggregator() override;

    // Called by the rolls:
    void setActiveScope(WeakReference<MidiTrack> selectedTrack);

    const Array<TimeSignatureEvent> &getAllOrdered() const noexcept;

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
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;

    void onAddTrack(MidiTrack *const track) override {}
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;
    void onChangeTrackBeatRange(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

private:

    ProjectNode &project;
    TimeSignaturesSequence &timelineSignatures;

    // todo how to store all events?

    WeakReference<MidiTrack> selectedTrack = nullptr;

    void rebuildAll();

    Array<TimeSignatureEvent> orderedEvents;

    ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignaturesAggregator)
};

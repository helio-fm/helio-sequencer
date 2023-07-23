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
#include "TimeSignaturesAggregator.h"
#include "TimeSignaturesSequence.h"
#include "TimeSignatureEvent.h"
#include "ProjectNode.h"
#include "ProjectTimeline.h"
#include "Pattern.h"
#include "Workspace.h"
#include "AudioCore.h"

// TimeSignaturesAggregator acts like a virtual midi track
// (so it can be used in the export and for building the playback midi data),
// and for that it maintains a "virtual" midi sequence, which it rebuilds
// on the fly, depending on user selection; and for that reason
// it uses its own listener interface instead of ProjectListener;
// so this class is here to help to avoid its virtual sequence sending
// ProjectListener events (hopefully someday I'll come up with a cleaner approach,
// but for now let's just send sequence's change events nowhere)
class DummyProjectEventDispatcher final : public ProjectEventDispatcher
{
public:

    explicit DummyProjectEventDispatcher(ProjectNode &project) :
        project(project) {}

    void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override {}
    void dispatchAddEvent(const MidiEvent &event) override {}
    void dispatchRemoveEvent(const MidiEvent &event) override {}
    void dispatchPostRemoveEvent(MidiSequence *const layer) override {}

    void dispatchAddClip(const Clip &clip) override {}
    void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) override {}
    void dispatchRemoveClip(const Clip &clip) override {}
    void dispatchPostRemoveClip(Pattern *const pattern) override {}

    void dispatchChangeTrackProperties() override {}
    void dispatchChangeTrackBeatRange() override {}
    void dispatchChangeProjectBeatRange() override {}

    ProjectNode *getProject() const noexcept override
    {
        return &this->project;
    }

private:

    ProjectNode &project;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DummyProjectEventDispatcher)
};


TimeSignaturesAggregator::TimeSignaturesAggregator(ProjectNode &parentProject,
    MidiSequence &timelineSignatures) :
    project(parentProject),
    timelineSignatures(timelineSignatures)
{
    this->dummyEventDispatcher = make<DummyProjectEventDispatcher>(this->project);
    this->project.addListener(this);
}

TimeSignaturesAggregator::~TimeSignaturesAggregator()
{
    this->removeAllListeners();
    this->project.removeListener(this);
}

static inline bool trackHasTimeSignature(MidiTrack *track) noexcept
{
    return track != nullptr && track->hasTimeSignatureOverride();
}

bool TimeSignaturesAggregator::isAggregatingTimeSignatureOverrides() const noexcept
{
    for (const auto &selectedTrack : this->selectedTracks)
    {
        if (trackHasTimeSignature(selectedTrack))
        {
            return true;
        }
    }

    return false; // or the timeline is empty?
}

void TimeSignaturesAggregator::setActiveScope(Array<WeakReference<MidiTrack>> tracks, bool forceRebuild)
{
    if (this->selectedTracks == tracks && !forceRebuild)
    {
        return;
    }

    this->selectedTracks = move(tracks);
    this->rebuildAll();
}

void TimeSignaturesAggregator::updateGridDefaultsIfNeeded(int &numerator, int &denominator, float &startBeat) const noexcept
{
    if (this->getSequence()->isEmpty()) // no time signatures at the timeline, and no clips selected
    {
        numerator = this->defaultGridNumerator;
        denominator = this->defaultGridDenominator;
        startBeat = this->defaultGridStart;
    }
}

//===----------------------------------------------------------------------===//
// Listeners management
//===----------------------------------------------------------------------===//

void TimeSignaturesAggregator::addListener(Listener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.add(listener);
}

void TimeSignaturesAggregator::removeListener(Listener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.remove(listener);
}

void TimeSignaturesAggregator::removeAllListeners()
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->listeners.clear();
}

//===----------------------------------------------------------------------===//
// VirtualMidiTrack
//===----------------------------------------------------------------------===//

String TimeSignaturesAggregator::getTrackInstrumentId() const noexcept
{
    // time signatures may emit events for the metronome:
    return App::Workspace().getAudioCore().getMetronomeInstrumentId();
}

MidiSequence *TimeSignaturesAggregator::getSequence() const noexcept
{
    return this->orderedEvents != nullptr ?
        this->orderedEvents.get() : // aggregation mode
        &this->timelineSignatures;  // fallback to timeline time signatures
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void TimeSignaturesAggregator::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature) &&
        !this->isAggregatingTimeSignatureOverrides())
    {
        jassert(event.getSequence() == this->getSequence());
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.isTypeOf(MidiEvent::Type::TimeSignature) &&
        !this->isAggregatingTimeSignatureOverrides())
    {
        jassert(oldEvent.getSequence() == this->getSequence());
        jassert(newEvent.getSequence() == this->getSequence());
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onPostRemoveMidiEvent(MidiSequence *const sequence)
{
    if (sequence == this->getSequence() &&
        !this->isAggregatingTimeSignatureOverrides())
    {
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onAddClip(const Clip &clip)
{
    if (this->isAggregatingTimeSignatureOverrides())
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    if (this->isAggregatingTimeSignatureOverrides() &&
        oldClip.getBeat() != newClip.getBeat())
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onRemoveClip(const Clip &clip)
{
    if (this->isAggregatingTimeSignatureOverrides())
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeTrackProperties(MidiTrack *const track)
{
    if (this->selectedTracks.contains(track))
    {
        // track color might have changed, or its time signature override
        // might have been added, changed or removed:
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeTrackBeatRange(MidiTrack *const track)
{
    if (this->isAggregatingTimeSignatureOverrides() &&
        this->selectedTracks.contains(track))
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->setActiveScope({}, true);
}

void TimeSignaturesAggregator::onRemoveTrack(MidiTrack *const track)
{
    jassert(this->project.getTimeline() != nullptr);
    jassert(track != this->project.getTimeline()->getTimeSignatures());

    if (this->selectedTracks.contains(track))
    {
        this->selectedTracks.removeAllInstancesOf(track);
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeProjectBeatRange(float firstBeat, float lastBeat) {}
void TimeSignaturesAggregator::onChangeViewBeatRange(float firstBeat, float lastBeat) {}

struct SortByTimeSignatureAbsolutePosition final
{
    static int compareElements(const Clip *const first, const Clip *const second)
    {
        const auto *firstTrack = first->getPattern()->getTrack();
        jassert(firstTrack->hasTimeSignatureOverride());

        const auto firstBeat = first->getBeat() +
            firstTrack->getTimeSignatureOverride()->getBeat() +
            firstTrack->getSequence()->getFirstBeat();

        const auto *secondTrack = second->getPattern()->getTrack();
        jassert(secondTrack->hasTimeSignatureOverride());

        const auto secondBeat = second->getBeat() +
            secondTrack->getTimeSignatureOverride()->getBeat() +
            secondTrack->getSequence()->getFirstBeat();

        const float diff = firstBeat - secondBeat;
        return (diff > 0.f) - (diff < 0.f);
    }
};

void TimeSignaturesAggregator::rebuildAll()
{
    if (!this->isAggregatingTimeSignatureOverrides())
    {
        // now it will return timeline's sequence in getSequence():
        this->orderedEvents = nullptr;
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
        return;
    }

    // clear all
    this->orderedEvents = make<TimeSignaturesSequence>(*this, *this->dummyEventDispatcher.get());

    // todo: multiple time signatures per track? now there can be only one

    Array<Clip *> allOrdererClips; // duplicate positions won't be a problem
    for (const auto selectedTrack : this->selectedTracks)
    {
        if (selectedTrack->hasTimeSignatureOverride())
        {
            allOrdererClips.addArray(selectedTrack->getPattern()->getClips());
        }
    }
    jassert(!allOrdererClips.isEmpty());
    static SortByTimeSignatureAbsolutePosition kSort;
    allOrdererClips.sort(kSort);

    // we're adding time signatures in such a way that continuous chunks of clips
    // will only have a time signature at the start of each chunk, instead of
    // time signatures at the start of each clip, which would be a visual noise:
    Meter chunkStartMeter;
    float chunkStartBeat = 0.f;
    // we'll also make sure all those time signatures have unique ids:
    MidiEvent::Id generatedTimeSignatureId = 0;

    for (const auto *clip : allOrdererClips)
    {
        const auto *track = clip->getPattern()->getTrack();
        jassert(track->hasTimeSignatureOverride());

        const auto *timeSignatureOverride = track->getTimeSignatureOverride();
        const auto sequenceFirstBeat = track->getSequence()->getFirstBeat();

        const auto startBeat = clip->getBeat() +
            timeSignatureOverride->getBeat() + sequenceFirstBeat;

        // the very first chunk
        if (!chunkStartMeter.isValid())
        {
            chunkStartBeat = startBeat;
            chunkStartMeter = timeSignatureOverride->getMeter();
            this->orderedEvents->appendUnsafe(timeSignatureOverride->withId(generatedTimeSignatureId).withBeat(startBeat));
            generatedTimeSignatureId++;

            continue;
        }

        if (chunkStartMeter.isEquivalentTo(timeSignatureOverride->getMeter()) &&
            fmodf(startBeat - chunkStartBeat, timeSignatureOverride->getBarLengthInBeats()) == 0.f)
        {
            // this time signature is the "continuation" of the previous one, skip it:
            continue;
        }

        // new chunk starts here, add time signature
        chunkStartBeat = startBeat;
        chunkStartMeter = timeSignatureOverride->getMeter();
        this->orderedEvents->appendUnsafe(timeSignatureOverride->withId(generatedTimeSignatureId).withBeat(startBeat));
        generatedTimeSignatureId++;
    }

    // for now, simple as that: remember the very first one
    // of the aggregated time signatures, and use it as the grid default
    if (!this->orderedEvents->isEmpty())
    {
        const auto *firstTimeSignature = static_cast<const TimeSignatureEvent *>(this->orderedEvents->getUnchecked(0));
        this->defaultGridNumerator = firstTimeSignature->getNumerator();
        this->defaultGridDenominator = firstTimeSignature->getDenominator();
        this->defaultGridStart = firstTimeSignature->getBeat();
    }

    this->listeners.call(&Listener::onTimeSignaturesUpdated);
}

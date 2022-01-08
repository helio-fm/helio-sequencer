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

TimeSignaturesAggregator::TimeSignaturesAggregator(ProjectNode &parentProject,
    MidiSequence &timelineSignatures) :
    project(parentProject),
    timelineSignatures(timelineSignatures)
{
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
    return trackHasTimeSignature(this->selectedTrack); // or the timeline is empty?
}

void TimeSignaturesAggregator::setActiveScope(WeakReference<MidiTrack> newTrack, bool forceRebuildAll)
{
    if (this->selectedTrack == newTrack && !forceRebuildAll)
    {
        return;
    }

    const auto shouldRebuildAll = forceRebuildAll ||
        trackHasTimeSignature(this->selectedTrack) || trackHasTimeSignature(newTrack);

    this->selectedTrack = move(newTrack);

    if (shouldRebuildAll)
    {
        this->rebuildAll();
    }
}

const Array<TimeSignatureEvent> &TimeSignaturesAggregator::getAllOrdered() const noexcept
{
    return this->orderedEvents;
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
// ProjectListener
//===----------------------------------------------------------------------===//

void TimeSignaturesAggregator::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature) &&
        !this->isAggregatingTimeSignatureOverrides())
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(event);
        jassert(timeSignature.getSequence() ==
            this->project.getTimeline()->getTimeSignatures()->getSequence());

        this->orderedEvents.addSorted(timeSignature, timeSignature);
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.isTypeOf(MidiEvent::Type::TimeSignature) &&
        !this->isAggregatingTimeSignatureOverrides())
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(oldEvent);
        jassert(timeSignature.getSequence() ==
            this->project.getTimeline()->getTimeSignatures()->getSequence());

        const auto &newTimeSignature = static_cast<const TimeSignatureEvent &>(newEvent);
        jassert(newTimeSignature.getSequence() ==
            this->project.getTimeline()->getTimeSignatures()->getSequence());

        const auto index = this->orderedEvents.indexOfSorted(timeSignature, timeSignature);
        if (index < 0)
        {
            jassertfalse; // how is that?
            return;
        }

        jassert(timeSignature.getId() == newTimeSignature.getId());
        this->orderedEvents.getReference(index) = newTimeSignature;
        this->orderedEvents.sort(newTimeSignature);
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature) &&
        !this->isAggregatingTimeSignatureOverrides())
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(event);
        jassert(timeSignature.getSequence() ==
            this->project.getTimeline()->getTimeSignatures()->getSequence());

        const auto index = this->orderedEvents.indexOfSorted(timeSignature, timeSignature);
        if (index < 0)
        {
            jassertfalse; // how is that?
            return;
        }

        this->orderedEvents.remove(index);
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
    if (track == this->selectedTrack)
    {
        // track color might have changed, or its time signature override
        // might have been added, changed or removed:
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeTrackBeatRange(MidiTrack *const track)
{
    if (this->isAggregatingTimeSignatureOverrides() &&
        track == this->selectedTrack)
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->setActiveScope(nullptr, true);
}

void TimeSignaturesAggregator::onRemoveTrack(MidiTrack *const track)
{
    jassert(this->project.getTimeline() != nullptr);
    jassert(track != this->project.getTimeline()->getTimeSignatures());

    if (track == this->selectedTrack)
    {
        this->setActiveScope(nullptr, true);
    }
}

void TimeSignaturesAggregator::onChangeProjectBeatRange(float firstBeat, float lastBeat) {}
void TimeSignaturesAggregator::onChangeViewBeatRange(float firstBeat, float lastBeat) {}

void TimeSignaturesAggregator::rebuildAll()
{
    this->orderedEvents.clearQuick();

    if (!this->isAggregatingTimeSignatureOverrides())
    {
        for (const auto *event : this->timelineSignatures)
        {
            const auto *ts = static_cast<const TimeSignatureEvent *>(event);
            this->orderedEvents.add(*ts);
        }

        this->listeners.call(&Listener::onTimeSignaturesUpdated);
        return;
    }

    jassert(this->selectedTrack != nullptr);

    const auto sequenceFirstBeat = this->selectedTrack->getSequence()->getFirstBeat();
    const auto *tsOverride = this->selectedTrack->getTimeSignatureOverride();

    // todo: multiple time signatures per track? now there can be only one

    // we're adding time signatures in such a way that continuous chunks of clips
    // will only have a time signature at the start of each chunk, instead of
    // time signatures at the start of each clip, which would be a visual noise:
    const Clip *chunkStartClip = nullptr;
    // we'll also make sure all those time signatures have unique ids:
    MidiEvent::Id generatedTimeSignatureId = 0;
    for (const auto *clip : this->selectedTrack->getPattern()->getClips())
    {
        const auto startBeat = clip->getBeat() +
            tsOverride->getBeat() + sequenceFirstBeat;

        if (chunkStartClip == nullptr)
        {
            chunkStartClip = clip;
            this->orderedEvents.add(tsOverride->
                withId(generatedTimeSignatureId).withBeat(startBeat));
            generatedTimeSignatureId++;
            continue;
        }

        const auto chunkStartBeat = chunkStartClip->getBeat() +
            tsOverride->getBeat() + sequenceFirstBeat;

        if (fmodf(startBeat - chunkStartBeat, tsOverride->getBarLengthInBeats()) == 0.f)
        {
            // this time signature is the "continuation" of the previous one, skip it:
            continue;
        }

        // new chunk starts here, add time signature
        chunkStartClip = clip;
        this->orderedEvents.add(tsOverride->
            withId(generatedTimeSignatureId).withBeat(startBeat));
        generatedTimeSignatureId++;
    }

    this->listeners.call(&Listener::onTimeSignaturesUpdated);
}

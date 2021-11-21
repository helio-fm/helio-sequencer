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
    TimeSignaturesSequence &timelineSignatures) :
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

void TimeSignaturesAggregator::setActiveScope(WeakReference<MidiTrack> newTrack)
{
    if (this->selectedTrack == newTrack)
    {
        return;
    }

    const auto shouldRebuildAll =
        trackHasTimeSignature(this->selectedTrack) || trackHasTimeSignature(newTrack);

    this->selectedTrack = move(newTrack);

    if (shouldRebuildAll)
    {
        this->rebuildAll();
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
// ProjectListener
//===----------------------------------------------------------------------===//

void TimeSignaturesAggregator::onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (newEvent.isTypeOf(MidiEvent::Type::TimeSignature) &&
        !trackHasTimeSignature(this->selectedTrack)) // fixme that's ugly
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(oldEvent);
        const auto &newTimeSignature = static_cast<const TimeSignatureEvent &>(newEvent);
        // todo what?
        // find, update, sort the list
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature) &&
        !trackHasTimeSignature(this->selectedTrack)) // fixme that's ugly
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(event);
        // todo add sorted,
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(event);
        // todo find, remove, if found, and
        this->listeners.call(&Listener::onTimeSignaturesUpdated);
    }
}

void TimeSignaturesAggregator::onAddClip(const Clip &clip)
{
    if (trackHasTimeSignature(this->selectedTrack))
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    if (trackHasTimeSignature(this->selectedTrack) &&
        oldClip.getBeat() != newClip.getBeat())
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onRemoveClip(const Clip &clip)
{
    if (trackHasTimeSignature(this->selectedTrack))
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeTrackProperties(MidiTrack *const track)
{
    if (track == this->selectedTrack && trackHasTimeSignature(track))
    {
        // track color or time signature might have changed
        // fixme: not rebuilding, just updating the ts?
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onChangeTrackBeatRange(MidiTrack *const track)
{
    if (track == this->selectedTrack && trackHasTimeSignature(track))
    {
        this->rebuildAll();
    }
}

void TimeSignaturesAggregator::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->rebuildAll();
}

void TimeSignaturesAggregator::onRemoveTrack(MidiTrack *const track)
{
    jassert(this->project.getTimeline() != nullptr);

    // what to do when the selected track is removed?
    //if (track == this->selectedTrack)
    //{
    //}

    // what to do when the timeline's ts track is removed?
    // if (track == this->project.getTimeline()->getTimeSignatures())
    //{
    //}
}

void TimeSignaturesAggregator::onChangeProjectBeatRange(float firstBeat, float lastBeat) {}
void TimeSignaturesAggregator::onChangeViewBeatRange(float firstBeat, float lastBeat) {}

void TimeSignaturesAggregator::rebuildAll()
{
    this->orderedEvents.clearQuick();

    if (this->selectedTrack == nullptr ||
        !this->selectedTrack->hasTimeSignatureOverride())
    {
        for (const auto *event : this->timelineSignatures)
        {
            const auto *ts = static_cast<const TimeSignatureEvent *>(event);
            this->orderedEvents.add(*ts);
        }

        this->listeners.call(&Listener::onTimeSignaturesUpdated);
        return;
    }

    const auto sequenceFirstBeat = this->selectedTrack->getSequence()->getFirstBeat();
    const auto &tsOverride = this->selectedTrack->getTimeSignatureOverride();

    // todo: multiple time signatures per track? now there can be only one

    // we're adding time signatures in such a way that continuous chunks of clips
    // will only have a time signature at the start of each chunk, instead of
    // time signatures at the start of each clip, which would be a visual noise
    const Clip *chunkStartClip = nullptr;
    for (const auto *clip : this->selectedTrack->getPattern()->getClips())
    {
        const auto startBeat = tsOverride.getBeat() + sequenceFirstBeat + clip->getBeat();

        if (chunkStartClip == nullptr)
        {
            chunkStartClip = clip;
            this->orderedEvents.add(tsOverride.withBeat(startBeat));
            continue;
        }

        const auto chunkStartBeat = tsOverride.getBeat() + sequenceFirstBeat + chunkStartClip->getBeat();
        if (fmodf(startBeat - chunkStartBeat, tsOverride.getBarLengthInBeats()) == 0.f)
        {
            // this time signature is the "continuation" of the previous one, skip it:
            continue;
        }

        // new chunk starts here, add time signature
        chunkStartClip = clip;
        this->orderedEvents.add(tsOverride.withBeat(startBeat));
    }

    this->listeners.call(&Listener::onTimeSignaturesUpdated);
}

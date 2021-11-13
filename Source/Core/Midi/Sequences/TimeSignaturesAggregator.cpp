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

void TimeSignaturesAggregator::setActiveClips(const Array<Clip> &selectedClips)
{
    // todo rebuild the orderedEvents

    // somehow call the listener events?

    // keep the selected clips for later
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
    if (newEvent.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(oldEvent);
        const auto &newTimeSignature = static_cast<const TimeSignatureEvent &>(newEvent);
        // todo
    }
}

void TimeSignaturesAggregator::onAddMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(event);
        // todo
    }
}

void TimeSignaturesAggregator::onRemoveMidiEvent(const MidiEvent &event)
{
    if (event.isTypeOf(MidiEvent::Type::TimeSignature))
    {
        const auto &timeSignature = static_cast<const TimeSignatureEvent &>(event);
        // todo
    }
}

void TimeSignaturesAggregator::onAddClip(const Clip &clip)
{

}

void TimeSignaturesAggregator::onChangeClip(const Clip &oldClip, const Clip &newClip)
{

}

void TimeSignaturesAggregator::onRemoveClip(const Clip &clip)
{

}

void TimeSignaturesAggregator::onChangeTrackProperties(MidiTrack *const track)
{
    // track's ts might have changed
    // or the timeline might have changed
//     if (this->project.getTimeline() != nullptr &&
//         track == this->project.getTimeline()->getTimeSignatures())
//     {
//         this->repaint();
//     }
}

void TimeSignaturesAggregator::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
//     this->reloadTrackMap();
}

void TimeSignaturesAggregator::onAddTrack(MidiTrack *const track)
{
//     if (this->project.getTimeline() != nullptr &&
//         track == this->project.getTimeline()->getTimeSignatures())
//     {
//         if (track->getSequence()->size() > 0)
//         {
//             this->reloadTrackMap();
//         }
//     }
}

void TimeSignaturesAggregator::onRemoveTrack(MidiTrack *const track)
{
//     if (this->project.getTimeline() != nullptr &&
//         track == this->project.getTimeline()->getTimeSignatures())
//     {
//         for (int i = 0; i < track->getSequence()->size(); ++i)
//         {
//             const auto &timeSignature =
//                 static_cast<const TimeSignatureEvent &>(*track->getSequence()->getUnchecked(i));
//             // todo
//         }
//     }
}

void TimeSignaturesAggregator::onChangeProjectBeatRange(float firstBeat, float lastBeat) {}
void TimeSignaturesAggregator::onChangeViewBeatRange(float firstBeat, float lastBeat) {}

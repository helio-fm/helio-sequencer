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
#include "CommandPaletteTimelineEvents.h"

#include "Transport.h"
#include "ProjectTimeline.h"
#include "HybridRoll.h"
#include "MidiSequence.h"
#include "AnnotationEvent.h"
#include "TimeSignatureEvent.h"
#include "KeySignatureEvent.h"

CommandPaletteTimelineEvents::CommandPaletteTimelineEvents(const ProjectNode &project) :
    project(project) {}

const CommandPaletteActionsProvider::Actions &CommandPaletteTimelineEvents::getActions() const
{
    // TODO rebuild flag
    if (!this->timelineEvents.isEmpty())
    {
        return this->timelineEvents;
    }

    double outTimeMs = 0.0;
    double outTempo = 0.0;

    const auto *timeline = this->project.getTimeline();

    const auto *annotationsSequence = timeline->getAnnotations()->getSequence();
    for (int i = 0; i < annotationsSequence->size(); ++i)
    {
        const auto *annotation = dynamic_cast<AnnotationEvent *>(annotationsSequence->getUnchecked(i));
        jassert(annotation != nullptr);

        const auto *roll = this->project.getLastFocusedRoll();
        jassert(roll != nullptr);

        const double seekPos = roll->getTransportPositionByBeat(annotation->getBeat());
        this->project.getTransport().calcTimeAndTempoAt(seekPos, outTimeMs, outTempo);
        const auto action = [this, i](TextEditor &ed)
        {
            auto *roll = this->project.getLastFocusedRoll();
            const auto *timeline = this->project.getTimeline();
            const auto *annotations = timeline->getAnnotations()->getSequence();
            const auto *annotation = dynamic_cast<AnnotationEvent *>(annotations->getUnchecked(i));
            jassert(annotation != nullptr);

            const double seekPosition = roll->getTransportPositionByBeat(annotation->getBeat());
            this->project.getTransport().seekToPosition(seekPosition);
            roll->scrollToSeekPosition();
            return true;
        };

        this->timelineEvents.add(new CommandPaletteAction(annotation->getDescription(),
            Transport::getTimeString(outTimeMs), annotation->getTrackColour(),
            action, float(outTimeMs)));
    }

    const auto *ksSequence = timeline->getKeySignatures()->getSequence();
    for (int i = 0; i < ksSequence->size(); ++i)
    {
        const auto *event = dynamic_cast<KeySignatureEvent *>(ksSequence->getUnchecked(i));
        jassert(event != nullptr);

        const auto *roll = this->project.getLastFocusedRoll();
        jassert(roll != nullptr);

        const double seekPos = roll->getTransportPositionByBeat(event->getBeat());
        this->project.getTransport().calcTimeAndTempoAt(seekPos, outTimeMs, outTempo);
        const auto action = [this, i](TextEditor &ed)
        {
            auto *roll = this->project.getLastFocusedRoll();
            const auto *timeline = this->project.getTimeline();
            const auto *ksSequence = timeline->getKeySignatures()->getSequence();
            const auto *event = dynamic_cast<KeySignatureEvent *>(ksSequence->getUnchecked(i));
            jassert(event != nullptr);

            const double seekPosition = roll->getTransportPositionByBeat(event->getBeat());
            this->project.getTransport().seekToPosition(seekPosition);
            roll->scrollToSeekPosition();
            return true;
        };

        this->timelineEvents.add(new CommandPaletteAction(event->toString(),
            Transport::getTimeString(outTimeMs), event->getTrackColour(),
            action, float(outTimeMs)));
    }

    const auto *tsSequence = timeline->getTimeSignatures()->getSequence();
    for (int i = 0; i < tsSequence->size(); ++i)
    {
        const auto *event = dynamic_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(i));
        jassert(event != nullptr);

        const auto *roll = this->project.getLastFocusedRoll();
        jassert(roll != nullptr);

        const double seekPos = roll->getTransportPositionByBeat(event->getBeat());
        this->project.getTransport().calcTimeAndTempoAt(seekPos, outTimeMs, outTempo);
        const auto action = [this, i](TextEditor &ed)
        {
            auto *roll = this->project.getLastFocusedRoll();
            const auto *timeline = this->project.getTimeline();
            const auto *tsSequence = timeline->getTimeSignatures()->getSequence();
            const auto *event = dynamic_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(i));
            jassert(event != nullptr);

            const double seekPosition = roll->getTransportPositionByBeat(event->getBeat());
            this->project.getTransport().seekToPosition(seekPosition);
            roll->scrollToSeekPosition();
            return true;
        };

        this->timelineEvents.add(new CommandPaletteAction(event->toString(),
            Transport::getTimeString(outTimeMs), event->getTrackColour(),
            action, float(outTimeMs)));
    }

    return this->timelineEvents;
}

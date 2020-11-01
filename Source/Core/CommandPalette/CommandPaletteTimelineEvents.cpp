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
#include "ProjectMetadata.h"
#include "Temperament.h"
#include "HybridRoll.h"
#include "MidiSequence.h"
#include "AnnotationEvent.h"
#include "TimeSignatureEvent.h"
#include "KeySignatureEvent.h"
#include "PianoTrackNode.h"
#include "PianoSequence.h"
#include "Pattern.h"
#include "Clip.h"

CommandPaletteTimelineEvents::CommandPaletteTimelineEvents(ProjectNode &project) :
    project(project)
{
    this->project.addListener(this);
}

CommandPaletteTimelineEvents::~CommandPaletteTimelineEvents()
{
    this->project.removeListener(this);
}

const CommandPaletteActionsProvider::Actions &CommandPaletteTimelineEvents::getActions() const
{
    if (!this->keySignatureActionsOutdated && !this->timeSignatureActionsOutdated &&
        !this->annotationActionsOutdated && !this->clipActionsOutdated)
    {
        return this->allActions;
    }

    const auto *timeline = this->project.getTimeline();
    
    if (this->annotationActionsOutdated)
    {
        this->annotationActionsCache.clearQuick();

        const auto *annotationsSequence = timeline->getAnnotations()->getSequence();
        for (int i = 0; i < annotationsSequence->size(); ++i)
        {
            const auto *annotation = dynamic_cast<AnnotationEvent *>(annotationsSequence->getUnchecked(i));
            jassert(annotation != nullptr);

            const auto startTimeMs = this->project.getTransport().findTimeAt(annotation->getBeat());
            const auto action = [this, i](TextEditor &ed)
            {
                auto *roll = this->project.getLastFocusedRoll();
                jassert(roll != nullptr);

                const auto *timeline = this->project.getTimeline();
                const auto *annotations = timeline->getAnnotations()->getSequence();
                const auto *annotation = dynamic_cast<AnnotationEvent *>(annotations->getUnchecked(i));
                jassert(annotation != nullptr);

                jassert(!this->project.getTransport().isPlaying());
                this->project.getTransport().seekToBeat(annotation->getBeat());
                roll->scrollToSeekPosition();
                return true;
            };

            this->annotationActionsCache.add(CommandPaletteAction::action(annotation->getDescription(),
                Transport::getTimeString(startTimeMs), float(startTimeMs))->
                withColour(annotation->getTrackColour())->
                withCallback(action));
        }

        this->annotationActionsOutdated = false;
    }

    if (this->keySignatureActionsOutdated)
    {
        this->keySignatureActionsCache.clearQuick();
        const auto &keyNames = this->project.getProjectInfo()->getTemperament()->getPeriod();

        const auto *ksSequence = timeline->getKeySignatures()->getSequence();
        for (int i = 0; i < ksSequence->size(); ++i)
        {
            const auto *event = dynamic_cast<KeySignatureEvent *>(ksSequence->getUnchecked(i));
            jassert(event != nullptr);

            const auto startTimeMs = this->project.getTransport().findTimeAt(event->getBeat());
            const auto action = [this, i](TextEditor &ed)
            {
                auto *roll = this->project.getLastFocusedRoll();
                jassert(roll != nullptr);

                const auto *timeline = this->project.getTimeline();
                const auto *ksSequence = timeline->getKeySignatures()->getSequence();
                const auto *event = dynamic_cast<KeySignatureEvent *>(ksSequence->getUnchecked(i));
                jassert(event != nullptr);

                jassert(!this->project.getTransport().isPlaying());
                this->project.getTransport().seekToBeat(event->getBeat());
                roll->scrollToSeekPosition();
                return true;
            };

            this->keySignatureActionsCache.add(CommandPaletteAction::action(event->toString(keyNames),
                Transport::getTimeString(startTimeMs), float(startTimeMs))->
                withColour(event->getTrackColour())->
                withCallback(action));
        }

        this->keySignatureActionsOutdated = false;
    }

    if (this->timeSignatureActionsOutdated)
    {
        this->timeSignatureActionsCache.clearQuick();

        const auto *tsSequence = timeline->getTimeSignatures()->getSequence();
        for (int i = 0; i < tsSequence->size(); ++i)
        {
            const auto *event = dynamic_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(i));
            jassert(event != nullptr);

            const auto startTimeMs = this->project.getTransport().findTimeAt(event->getBeat());
            const auto action = [this, i](TextEditor &ed)
            {
                auto *roll = this->project.getLastFocusedRoll();
                jassert(roll != nullptr);

                const auto *timeline = this->project.getTimeline();
                const auto *tsSequence = timeline->getTimeSignatures()->getSequence();
                const auto *event = dynamic_cast<TimeSignatureEvent *>(tsSequence->getUnchecked(i));
                jassert(event != nullptr);

                jassert(!this->project.getTransport().isPlaying());
                this->project.getTransport().seekToBeat(event->getBeat());
                roll->scrollToSeekPosition();
                return true;
            };

            this->timeSignatureActionsCache.add(CommandPaletteAction::action(event->toString(),
                Transport::getTimeString(startTimeMs), float(startTimeMs))->
                withColour(event->getTrackColour())->
                withCallback(action));
        }

        this->timeSignatureActionsOutdated = false;
    }

    if (this->clipActionsOutdated)
    {
        this->clipActionsCache.clearQuick();

        for (auto *pianoTrackNode : this->project.findChildrenOfType<PianoTrackNode>())
        {
            const auto *sequence = pianoTrackNode->getSequence();
            for (const auto *clip : pianoTrackNode->getPattern()->getClips())
            {
                const auto seekStart = sequence->getFirstBeat() + clip->getBeat();
                const auto seekEnd = sequence->getLastBeat() + clip->getBeat();
                const auto startTimeMs = this->project.getTransport().findTimeAt(seekStart);
                const auto endTimeMs = this->project.getTransport().findTimeAt(seekEnd);
                const auto timeString = Transport::getTimeString(startTimeMs) + " - " + Transport::getTimeString(endTimeMs);

                const auto action = [this, pianoTrackNode, clip](TextEditor &ed)
                {
                    this->project.setEditableScope(pianoTrackNode, *clip, true);
                    return true;
                };

                // all clips are to be listed at the bottom, thus + 10000 sec offset:
                static const float orderOffset = 10000000.f;
                this->clipActionsCache.add(CommandPaletteAction::action(pianoTrackNode->getTrackName(),
                    timeString, float(startTimeMs + orderOffset))->
                    withColour(pianoTrackNode->getTrackColour())->
                    withCallback(action));
            }
        }

        this->clipActionsOutdated = false;
    }

    this->allActions.clearQuick();
    this->allActions.addArray(this->annotationActionsCache);
    this->allActions.addArray(this->keySignatureActionsCache);
    this->allActions.addArray(this->timeSignatureActionsCache);
    this->allActions.addArray(this->clipActionsCache);
    return this->allActions;
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void CommandPaletteTimelineEvents::onAddMidiEvent(const MidiEvent &event)
{
    switch (event.getType())
    {
    case MidiEvent::Type::Annotation:
        this->annotationActionsOutdated = true;
        break;
    case MidiEvent::Type::KeySignature:
        this->keySignatureActionsOutdated = true;
        break;
    case MidiEvent::Type::TimeSignature:
        this->timeSignatureActionsOutdated = true;
        break;
    case MidiEvent::Type::Note:
        // if any added/changed/removed note changes its track range, onChangeTrackBeatRange() will be called
    case MidiEvent::Type::Auto:
    default:
        break;
    }
}

void CommandPaletteTimelineEvents::onChangeMidiEvent(const MidiEvent &event, const MidiEvent &newEvent)
{
    this->onAddMidiEvent(event); // same
}

void CommandPaletteTimelineEvents::onRemoveMidiEvent(const MidiEvent &event)
{
    this->onAddMidiEvent(event); // same
}

void CommandPaletteTimelineEvents::onAddClip(const Clip &clip)
{
    this->clipActionsOutdated = true;
}

void CommandPaletteTimelineEvents::onChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->clipActionsOutdated = true;
}

void CommandPaletteTimelineEvents::onRemoveClip(const Clip &clip)
{
    this->clipActionsOutdated = true;
}

void CommandPaletteTimelineEvents::onAddTrack(MidiTrack *const track)
{
    this->clipActionsOutdated = true;
}

void CommandPaletteTimelineEvents::onRemoveTrack(MidiTrack *const track)
{
    this->clipActionsOutdated = true;
}

void CommandPaletteTimelineEvents::onChangeTrackProperties(MidiTrack *const track)
{
    this->clipActionsOutdated = true; // the name might have changed
}

void CommandPaletteTimelineEvents::onChangeTrackBeatRange(MidiTrack *const track)
{
    this->clipActionsOutdated = true;
}

void CommandPaletteTimelineEvents::onReloadProjectContent(const Array<MidiTrack *> &tracks,
    const ProjectMetadata *meta)
{
    this->annotationActionsOutdated = true;
    this->keySignatureActionsOutdated = true;
    this->timeSignatureActionsOutdated = true;
    this->clipActionsOutdated = true;
}

void CommandPaletteTimelineEvents::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    // time signatures have changed, reset all
    this->annotationActionsOutdated = true;
    this->keySignatureActionsOutdated = true;
    this->timeSignatureActionsOutdated = true;
    this->clipActionsOutdated = true;
}

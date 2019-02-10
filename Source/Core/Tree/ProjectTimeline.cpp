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
#include "ProjectTimeline.h"
#include "AnnotationsSequence.h"
#include "TimeSignaturesSequence.h"
#include "KeySignaturesSequence.h"
#include "ProjectNode.h"
#include "Pattern.h"
#include "Icons.h"

// A simple wrappers around the sequences
// We don't need any patterns here
class AnnotationsTrack final : public EmptyMidiTrack
{
public:

    AnnotationsTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    const String &getTrackId() const noexcept override
    { return this->timeline.annotationsTrackId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.annotationsSequence; }

    ProjectTimeline &timeline;
};

class TimeSignaturesTrack final : public EmptyMidiTrack
{
public:

    TimeSignaturesTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    const String &getTrackId() const noexcept override
    { return this->timeline.timeSignaturesTrackId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.timeSignaturesSequence; }

    ProjectTimeline &timeline;
};

class KeySignaturesTrack final : public EmptyMidiTrack
{
public:

    KeySignaturesTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    const String &getTrackId() const noexcept override
    { return this->timeline.keySignaturesTrackId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.keySignaturesSequence; }

    ProjectTimeline &timeline;
};

ProjectTimeline::ProjectTimeline(ProjectNode &parentProject, String trackName) :
    project(parentProject),
    annotationsTrackId(Uuid().toString()),
    timeSignaturesTrackId(Uuid().toString()),
    keySignaturesTrackId(Uuid().toString())
{
    this->annotationsTrack = new AnnotationsTrack(*this);
    this->annotationsSequence = new AnnotationsSequence(*this->annotationsTrack, *this);

    this->timeSignaturesTrack = new TimeSignaturesTrack(*this);
    this->timeSignaturesSequence = new TimeSignaturesSequence(*this->timeSignaturesTrack, *this);

    this->keySignaturesTrack = new KeySignaturesTrack(*this);
    this->keySignaturesSequence = new KeySignaturesSequence(*this->keySignaturesTrack, *this);

    using namespace Serialization::VCS;
    this->vcsDiffLogic = new VCS::ProjectTimelineDiffLogic(*this);
    this->deltas.add(new VCS::Delta({}, ProjectTimelineDeltas::annotationsAdded));
    this->deltas.add(new VCS::Delta({}, ProjectTimelineDeltas::keySignaturesAdded));
    this->deltas.add(new VCS::Delta({}, ProjectTimelineDeltas::timeSignaturesAdded));

    this->project.broadcastAddTrack(this->annotationsTrack);
    this->project.broadcastAddTrack(this->keySignaturesTrack);
    this->project.broadcastAddTrack(this->timeSignaturesTrack);
}

ProjectTimeline::~ProjectTimeline()
{
    this->project.broadcastRemoveTrack(this->timeSignaturesTrack);
    this->project.broadcastRemoveTrack(this->keySignaturesTrack);
    this->project.broadcastRemoveTrack(this->annotationsTrack);
}

MidiTrack *ProjectTimeline::getAnnotations() const noexcept
{
    return this->annotationsTrack;
}

MidiTrack *ProjectTimeline::getTimeSignatures() const noexcept
{
    return this->timeSignaturesTrack;
}

MidiTrack *ProjectTimeline::getKeySignatures() const noexcept
{
    return this->keySignaturesTrack;
}

//===----------------------------------------------------------------------===//
// Navigation helpers
//===----------------------------------------------------------------------===//

static float findNextTrackAnchor(MidiTrack *track, float beat)
{
    for (int i = 0; i < track->getSequence()->size(); ++i)
    {
        const auto *current = track->getSequence()->getUnchecked(i);
        if (current->getBeat() > beat)
        {
            return current->getBeat();
        }
    }

    return FLT_MAX;
}

static float findPreviousTrackAnchor(MidiTrack *track, float beat)
{
    for (int i = track->getSequence()->size(); i --> 0                        ;)
    {
        const auto *current = track->getSequence()->getUnchecked(i);
        if (current->getBeat() < beat)
        {
            return current->getBeat();
        }
    }

    return -FLT_MAX;
}

// finds the nearest timeline event, like key or time signature, or annotation
float ProjectTimeline::findNextAnchorBeat(float beat) const
{
    const auto keyEvent = findNextTrackAnchor(this->keySignaturesTrack, beat);
    const auto timeEvent = findNextTrackAnchor(this->timeSignaturesTrack, beat);
    const auto annotation = findNextTrackAnchor(this->annotationsTrack, beat);
    return jmin(keyEvent, timeEvent, annotation);
}

float ProjectTimeline::findPreviousAnchorBeat(float beat) const
{
    const auto keyEvent = findPreviousTrackAnchor(this->keySignaturesTrack, beat);
    const auto timeEvent = findPreviousTrackAnchor(this->timeSignaturesTrack, beat);
    const auto annotation = findPreviousTrackAnchor(this->annotationsTrack, beat);
    return jmax(keyEvent, timeEvent, annotation);
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String ProjectTimeline::getVCSName() const
{
    return "vcs::items::timeline";
}

int ProjectTimeline::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *ProjectTimeline::getDelta(int index) const
{
    using namespace Serialization::VCS;
    if (this->deltas[index]->hasType(ProjectTimelineDeltas::annotationsAdded))
    {
        const int numEvents = this->annotationsSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} annotations", numEvents));
    }
    else if (this->deltas[index]->hasType(ProjectTimelineDeltas::timeSignaturesAdded))
    {
        const int numEvents = this->timeSignaturesSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} time signatures", numEvents));
    }
    else if (this->deltas[index]->hasType(ProjectTimelineDeltas::keySignaturesAdded))
    {
        const int numEvents = this->keySignaturesSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} key signatures", numEvents));
    }

    return this->deltas[index];
}

ValueTree ProjectTimeline::getDeltaData(int deltaIndex) const
{
    using namespace Serialization::VCS;
    if (this->deltas[deltaIndex]->hasType(ProjectTimelineDeltas::annotationsAdded))
    {
        return this->serializeAnnotationsDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(ProjectTimelineDeltas::timeSignaturesAdded))
    {
        return this->serializeTimeSignaturesDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(ProjectTimelineDeltas::keySignaturesAdded))
    {
        return this->serializeKeySignaturesDelta();
    }

    jassertfalse;
    return {};
}

VCS::DiffLogic *ProjectTimeline::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void ProjectTimeline::resetStateTo(const VCS::TrackedItem &newState)
{
    using namespace Serialization::VCS;
    bool annotationsChanged = false;
    bool timeSignaturesChanged = false;
    bool keySignaturesChanged = false;

    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        const auto newDeltaData(newState.getDeltaData(i));
        
        if (newDelta->hasType(ProjectTimelineDeltas::annotationsAdded))
        {
            this->resetAnnotationsDelta(newDeltaData);
            annotationsChanged = true;
        }
        else if (newDelta->hasType(ProjectTimelineDeltas::timeSignaturesAdded))
        {
            this->resetTimeSignaturesDelta(newDeltaData);
            timeSignaturesChanged = true;
        }
        else if (newDelta->hasType(ProjectTimelineDeltas::keySignaturesAdded))
        {
            this->resetKeySignaturesDelta(newDeltaData);
            keySignaturesChanged = true;
        }
    }
}


//===----------------------------------------------------------------------===//
// ProjectEventDispatcher
//===----------------------------------------------------------------------===//

void ProjectTimeline::dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    this->project.broadcastChangeEvent(oldEvent, newEvent);
}

void ProjectTimeline::dispatchAddEvent(const MidiEvent &event)
{
    this->project.broadcastAddEvent(event);
}

void ProjectTimeline::dispatchRemoveEvent(const MidiEvent &event)
{
    this->project.broadcastRemoveEvent(event);
}

void ProjectTimeline::dispatchPostRemoveEvent(MidiSequence *const layer)
{
    this->project.broadcastPostRemoveEvent(layer);
}

void ProjectTimeline::dispatchChangeTrackProperties(MidiTrack *const track)
{
    this->project.broadcastChangeTrackProperties(track);
}

void ProjectTimeline::dispatchChangeProjectBeatRange()
{
    this->project.broadcastChangeProjectBeatRange();

}

// Timeline sequences are the case where there are no patterns and clips
// So just leave this empty:

void ProjectTimeline::dispatchAddClip(const Clip &clip) {}
void ProjectTimeline::dispatchChangeClip(const Clip &oldClip, const Clip &newClip) {}
void ProjectTimeline::dispatchRemoveClip(const Clip &clip) {}
void ProjectTimeline::dispatchPostRemoveClip(Pattern *const pattern) {}

ProjectNode *ProjectTimeline::getProject() const noexcept
{
    return &this->project;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void ProjectTimeline::reset()
{
    this->annotationsSequence->reset();
    this->keySignaturesSequence->reset();
    this->timeSignaturesSequence->reset();
}

ValueTree ProjectTimeline::serialize() const
{
    ValueTree tree(this->vcsDiffLogic->getType());

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::Core::annotationsTrackId,
        this->annotationsTrackId, nullptr);

    tree.setProperty(Serialization::Core::keySignaturesTrackId,
        this->keySignaturesTrackId, nullptr);

    tree.setProperty(Serialization::Core::timeSignaturesTrackId,
        this->timeSignaturesTrackId, nullptr);

    tree.appendChild(this->annotationsSequence->serialize(), nullptr);
    tree.appendChild(this->keySignaturesSequence->serialize(), nullptr);
    tree.appendChild(this->timeSignaturesSequence->serialize(), nullptr);

    return tree;
}

void ProjectTimeline::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto root = tree.hasType(this->vcsDiffLogic->getType()) ?
        tree : tree.getChildWithName(this->vcsDiffLogic->getType());
    
    if (!root.isValid())
    {
        return;
    }

    this->deserializeVCSUuid(root);

    this->annotationsTrackId =
        root.getProperty(Serialization::Core::annotationsTrackId,
            this->annotationsTrackId);

    this->keySignaturesTrackId =
        root.getProperty(Serialization::Core::keySignaturesTrackId,
            this->keySignaturesTrackId);

    this->timeSignaturesTrackId =
        root.getProperty(Serialization::Core::timeSignaturesTrackId,
            this->timeSignaturesTrackId);

    forEachValueTreeChildWithType(root, e, Serialization::Midi::annotations)
    {
        this->annotationsSequence->deserialize(e);
    }

    forEachValueTreeChildWithType(root, e, Serialization::Midi::keySignatures)
    {
        this->keySignaturesSequence->deserialize(e);
    }

    forEachValueTreeChildWithType(root, e, Serialization::Midi::timeSignatures)
    {
        this->timeSignaturesSequence->deserialize(e);
    }

    // Debug::
    //TimeSignatureEvent e(this->timeSignaturesSequence, 0.f, 9, 16);
    //(static_cast<TimeSignaturesSequence *>(this->timeSignaturesSequence.get()))->insert(e, false);
    //KeySignatureEvent e(this->keySignaturesSequence, 0.f);
    //(static_cast<KeySignaturesSequence *>(this->keySignaturesSequence.get()))->insert(e, false);
}

//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

ValueTree ProjectTimeline::serializeAnnotationsDelta() const
{
    ValueTree tree(Serialization::VCS::ProjectTimelineDeltas::annotationsAdded);

    for (int i = 0; i < this->annotationsSequence->size(); ++i)
    {
        const MidiEvent *event = this->annotationsSequence->getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }

    return tree;
}

void ProjectTimeline::resetAnnotationsDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectTimelineDeltas::annotationsAdded));
    this->annotationsSequence->reset();

    forEachValueTreeChildWithType(state, e, Serialization::Midi::annotation)
    {
        this->annotationsSequence->checkoutEvent<AnnotationEvent>(e);
    }

    this->annotationsSequence->updateBeatRange(false);
}

ValueTree ProjectTimeline::serializeTimeSignaturesDelta() const
{
    ValueTree tree(Serialization::VCS::ProjectTimelineDeltas::timeSignaturesAdded);

    for (int i = 0; i < this->timeSignaturesSequence->size(); ++i)
    {
        const MidiEvent *event = this->timeSignaturesSequence->getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }
    
    return tree;
}

void ProjectTimeline::resetTimeSignaturesDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectTimelineDeltas::timeSignaturesAdded));
    this->timeSignaturesSequence->reset();
    
    forEachValueTreeChildWithType(state, e, Serialization::Midi::timeSignature)
    {
        this->timeSignaturesSequence->checkoutEvent<TimeSignatureEvent>(e);
    }

    this->timeSignaturesSequence->updateBeatRange(false);
}

ValueTree ProjectTimeline::serializeKeySignaturesDelta() const
{
    ValueTree tree(Serialization::VCS::ProjectTimelineDeltas::keySignaturesAdded);

    for (int i = 0; i < this->keySignaturesSequence->size(); ++i)
    {
        const MidiEvent *event = this->keySignaturesSequence->getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }

    return tree;
}

void ProjectTimeline::resetKeySignaturesDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::ProjectTimelineDeltas::keySignaturesAdded));
    this->keySignaturesSequence->reset();

    forEachValueTreeChildWithType(state, e, Serialization::Midi::keySignature)
    {
        this->keySignaturesSequence->checkoutEvent<KeySignatureEvent>(e);
    }

    this->keySignaturesSequence->updateBeatRange(false);
}

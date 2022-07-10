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
#include "Workspace.h"
#include "AudioCore.h"

// A simple wrappers around the sequences
// We don't need any patterns here
class AnnotationsTrack final : public VirtualMidiTrack
{
public:

    AnnotationsTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    const String &getTrackId() const noexcept override
    { return this->timeline.annotationsTrackId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.annotationsSequence.get(); }

    Colour getTrackColour() const noexcept override
    { return findDefaultColour(Label::textColourId); }

    ProjectTimeline &timeline;
};

class TimeSignaturesTrack final : public VirtualMidiTrack
{
public:

    TimeSignaturesTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    const String &getTrackId() const noexcept override
    { return this->timeline.timeSignaturesTrackId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.timeSignaturesSequence.get(); }

    Colour getTrackColour() const noexcept override
    { return findDefaultColour(Label::textColourId); }

    String getTrackInstrumentId() const noexcept
    {
        // time signatures may emit events for the metronome:
        return App::Workspace().getAudioCore().getMetronomeInstrumentId();
    }

    ProjectTimeline &timeline;
};

class KeySignaturesTrack final : public VirtualMidiTrack
{
public:

    KeySignaturesTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    const String &getTrackId() const noexcept override
    { return this->timeline.keySignaturesTrackId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.keySignaturesSequence.get(); }

    Colour getTrackColour() const noexcept override
    { return findDefaultColour(Label::textColourId); }

    ProjectTimeline &timeline;
};

ProjectTimeline::ProjectTimeline(ProjectNode &parentProject, String trackName) :
    project(parentProject)
{
    this->annotationsTrack = make<AnnotationsTrack>(*this);
    this->annotationsSequence = make<AnnotationsSequence>(*this->annotationsTrack, *this);

    this->timeSignaturesTrack = make<TimeSignaturesTrack>(*this);
    this->timeSignaturesSequence = make<TimeSignaturesSequence>(*this->timeSignaturesTrack, *this);

    this->keySignaturesTrack = make<KeySignaturesTrack>(*this);
    this->keySignaturesSequence = make<KeySignaturesSequence>(*this->keySignaturesTrack, *this);

    this->timeSignaturesAggregator = make<TimeSignaturesAggregator>(this->project, *this->timeSignaturesSequence);

    using namespace Serialization::VCS;
    this->vcsDiffLogic = make<VCS::ProjectTimelineDiffLogic>(*this);
    this->deltas.add(new VCS::Delta({}, AnnotationDeltas::annotationsAdded));
    this->deltas.add(new VCS::Delta({}, KeySignatureDeltas::keySignaturesAdded));
    this->deltas.add(new VCS::Delta({}, TimeSignatureDeltas::timeSignaturesAdded));

    this->project.broadcastAddTrack(this->annotationsTrack.get());
    this->project.broadcastAddTrack(this->keySignaturesTrack.get());
    this->project.broadcastAddTrack(this->timeSignaturesTrack.get());
}

ProjectTimeline::~ProjectTimeline()
{
    this->project.broadcastRemoveTrack(this->timeSignaturesTrack.get());
    this->project.broadcastRemoveTrack(this->keySignaturesTrack.get());
    this->project.broadcastRemoveTrack(this->annotationsTrack.get());
}

MidiTrack *ProjectTimeline::getAnnotations() const noexcept
{
    return this->annotationsTrack.get();
}

MidiTrack *ProjectTimeline::getKeySignatures() const noexcept
{
    return this->keySignaturesTrack.get();
}

MidiTrack *ProjectTimeline::getTimeSignatures() const noexcept
{
    return this->timeSignaturesTrack.get();
}

TimeSignaturesAggregator *ProjectTimeline::getTimeSignaturesAggregator() const noexcept
{
    return this->timeSignaturesAggregator.get();
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
    const auto keyEvent = findNextTrackAnchor(this->keySignaturesTrack.get(), beat);
    const auto timeEvent = findNextTrackAnchor(this->timeSignaturesTrack.get(), beat);
    const auto annotation = findNextTrackAnchor(this->annotationsTrack.get(), beat);
    return jmin(keyEvent, timeEvent, annotation);
}

float ProjectTimeline::findPreviousAnchorBeat(float beat) const
{
    const auto keyEvent = findPreviousTrackAnchor(this->keySignaturesTrack.get(), beat);
    const auto timeEvent = findPreviousTrackAnchor(this->timeSignaturesTrack.get(), beat);
    const auto annotation = findPreviousTrackAnchor(this->annotationsTrack.get(), beat);
    return jmax(keyEvent, timeEvent, annotation);
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String ProjectTimeline::getVCSName() const
{
    return TRANS(I18n::VCS::projectTimeline);
}

int ProjectTimeline::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *ProjectTimeline::getDelta(int index) const
{
    using namespace Serialization::VCS;
    if (this->deltas[index]->hasType(AnnotationDeltas::annotationsAdded))
    {
        const int numEvents = this->annotationsSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} annotations", numEvents));
    }
    else if (this->deltas[index]->hasType(TimeSignatureDeltas::timeSignaturesAdded))
    {
        const int numEvents = this->timeSignaturesSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} time signatures", numEvents));
    }
    else if (this->deltas[index]->hasType(KeySignatureDeltas::keySignaturesAdded))
    {
        const int numEvents = this->keySignaturesSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} key signatures", numEvents));
    }

    return this->deltas[index];
}

SerializedData ProjectTimeline::getDeltaData(int deltaIndex) const
{
    using namespace Serialization::VCS;
    if (this->deltas[deltaIndex]->hasType(AnnotationDeltas::annotationsAdded))
    {
        return this->serializeAnnotationsDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(TimeSignatureDeltas::timeSignaturesAdded))
    {
        return this->serializeTimeSignaturesDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(KeySignatureDeltas::keySignaturesAdded))
    {
        return this->serializeKeySignaturesDelta();
    }

    jassertfalse;
    return {};
}

VCS::DiffLogic *ProjectTimeline::getDiffLogic() const
{
    return this->vcsDiffLogic.get();
}

void ProjectTimeline::resetStateTo(const VCS::TrackedItem &newState)
{
    using namespace Serialization::VCS;

    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        const auto newDeltaData(newState.getDeltaData(i));
        
        if (newDelta->hasType(AnnotationDeltas::annotationsAdded))
        {
            this->resetAnnotationsDelta(newDeltaData);
        }
        else if (newDelta->hasType(TimeSignatureDeltas::timeSignaturesAdded))
        {
            this->resetTimeSignaturesDelta(newDeltaData);
        }
        else if (newDelta->hasType(KeySignatureDeltas::keySignaturesAdded))
        {
            this->resetKeySignaturesDelta(newDeltaData);
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

void ProjectTimeline::dispatchChangeTrackProperties()
{
    jassertfalse; // should never be called
}

void ProjectTimeline::dispatchChangeTrackBeatRange()
{
    this->project.broadcastChangeProjectBeatRange();
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

SerializedData ProjectTimeline::serialize() const
{
    SerializedData tree(this->vcsDiffLogic->getType());

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::Core::annotationsTrackId,
        this->annotationsTrackId);

    tree.setProperty(Serialization::Core::keySignaturesTrackId,
        this->keySignaturesTrackId);

    tree.setProperty(Serialization::Core::timeSignaturesTrackId,
        this->timeSignaturesTrackId);

    tree.appendChild(this->annotationsSequence->serialize());
    tree.appendChild(this->keySignaturesSequence->serialize());
    tree.appendChild(this->timeSignaturesSequence->serialize());

    return tree;
}

void ProjectTimeline::deserialize(const SerializedData &data)
{
    this->reset();
    
    const auto root = data.hasType(this->vcsDiffLogic->getType()) ?
        data : data.getChildWithName(this->vcsDiffLogic->getType());
    
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

    forEachChildWithType(root, e, Serialization::Midi::annotations)
    {
        this->annotationsSequence->deserialize(e);
    }

    forEachChildWithType(root, e, Serialization::Midi::keySignatures)
    {
        this->keySignaturesSequence->deserialize(e);
    }

    forEachChildWithType(root, e, Serialization::Midi::timeSignatures)
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

SerializedData ProjectTimeline::serializeAnnotationsDelta() const
{
    SerializedData tree(Serialization::VCS::AnnotationDeltas::annotationsAdded);

    for (int i = 0; i < this->annotationsSequence->size(); ++i)
    {
        const MidiEvent *event = this->annotationsSequence->getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}

void ProjectTimeline::resetAnnotationsDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::AnnotationDeltas::annotationsAdded));
    this->annotationsSequence->reset();

    forEachChildWithType(state, e, Serialization::Midi::annotation)
    {
        this->annotationsSequence->checkoutEvent<AnnotationEvent>(e);
    }

    this->annotationsSequence->updateBeatRange(false);
}

SerializedData ProjectTimeline::serializeTimeSignaturesDelta() const
{
    SerializedData tree(Serialization::VCS::TimeSignatureDeltas::timeSignaturesAdded);

    for (int i = 0; i < this->timeSignaturesSequence->size(); ++i)
    {
        const MidiEvent *event = this->timeSignaturesSequence->getUnchecked(i);
        tree.appendChild(event->serialize());
    }
    
    return tree;
}

void ProjectTimeline::resetTimeSignaturesDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::TimeSignatureDeltas::timeSignaturesAdded));
    this->timeSignaturesSequence->reset();
    
    forEachChildWithType(state, e, Serialization::Midi::timeSignature)
    {
        this->timeSignaturesSequence->checkoutEvent<TimeSignatureEvent>(e);
    }

    this->timeSignaturesSequence->updateBeatRange(false);
}

SerializedData ProjectTimeline::serializeKeySignaturesDelta() const
{
    SerializedData tree(Serialization::VCS::KeySignatureDeltas::keySignaturesAdded);

    for (int i = 0; i < this->keySignaturesSequence->size(); ++i)
    {
        const MidiEvent *event = this->keySignaturesSequence->getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}

void ProjectTimeline::resetKeySignaturesDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::KeySignatureDeltas::keySignaturesAdded));
    this->keySignaturesSequence->reset();

    forEachChildWithType(state, e, Serialization::Midi::keySignature)
    {
        this->keySignaturesSequence->checkoutEvent<KeySignatureEvent>(e);
    }

    this->keySignaturesSequence->updateBeatRange(false);
}

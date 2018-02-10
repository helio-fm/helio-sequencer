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
#include "ProjectTimelineDeltas.h"
#include "ProjectTreeItem.h"
#include "Pattern.h"
#include "Icons.h"


// A simple wrappers around the sequences
// We don't need any patterns here
class AnnotationsTrack : public EmptyMidiTrack
{
public:

    AnnotationsTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    Uuid getTrackId() const noexcept override
    { return this->timeline.annotationsId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.annotationsSequence; }

    ProjectTimeline &timeline;
};

class TimeSignaturesTrack : public EmptyMidiTrack
{
public:

    TimeSignaturesTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    Uuid getTrackId() const noexcept override
    { return this->timeline.timeSignaturesId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.timeSignaturesSequence; }

    ProjectTimeline &timeline;
};

class KeySignaturesTrack : public EmptyMidiTrack
{
public:

    KeySignaturesTrack(ProjectTimeline &owner) :
        timeline(owner) {}

    Uuid getTrackId() const noexcept override
    { return this->timeline.keySignaturesId; }

    MidiSequence *getSequence() const noexcept override
    { return this->timeline.keySignaturesSequence; }

    ProjectTimeline &timeline;
};

ProjectTimeline::ProjectTimeline(ProjectTreeItem &parentProject,
                                 String trackName) :
    project(parentProject),
    name(std::move(trackName))
{
    this->annotationsTrack = new AnnotationsTrack(*this);
    this->annotationsSequence = new AnnotationsSequence(*this->annotationsTrack, *this);

    this->timeSignaturesTrack = new TimeSignaturesTrack(*this);
    this->timeSignaturesSequence = new TimeSignaturesSequence(*this->timeSignaturesTrack, *this);

    this->keySignaturesTrack = new KeySignaturesTrack(*this);
    this->keySignaturesSequence = new KeySignaturesSequence(*this->keySignaturesTrack, *this);

    this->vcsDiffLogic = new VCS::ProjectTimelineDiffLogic(*this);
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::annotationsAdded));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::keySignaturesAdded));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::timeSignaturesAdded));

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
    if (this->deltas[index]->getType() == ProjectTimelineDeltas::annotationsAdded)
    {
        const int numEvents = this->annotationsSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} annotations", numEvents));
    }
    else if (this->deltas[index]->getType() == ProjectTimelineDeltas::timeSignaturesAdded)
    {
        const int numEvents = this->timeSignaturesSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} time signatures", numEvents));
    }
    else if (this->deltas[index]->getType() == ProjectTimelineDeltas::keySignaturesAdded)
    {
        const int numEvents = this->keySignaturesSequence->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} key signatures", numEvents));
    }

    return this->deltas[index];
}

XmlElement *ProjectTimeline::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == ProjectTimelineDeltas::annotationsAdded)
    {
        return this->serializeAnnotationsDelta();
    }
    else if (this->deltas[index]->getType() == ProjectTimelineDeltas::timeSignaturesAdded)
    {
        return this->serializeTimeSignaturesDelta();
    }
    else if (this->deltas[index]->getType() == ProjectTimelineDeltas::keySignaturesAdded)
    {
        return this->serializeKeySignaturesDelta();
    }

    jassertfalse;
    return nullptr;
}

VCS::DiffLogic *ProjectTimeline::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void ProjectTimeline::resetStateTo(const VCS::TrackedItem &newState)
{
    bool annotationsChanged = false;
    bool timeSignaturesChanged = false;
    bool keySignaturesChanged = false;

    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == ProjectTimelineDeltas::annotationsAdded)
        {
            this->resetAnnotationsDelta(newDeltaData);
            annotationsChanged = true;
        }
        else if (newDelta->getType() == ProjectTimelineDeltas::timeSignaturesAdded)
        {
            this->resetTimeSignaturesDelta(newDeltaData);
            timeSignaturesChanged = true;
        }
        else if (newDelta->getType() == ProjectTimelineDeltas::keySignaturesAdded)
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

ProjectTreeItem *ProjectTimeline::getProject() const
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
    XmlElement *xml = new XmlElement(this->vcsDiffLogic->getType());

    this->serializeVCSUuid(*xml);
    xml->setAttribute("name", this->name);
    xml->setAttribute("annotationsId", this->annotationsId.toString());
    xml->setAttribute("keySignaturesId", this->keySignaturesId.toString());
    xml->setAttribute("timeSignaturesId", this->timeSignaturesId.toString());

    xml->addChildElement(this->annotationsSequence->serialize());
    xml->addChildElement(this->keySignaturesSequence->serialize());
    xml->addChildElement(this->timeSignaturesSequence->serialize());

    return xml;
}

void ProjectTimeline::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const XmlElement *root = tree.hasTagName(this->vcsDiffLogic->getType()) ?
        &tree : tree.getChildByName(this->vcsDiffLogic->getType());
    
    if (root == nullptr)
    {
        return;
    }

    this->deserializeVCSUuid(*root);
    this->name = root->getStringAttribute("name", this->name);

    this->annotationsId =
        Uuid(root->getStringAttribute("annotationsId",
            this->annotationsId.toString()));

    this->timeSignaturesId =
        Uuid(root->getStringAttribute("timeSignaturesId",
            this->timeSignaturesId.toString()));

    this->keySignaturesId =
        Uuid(root->getStringAttribute("keySignaturesId",
            this->keySignaturesId.toString()));

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::annotations)
    {
        this->annotationsSequence->deserialize(*e);
    }

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::keySignatures)
    {
        this->keySignaturesSequence->deserialize(*e);
    }

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::timeSignatures)
    {
        this->timeSignaturesSequence->deserialize(*e);
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

XmlElement *ProjectTimeline::serializeAnnotationsDelta() const
{
    auto xml = new XmlElement(ProjectTimelineDeltas::annotationsAdded);

    for (int i = 0; i < this->annotationsSequence->size(); ++i)
    {
        const MidiEvent *event = this->annotationsSequence->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}

void ProjectTimeline::resetAnnotationsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectTimelineDeltas::annotationsAdded);
    this->annotationsSequence->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::annotation)
    {
        this->annotationsSequence->silentImport(
            AnnotationEvent(this->annotationsSequence.get()).withParameters(*e));
    }
}

XmlElement *ProjectTimeline::serializeTimeSignaturesDelta() const
{
    auto xml = new XmlElement(ProjectTimelineDeltas::timeSignaturesAdded);

    for (int i = 0; i < this->timeSignaturesSequence->size(); ++i)
    {
        const MidiEvent *event = this->timeSignaturesSequence->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }
    
    return xml;
}

void ProjectTimeline::resetTimeSignaturesDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectTimelineDeltas::timeSignaturesAdded);
    this->timeSignaturesSequence->reset();
    
    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::timeSignature)
    {
        this->timeSignaturesSequence->silentImport(
            TimeSignatureEvent(this->timeSignaturesSequence.get()).withParameters(*e));
    }
}

XmlElement *ProjectTimeline::serializeKeySignaturesDelta() const
{
    auto xml = new XmlElement(ProjectTimelineDeltas::keySignaturesAdded);

    for (int i = 0; i < this->keySignaturesSequence->size(); ++i)
    {
        const MidiEvent *event = this->keySignaturesSequence->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}

void ProjectTimeline::resetKeySignaturesDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectTimelineDeltas::keySignaturesAdded);
    this->keySignaturesSequence->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::keySignature)
    {
        this->keySignaturesSequence->silentImport(
            KeySignatureEvent(this->keySignaturesSequence.get()).withParameters(*e));
    }
}

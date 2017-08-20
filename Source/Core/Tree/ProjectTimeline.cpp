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

ProjectTimeline::ProjectTimeline(ProjectTreeItem &parentProject,
                                 String trackName) :
    project(parentProject),
    name(std::move(trackName))
{
    this->annotationsTrack = new AnnotationsTrack(*this);
    this->annotationsSequence = new AnnotationsSequence(*this->annotationsTrack, *this);

    this->timeSignaturesTrack = new TimeSignaturesTrack(*this);
    this->timeSignaturesSequence = new TimeSignaturesSequence(*this->timeSignaturesTrack, *this);

    this->vcsDiffLogic = new VCS::ProjectTimelineDiffLogic(*this);
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::annotationsAdded));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::timeSignaturesAdded));
    
    this->project.broadcastAddTrack(this->annotationsTrack);
    this->project.broadcastAddTrack(this->timeSignaturesTrack);
}

ProjectTimeline::~ProjectTimeline()
{
    this->project.broadcastRemoveTrack(this->timeSignaturesTrack);
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


//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String ProjectTimeline::getVCSName() const
{
    return ("vcs::items::timeline");
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
    bool signaturesChanged = false;
    
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
            signaturesChanged = true;
        }
    }
    
    if (annotationsChanged)
    {
        this->dispatchChangeTrackContent(this->annotationsTrack);
        this->dispatchChangeTrackBeatRange(this->annotationsTrack);
    }

    if (signaturesChanged)
    {
        this->dispatchChangeTrackContent(this->timeSignaturesTrack);
        this->dispatchChangeTrackBeatRange(this->timeSignaturesTrack);
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

void ProjectTimeline::dispatchChangeTrackBeatRange(MidiTrack *const track)
{
    this->project.broadcastChangeProjectBeatRange();
}

void ProjectTimeline::dispatchChangeTrackContent(MidiTrack *const track)
{
    this->project.broadcastResetTrackContent(track);
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
    this->timeSignaturesSequence->reset();
}

XmlElement *ProjectTimeline::serialize() const
{
    XmlElement *xml = new XmlElement(this->vcsDiffLogic->getType());

    this->serializeVCSUuid(*xml);
    xml->setAttribute("name", this->name);
    xml->setAttribute("annotationsId", this->annotationsId.toString());
    xml->setAttribute("timeSignaturesId", this->timeSignaturesId.toString());

    xml->addChildElement(this->annotationsSequence->serialize());
    xml->addChildElement(this->timeSignaturesSequence->serialize());

    return xml;
}

void ProjectTimeline::deserialize(const XmlElement &xml)
{
    this->reset();
    
    const XmlElement *root = xml.hasTagName(this->vcsDiffLogic->getType()) ?
        &xml : xml.getChildByName(this->vcsDiffLogic->getType());
    
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

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::annotations)
    {
        this->annotationsSequence->deserialize(*e);
    }

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::timeSignatures)
    {
        this->timeSignaturesSequence->deserialize(*e);
    }
    
    // Debug::
    //TimeSignatureEvent e(this->timeSignaturesSequence, 0.f, 9, 16);
    //(static_cast<TimeSignaturesSequence *>(this->timeSignaturesSequence.get()))->insert(e, false);
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
            AnnotationEvent(this->annotationsSequence).withParameters(*e));
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
            TimeSignatureEvent(this->timeSignaturesSequence).withParameters(*e));
    }
}

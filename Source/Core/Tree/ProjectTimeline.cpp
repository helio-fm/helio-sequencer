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
#include "AnnotationsLayer.h"
#include "TimeSignaturesLayer.h"
#include "ProjectTimelineDeltas.h"
#include "ProjectTreeItem.h"
#include "Icons.h"


ProjectTimeline::ProjectTimeline(ProjectTreeItem &parentProject,
                                 String trackName) :
    project(parentProject),
    name(std::move(trackName))
{
    this->annotations = new AnnotationsLayer(*this);
    this->timeSignatures = new TimeSignaturesLayer(*this);

    this->vcsDiffLogic = new VCS::ProjectTimelineDiffLogic(*this);
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::annotationsAdded));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::timeSignaturesAdded));
    
    this->project.broadcastLayerAdded(this->annotations);
    this->project.broadcastLayerAdded(this->timeSignatures);
}

ProjectTimeline::~ProjectTimeline()
{
    this->project.broadcastLayerRemoved(this->timeSignatures);
    this->project.broadcastLayerRemoved(this->annotations);
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
        const int numEvents = this->annotations->size();
        this->deltas[index]->setDescription(VCS::DeltaDescription("{x} annotations", numEvents));
    }
    else if (this->deltas[index]->getType() == ProjectTimelineDeltas::timeSignaturesAdded)
    {
        const int numEvents = this->timeSignatures->size();
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
        else if (newDelta->getType() == ProjectTimelineDeltas::timeSignaturesRemoved)
        {
            this->resetTimeSignaturesDelta(newDeltaData);
            signaturesChanged = true;
        }
    }
    
    if (annotationsChanged)
    {
        this->annotations->notifyLayerChanged();
        this->annotations->notifyBeatRangeChanged();
    }
    
    if (signaturesChanged)
    {
        this->timeSignatures->notifyLayerChanged();
        this->timeSignatures->notifyBeatRangeChanged();
    }
}


//===----------------------------------------------------------------------===//
// MidiLayerOwner
//===----------------------------------------------------------------------===//

Transport *ProjectTimeline::getTransport() const
{
    return &this->project.getTransport();
}

String ProjectTimeline::getXPath() const
{
    return this->name;
}

void ProjectTimeline::setXPath(const String &path)
{
    if (path != this->name)
    {
        this->name = path;
    }
}

void ProjectTimeline::onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    this->project.broadcastEventChanged(oldEvent, newEvent);
}

void ProjectTimeline::onEventAdded(const MidiEvent &event)
{
    this->project.broadcastEventAdded(event);
}

void ProjectTimeline::onEventRemoved(const MidiEvent &event)
{
    this->project.broadcastEventRemoved(event);
}

void ProjectTimeline::onLayerChanged(const MidiLayer *midiLayer)
{
    this->project.broadcastLayerChanged(midiLayer);
}

void ProjectTimeline::onBeatRangeChanged()
{
    this->project.broadcastBeatRangeChanged();
}

ProjectTreeItem *ProjectTimeline::getProject() const
{
    return &this->project;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void ProjectTimeline::reset()
{
    this->annotations->reset();
    this->timeSignatures->reset();
}

XmlElement *ProjectTimeline::serialize() const
{
    XmlElement *xml = new XmlElement(this->vcsDiffLogic->getType());

    this->serializeVCSUuid(*xml);
    xml->setAttribute("name", this->name);
    xml->addChildElement(this->annotations->serialize());
    xml->addChildElement(this->timeSignatures->serialize());

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
    this->name = root->getStringAttribute("name");

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::annotations)
    {
        this->annotations->deserialize(*e);
    }

    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::timeSignatures)
    {
        this->timeSignatures->deserialize(*e);
    }
    
    // Debug::
    //TimeSignatureEvent e(this->timeSignatures);
    //(static_cast<TimeSignaturesLayer *>(this->timeSignatures.get()))->insert(e, false);
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

XmlElement *ProjectTimeline::serializeAnnotationsDelta() const
{
    auto xml = new XmlElement(ProjectTimelineDeltas::annotationsAdded);

    for (int i = 0; i < this->annotations->size(); ++i)
    {
        const MidiEvent *event = this->annotations->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}

void ProjectTimeline::resetAnnotationsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectTimelineDeltas::annotationsAdded);

    this->annotations->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::annotation)
    {
        this->annotations->silentImport(AnnotationEvent(this->annotations).withParameters(*e));
    }
}

XmlElement *ProjectTimeline::serializeTimeSignaturesDelta() const
{
    auto xml = new XmlElement(ProjectTimelineDeltas::timeSignaturesAdded);
    
    for (int i = 0; i < this->timeSignatures->size(); ++i)
    {
        const MidiEvent *event = this->timeSignatures->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }
    
    return xml;
}

void ProjectTimeline::resetTimeSignaturesDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectTimelineDeltas::timeSignaturesAdded);
    
    this->timeSignatures->reset();
    
    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::timeSignature)
    {
        this->timeSignatures->silentImport(TimeSignatureEvent(this->timeSignatures).withParameters(*e));
    }
}
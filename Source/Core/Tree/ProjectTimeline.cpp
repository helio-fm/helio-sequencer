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
#include "ProjectTimelineDeltas.h"
#include "ProjectTreeItem.h"
#include "Icons.h"


ProjectTimeline::ProjectTimeline(ProjectTreeItem &parentProject,
                                       String trackName) :
    project(parentProject),
    name(std::move(trackName))
{
    this->layer = new AnnotationsLayer(*this);
    this->vcsDiffLogic = new VCS::ProjectTimelineDiffLogic(*this);
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), ProjectTimelineDeltas::annotationsAdded));
    
    this->project.broadcastLayerAdded(this->layer);
}

ProjectTimeline::~ProjectTimeline()
{
    this->project.broadcastLayerRemoved(this->layer);
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
        const int numEvents = this->layer->size();

        if (numEvents == 0)
        {
            // TODO "no annotations, no time signatures"
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty layer"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} annotations", numEvents));
        }
    }

    return this->deltas[index];
}

XmlElement *ProjectTimeline::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == ProjectTimelineDeltas::annotationsAdded)
    {
        return this->serializeAnnotationsDelta();
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
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == ProjectTimelineDeltas::annotationsAdded)
        {
            this->resetAnnotationsDelta(newDeltaData);
        }
    }
    
    this->getLayer()->notifyLayerChanged();
    this->getLayer()->notifyBeatRangeChanged();
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
    this->layer->reset();
}

XmlElement *ProjectTimeline::serialize() const
{
    XmlElement *xml = new XmlElement(this->vcsDiffLogic->getType());

    this->serializeVCSUuid(*xml);
    xml->setAttribute("name", this->name);
    xml->addChildElement(this->layer->serialize());

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

    // он все равно должен быть один, но так короче
    forEachXmlChildElementWithTagName(*root, e, Serialization::Core::annotations)
    {
        this->layer->deserialize(*e);
    }
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

XmlElement *ProjectTimeline::serializeAnnotationsDelta() const
{
    auto xml = new XmlElement(ProjectTimelineDeltas::annotationsAdded);

    for (int i = 0; i < this->layer->size(); ++i)
    {
        const MidiEvent *event = this->layer->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}

void ProjectTimeline::resetAnnotationsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == ProjectTimelineDeltas::annotationsAdded);

    this->reset();
    this->layer->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::annotation)
    {
        this->layer->silentImport(AnnotationEvent(this->layer).withParameters(*e));
    }
}

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
#include "ProjectAnnotations.h"
#include "AnnotationsLayer.h"
#include "AnnotationDeltas.h"
#include "ProjectTreeItem.h"
#include "Icons.h"


ProjectAnnotations::ProjectAnnotations(ProjectTreeItem &parentProject,
                                       String trackName) :
    project(parentProject),
    name(std::move(trackName))
{
    this->layer = new AnnotationsLayer(*this);
    this->vcsDiffLogic = new VCS::AnnotationsLayerDiffLogic(*this);
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AnnotationDeltas::annotationsAdded));
    
    this->project.broadcastLayerAdded(this->layer);
}

ProjectAnnotations::~ProjectAnnotations()
{
    this->project.broadcastLayerRemoved(this->layer);
}


//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String ProjectAnnotations::getVCSName() const
{
    return ("vcs::items::annotations");
}

int ProjectAnnotations::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *ProjectAnnotations::getDelta(int index) const
{
    if (this->deltas[index]->getType() == AnnotationDeltas::annotationsAdded)
    {
        const int numEvents = this->layer->size();

        if (numEvents == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty layer"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} annotations", numEvents));
        }
    }

    return this->deltas[index];
}

XmlElement *ProjectAnnotations::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == AnnotationDeltas::annotationsAdded)
    {
        return this->serializeAnnotationsDelta();
    }

    jassertfalse;
    return nullptr;
}

VCS::DiffLogic *ProjectAnnotations::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void ProjectAnnotations::resetStateTo(const VCS::TrackedItem &newState)
{
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == AnnotationDeltas::annotationsAdded)
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

Transport *ProjectAnnotations::getTransport() const
{
    return &this->project.getTransport();
}

String ProjectAnnotations::getXPath() const
{
    return this->name;
}

void ProjectAnnotations::setXPath(const String &path)
{
    if (path != this->name)
    {
        this->name = path;
    }
}

void ProjectAnnotations::onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    this->project.broadcastEventChanged(oldEvent, newEvent);
}

void ProjectAnnotations::onEventAdded(const MidiEvent &event)
{
    this->project.broadcastEventAdded(event);
}

void ProjectAnnotations::onEventRemoved(const MidiEvent &event)
{
    this->project.broadcastEventRemoved(event);
}

void ProjectAnnotations::onLayerChanged(const MidiLayer *midiLayer)
{
    this->project.broadcastLayerChanged(midiLayer);
}

void ProjectAnnotations::onBeatRangeChanged()
{
    this->project.broadcastBeatRangeChanged();
}

ProjectTreeItem *ProjectAnnotations::getProject() const
{
    return &this->project;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void ProjectAnnotations::reset()
{
    this->layer->reset();
}

XmlElement *ProjectAnnotations::serialize() const
{
    XmlElement *xml = new XmlElement(this->vcsDiffLogic->getType());

    this->serializeVCSUuid(*xml);
    xml->setAttribute("name", this->name);
    xml->addChildElement(this->layer->serialize());

    return xml;
}

void ProjectAnnotations::deserialize(const XmlElement &xml)
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

XmlElement *ProjectAnnotations::serializeAnnotationsDelta() const
{
    auto xml = new XmlElement(AnnotationDeltas::annotationsAdded);

    for (int i = 0; i < this->layer->size(); ++i)
    {
        const MidiEvent *event = this->layer->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}

void ProjectAnnotations::resetAnnotationsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AnnotationDeltas::annotationsAdded);

    this->reset();
    this->layer->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::annotation)
    {
        this->layer->silentImport(AnnotationEvent(this->layer).withParameters(*e));
    }
}

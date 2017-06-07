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
#include "PianoLayerTreeItem.h"
#include "PianoLayer.h"
#include "ProjectTreeItem.h"
#include "TreeItemChildrenSerializer.h"
#include "TreeItemComponent.h"
#include "Icons.h"

#include "MainLayout.h"
#include "Instrument.h"
#include "OrchestraPit.h"

#include "Delta.h"
#include "PianoLayerDeltas.h"
#include "PianoLayerDiffLogic.h"


PianoLayerTreeItem::PianoLayerTreeItem(const String &name) :
    LayerTreeItem(name)
{
    this->layer = new PianoLayer(*this);

    // плеер сам назначит
    //this->layer->setInstrumentId(this->workspace.getDefaultInstrument()->getInstrumentID());

    this->vcsDiffLogic = new VCS::PianoLayerDiffLogic(*this);

    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PianoLayerDeltas::layerPath));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PianoLayerDeltas::layerMute));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PianoLayerDeltas::layerColour));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PianoLayerDeltas::layerInstrument));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PianoLayerDeltas::notesAdded));
}

Image PianoLayerTreeItem::getIcon() const
{
    return Icons::findByName(Icons::layer, TREE_ICON_HEIGHT);
}


int PianoLayerTreeItem::getNumDeltas() const
{
    return this->deltas.size();
}

void PianoLayerTreeItem::selectAllPianoSiblings(PianoLayerTreeItem *layerItem)
{
    // select all layers in the project
    const auto pianoTreeItems = layerItem->getProject()->findChildrenOfType<PianoLayerTreeItem>();
    
    for (PianoLayerTreeItem *siblingPianoItem : pianoTreeItems)
    {
        if (siblingPianoItem != layerItem) // already selected, so don't blink twice
        {
            siblingPianoItem->setSelected(false, false, sendNotification);
            siblingPianoItem->setSelected(true, false, sendNotification);
        }
    }
    
    layerItem->setSelected(false, false, sendNotification);
    layerItem->setSelected(true, false, sendNotification);
}

//===----------------------------------------------------------------------===//
// VCS stuff
//===----------------------------------------------------------------------===//

VCS::Delta *PianoLayerTreeItem::getDelta(int index) const
{
    if (this->deltas[index]->getType() == PianoLayerDeltas::notesAdded)
    {
        const int numEvents = this->getLayer()->size();

        if (numEvents == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty layer"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} notes", numEvents));
        }
    }

    return this->deltas[index];
}

XmlElement *PianoLayerTreeItem::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == PianoLayerDeltas::layerPath)
    {
        return this->serializePathDelta();
    }
    if (this->deltas[index]->getType() == PianoLayerDeltas::layerMute)
    {
        return this->serializeMuteDelta();
    }
    else if (this->deltas[index]->getType() == PianoLayerDeltas::layerColour)
    {
        return this->serializeColourDelta();
    }
    else if (this->deltas[index]->getType() == PianoLayerDeltas::layerInstrument)
    {
        return this->serializeInstrumentDelta();
    }
    else if (this->deltas[index]->getType() == PianoLayerDeltas::notesAdded)
    {
        return this->serializeEventsDelta();
    }

    jassertfalse;
    return nullptr;
}

VCS::DiffLogic *PianoLayerTreeItem::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void PianoLayerTreeItem::resetStateTo(const VCS::TrackedItem &newState)
{
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == PianoLayerDeltas::layerPath)
        {
            this->resetPathDelta(newDeltaData);
        }
        else if (newDelta->getType() == PianoLayerDeltas::layerMute)
        {
            this->resetMuteDelta(newDeltaData);
        }
        else if (newDelta->getType() == PianoLayerDeltas::layerColour)
        {
            this->resetColourDelta(newDeltaData);
        }
        else if (newDelta->getType() == PianoLayerDeltas::layerInstrument)
        {
            this->resetInstrumentDelta(newDeltaData);
        }
        // предполагается, что у состояния будет одна нотная дельта типа notesAdded
        else if (newDelta->getType() == PianoLayerDeltas::notesAdded)
        {
            this->resetEventsDelta(newDeltaData);
        }
    }
    
    this->getLayer()->notifyLayerChanged();
    this->getLayer()->notifyBeatRangeChanged();
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *PianoLayerTreeItem::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::treeItem);

    this->serializeVCSUuid(*xml);

    xml->setAttribute("type", Serialization::Core::pianoLayer);
    xml->setAttribute("name", this->name);

    xml->addChildElement(this->layer->serialize());

    TreeItemChildrenSerializer::serializeChildren(*this, *xml);

    return xml;
}

void PianoLayerTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    const String& type = xml.getStringAttribute("type");

    if (type != Serialization::Core::pianoLayer) { return; }

    this->deserializeVCSUuid(xml);

    this->setName(xml.getStringAttribute("name"));

    // он все равно должен быть один, но так короче
    forEachXmlChildElementWithTagName(xml, e, Serialization::Core::track)
    {
        this->layer->deserialize(*e);
    }

    TreeItemChildrenSerializer::deserializeChildren(*this, xml);
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

XmlElement *PianoLayerTreeItem::serializePathDelta() const
{
    auto xml = new XmlElement(PianoLayerDeltas::layerPath);
    xml->setAttribute(Serialization::VCS::delta, this->getXPath());
    return xml;
}

XmlElement *PianoLayerTreeItem::serializeMuteDelta() const
{
    auto xml = new XmlElement(PianoLayerDeltas::layerMute);
    xml->setAttribute(Serialization::VCS::delta, this->getLayer()->getMuteStateAsString());
    return xml;
}

XmlElement *PianoLayerTreeItem::serializeColourDelta() const
{
    auto xml = new XmlElement(PianoLayerDeltas::layerColour);
    xml->setAttribute(Serialization::VCS::delta, this->getLayer()->getColour().toString());
    return xml;
}

XmlElement *PianoLayerTreeItem::serializeInstrumentDelta() const
{
    auto xml = new XmlElement(PianoLayerDeltas::layerInstrument);
    xml->setAttribute(Serialization::VCS::delta, this->getLayer()->getInstrumentId());
    return xml;
}

XmlElement *PianoLayerTreeItem::serializeEventsDelta() const
{
    auto xml = new XmlElement(PianoLayerDeltas::notesAdded);

    // да, дублируется сериализация :( причем 2 раза
    for (int i = 0; i < this->getLayer()->size(); ++i)
    {
        const MidiEvent *event = this->getLayer()->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}


void PianoLayerTreeItem::resetPathDelta(const XmlElement *state)
{
    jassert(state->getTagName() == PianoLayerDeltas::layerPath);
    const String &path(state->getStringAttribute(Serialization::VCS::delta));
    this->setXPath(path);
}

void PianoLayerTreeItem::resetMuteDelta(const XmlElement *state)
{
    jassert(state->getTagName() == PianoLayerDeltas::layerMute);
    const String &muteState(state->getStringAttribute(Serialization::VCS::delta));
    const bool willMute = MidiLayer::isMuted(muteState);
    
    if (willMute != this->getLayer()->isMuted())
    {
        this->getLayer()->setMuted(willMute);
        this->repaintItem();
    }
}

void PianoLayerTreeItem::resetColourDelta(const XmlElement *state)
{
    jassert(state->getTagName() == PianoLayerDeltas::layerColour);
    const String &colourString(state->getStringAttribute(Serialization::VCS::delta));
    const Colour &colour(Colour::fromString(colourString));

    if (colour != this->getLayer()->getColour())
    {
        this->getLayer()->setColour(colour);
        this->repaintItem();
    }
}

void PianoLayerTreeItem::resetInstrumentDelta(const XmlElement *state)
{
    jassert(state->getTagName() == PianoLayerDeltas::layerInstrument);
    const String &instrumentId(state->getStringAttribute(Serialization::VCS::delta));
    this->getLayer()->setInstrumentId(instrumentId);
}

void PianoLayerTreeItem::resetEventsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == PianoLayerDeltas::notesAdded);

    // не очень круто вызывать здесь reset, когда нужно просто очистить список событий
    this->reset();
    this->getLayer()->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::note)
    {
        this->getLayer()->silentImport(Note(this->getLayer()).withParameters(*e));
    }
}

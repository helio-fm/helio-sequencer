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
#include "AutomationLayerTreeItem.h"
#include "AutomationLayer.h"
#include "AutoLayerDeltas.h"
#include "TreeItemChildrenSerializer.h"
#include "Icons.h"
#include "TreeItemComponentCompact.h"
#include "TreeItemComponentDefault.h"

AutomationLayerTreeItem::AutomationLayerTreeItem(const String &name) :
    LayerTreeItem(name)
{
    this->layer = new AutomationLayer(*this);
    this->vcsDiffLogic = new VCS::AutomationLayerDiffLogic(*this);

    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoLayerDeltas::layerPath));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoLayerDeltas::layerMute));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoLayerDeltas::layerColour));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoLayerDeltas::layerInstrument));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoLayerDeltas::layerController));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoLayerDeltas::eventsAdded));
    
    this->setGreyedOut(true); // automations not visible by default
    
#if HELIO_MOBILE
    // для мобил выключаю автоматизации нафиг, неюзабельно будет совершенно
    this->setVisible(false);
#endif
}

Image AutomationLayerTreeItem::getIcon() const
{
    return Icons::findByName(Icons::automation, TREE_ICON_HEIGHT);
}

void AutomationLayerTreeItem::paintItem(Graphics &g, int width, int height)
{
    if (this->isCompactMode())
    {
        TreeItemComponentCompact::paintBackground(g, width, height, false, false);
    }
    else
    {
        TreeItemComponentDefault::paintBackground(g, width, height, false, false);
    }
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

int AutomationLayerTreeItem::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *AutomationLayerTreeItem::getDelta(int index) const
{
    if (this->deltas[index]->getType() == AutoLayerDeltas::eventsAdded)
    {
        const int numEvents = this->getLayer()->size();

        if (numEvents == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty layer"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} events", numEvents));
        }
    }

    return this->deltas[index];
}

XmlElement *AutomationLayerTreeItem::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == AutoLayerDeltas::layerPath)
    {
        return this->serializePathDelta();
    }
    if (this->deltas[index]->getType() == AutoLayerDeltas::layerMute)
    {
        return this->serializeMuteDelta();
    }
    else if (this->deltas[index]->getType() == AutoLayerDeltas::layerColour)
    {
        return this->serializeColourDelta();
    }
    else if (this->deltas[index]->getType() == AutoLayerDeltas::layerInstrument)
    {
        return this->serializeInstrumentDelta();
    }
    else if (this->deltas[index]->getType() == AutoLayerDeltas::layerController)
    {
        return this->serializeControllerDelta();
    }
    else if (this->deltas[index]->getType() == AutoLayerDeltas::eventsAdded)
    {
        return this->serializeEventsDelta();
    }

    jassertfalse;
    return nullptr;
}

VCS::DiffLogic *AutomationLayerTreeItem::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void AutomationLayerTreeItem::resetStateTo(const VCS::TrackedItem &newState)
{
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == AutoLayerDeltas::layerPath)
        {
            this->resetPathDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoLayerDeltas::layerMute)
        {
            this->resetMuteDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoLayerDeltas::layerColour)
        {
            this->resetColourDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoLayerDeltas::layerInstrument)
        {
            this->resetInstrumentDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoLayerDeltas::layerController)
        {
            this->resetControllerDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoLayerDeltas::eventsAdded)
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

XmlElement *AutomationLayerTreeItem::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::treeItem);

    this->serializeVCSUuid(*xml);

    xml->setAttribute("type", Serialization::Core::autoLayer);
    xml->setAttribute("name", this->name);

    xml->addChildElement(this->layer->serialize());

    TreeItemChildrenSerializer::serializeChildren(*this, *xml);

    return xml;
}

void AutomationLayerTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    const String& type = xml.getStringAttribute("type");

    if (type != Serialization::Core::autoLayer) { return; }

    this->deserializeVCSUuid(xml);

    this->setName(xml.getStringAttribute("name"));

    // он все равно должен быть один, но так короче
    forEachXmlChildElementWithTagName(xml, e, Serialization::Core::automation)
    {
        this->layer->deserialize(*e);
    }

    TreeItemChildrenSerializer::deserializeChildren(*this, xml);
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

// вот все это можно вынести в LayerTreeItem

XmlElement *AutomationLayerTreeItem::serializePathDelta() const
{
    auto xml = new XmlElement(AutoLayerDeltas::layerPath);
    xml->setAttribute(Serialization::VCS::delta, this->getXPath());
    return xml;
}

XmlElement *AutomationLayerTreeItem::serializeMuteDelta() const
{
    auto xml = new XmlElement(AutoLayerDeltas::layerMute);
    xml->setAttribute(Serialization::VCS::delta, this->getLayer()->getMuteStateAsString());
    return xml;
}

XmlElement *AutomationLayerTreeItem::serializeColourDelta() const
{
    auto xml = new XmlElement(AutoLayerDeltas::layerColour);
    xml->setAttribute(Serialization::VCS::delta, this->getLayer()->getColour().toString());
    return xml;
}

XmlElement *AutomationLayerTreeItem::serializeInstrumentDelta() const
{
    auto xml = new XmlElement(AutoLayerDeltas::layerInstrument);
    xml->setAttribute(Serialization::VCS::delta, this->getLayer()->getInstrumentId());
    return xml;
}

XmlElement *AutomationLayerTreeItem::serializeControllerDelta() const
{
    auto xml = new XmlElement(AutoLayerDeltas::layerController);
    xml->setAttribute(Serialization::VCS::delta, this->getLayer()->getControllerNumber());
    return xml;
}

XmlElement *AutomationLayerTreeItem::serializeEventsDelta() const
{
    auto xml = new XmlElement(AutoLayerDeltas::eventsAdded);

    for (int i = 0; i < this->getLayer()->size(); ++i)
    {
        const MidiEvent *event = this->getLayer()->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}


void AutomationLayerTreeItem::resetPathDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoLayerDeltas::layerPath);
    const String &path(state->getStringAttribute(Serialization::VCS::delta));
    this->setXPath(path);
}

void AutomationLayerTreeItem::resetMuteDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoLayerDeltas::layerMute);
    const String &muteState(state->getStringAttribute(Serialization::VCS::delta));
    const bool willMute = MidiLayer::isMuted(muteState);
    
    if (willMute != this->getLayer()->isMuted())
    {
        this->getLayer()->setMuted(willMute);
        this->repaintItem();
    }
}

void AutomationLayerTreeItem::resetColourDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoLayerDeltas::layerColour);
    const String &colourString(state->getStringAttribute(Serialization::VCS::delta));
    const Colour &colour(Colour::fromString(colourString));

    if (colour != this->getLayer()->getColour())
    {
        this->getLayer()->setColour(colour);
        this->repaintItem();
    }
}

void AutomationLayerTreeItem::resetInstrumentDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoLayerDeltas::layerInstrument);
    const String &instrumentId(state->getStringAttribute(Serialization::VCS::delta));
    this->getLayer()->setInstrumentId(instrumentId);
}

void AutomationLayerTreeItem::resetControllerDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoLayerDeltas::layerController);
    const int ccNumber(state->getIntAttribute(Serialization::VCS::delta));
    this->getLayer()->setControllerNumber(ccNumber);
}

void AutomationLayerTreeItem::resetEventsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoLayerDeltas::eventsAdded);

    this->reset();
    this->getLayer()->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::event)
    {
        this->getLayer()->silentImport(AutomationEvent(this->getLayer()).withParameters(*e));
    }
}

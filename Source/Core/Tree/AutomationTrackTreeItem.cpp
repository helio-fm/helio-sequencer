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
#include "AutomationTrackTreeItem.h"
#include "AutomationSequence.h"
#include "AutoSequenceDeltas.h"
#include "TreeItemChildrenSerializer.h"
#include "Icons.h"
#include "TreeItemComponentCompact.h"
#include "TreeItemComponentDefault.h"
#include "Pattern.h"
#include "PatternDeltas.h"

AutomationTrackTreeItem::AutomationTrackTreeItem(const String &name) :
    MidiTrackTreeItem(name, Serialization::Core::autoLayer)
{
    this->layer = new AutomationSequence(*this, *this);
    this->pattern = new Pattern(*this, *this);

    this->vcsDiffLogic = new VCS::AutomationTrackDiffLogic(*this);

    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoSequenceDeltas::layerPath));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoSequenceDeltas::layerMute));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoSequenceDeltas::layerColour));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoSequenceDeltas::layerInstrument));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoSequenceDeltas::layerController));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), AutoSequenceDeltas::eventsAdded));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PatternDeltas::clipsAdded));
    
#if HELIO_MOBILE
    // для мобил выключаю автоматизации нафиг, неюзабельно будет совершенно
    this->setVisible(false);
#endif
}

Image AutomationTrackTreeItem::getIcon() const
{
    return Icons::findByName(Icons::automation, TREE_ICON_HEIGHT);
}

void AutomationTrackTreeItem::paintItem(Graphics &g, int width, int height)
{
    TreeItemComponentDefault::paintBackground(g, width, height, false, false);
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

int AutomationTrackTreeItem::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *AutomationTrackTreeItem::getDelta(int index) const
{
    if (this->deltas[index]->getType() == AutoSequenceDeltas::eventsAdded)
    {
        const int numEvents = this->getSequence()->size();

        if (numEvents == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty layer"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} events", numEvents));
        }
    }
    else if (this->deltas[index]->getType() == PatternDeltas::clipsAdded)
    {
        const int numClips = this->getPattern()->size();

        if (numClips == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty pattern"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} clips", numClips));
        }
    }

    return this->deltas[index];
}

XmlElement *AutomationTrackTreeItem::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == AutoSequenceDeltas::layerPath)
    {
        return this->serializePathDelta();
    }
    if (this->deltas[index]->getType() == AutoSequenceDeltas::layerMute)
    {
        return this->serializeMuteDelta();
    }
    else if (this->deltas[index]->getType() == AutoSequenceDeltas::layerColour)
    {
        return this->serializeColourDelta();
    }
    else if (this->deltas[index]->getType() == AutoSequenceDeltas::layerInstrument)
    {
        return this->serializeInstrumentDelta();
    }
    else if (this->deltas[index]->getType() == AutoSequenceDeltas::layerController)
    {
        return this->serializeControllerDelta();
    }
    else if (this->deltas[index]->getType() == AutoSequenceDeltas::eventsAdded)
    {
        return this->serializeEventsDelta();
    }
    else if (this->deltas[index]->getType() == PatternDeltas::clipsAdded)
    {
        return this->serializeClipsDelta();
    }

    jassertfalse;
    return nullptr;
}

VCS::DiffLogic *AutomationTrackTreeItem::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void AutomationTrackTreeItem::resetStateTo(const VCS::TrackedItem &newState)
{
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == AutoSequenceDeltas::layerPath)
        {
            this->resetPathDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoSequenceDeltas::layerMute)
        {
            this->resetMuteDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoSequenceDeltas::layerColour)
        {
            this->resetColourDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoSequenceDeltas::layerInstrument)
        {
            this->resetInstrumentDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoSequenceDeltas::layerController)
        {
            this->resetControllerDelta(newDeltaData);
        }
        else if (newDelta->getType() == AutoSequenceDeltas::eventsAdded)
        {
            this->resetEventsDelta(newDeltaData);
        }
        else if (newDelta->getType() == PatternDeltas::clipsAdded)
        {
            this->resetClipsDelta(newDeltaData);
        }
    }
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *AutomationTrackTreeItem::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::treeItem);

    this->serializeVCSUuid(*xml);

    xml->setAttribute(Serialization::Core::treeItemType, this->type);
    xml->setAttribute(Serialization::Core::treeItemName, this->name);

    this->serializeTrackProperties(*xml);

    xml->addChildElement(this->layer->serialize());
    xml->addChildElement(this->pattern->serialize());

    TreeItemChildrenSerializer::serializeChildren(*this, *xml);

    return xml;
}

void AutomationTrackTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    this->deserializeVCSUuid(xml);
    this->deserializeTrackProperties(xml);

    // он все равно должен быть один, но так короче
    forEachXmlChildElementWithTagName(xml, e, Serialization::Core::automation)
    {
        this->layer->deserialize(*e);
    }

    forEachXmlChildElementWithTagName(xml, e, Serialization::Core::pattern)
    {
        this->pattern->deserialize(*e);
    }

    // Proceed with basic properties and children
    TreeItem::deserialize(xml);
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

// вот все это можно вынести в LayerTreeItem

XmlElement *AutomationTrackTreeItem::serializePathDelta() const
{
    auto xml = new XmlElement(AutoSequenceDeltas::layerPath);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackName());
    return xml;
}

XmlElement *AutomationTrackTreeItem::serializeMuteDelta() const
{
    auto xml = new XmlElement(AutoSequenceDeltas::layerMute);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackMuteStateAsString());
    return xml;
}

XmlElement *AutomationTrackTreeItem::serializeColourDelta() const
{
    auto xml = new XmlElement(AutoSequenceDeltas::layerColour);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackColour().toString());
    return xml;
}

XmlElement *AutomationTrackTreeItem::serializeInstrumentDelta() const
{
    auto xml = new XmlElement(AutoSequenceDeltas::layerInstrument);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackInstrumentId());
    return xml;
}

XmlElement *AutomationTrackTreeItem::serializeControllerDelta() const
{
    auto xml = new XmlElement(AutoSequenceDeltas::layerController);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackControllerNumber());
    return xml;
}

XmlElement *AutomationTrackTreeItem::serializeEventsDelta() const
{
    auto xml = new XmlElement(AutoSequenceDeltas::eventsAdded);

    for (int i = 0; i < this->getSequence()->size(); ++i)
    {
        const MidiEvent *event = this->getSequence()->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}


void AutomationTrackTreeItem::resetPathDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerPath);
    const String &path(state->getStringAttribute(Serialization::VCS::delta));
    this->setXPath(path);
}

void AutomationTrackTreeItem::resetMuteDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerMute);
    const String &muteState(state->getStringAttribute(Serialization::VCS::delta));
    const bool willMute = MidiTrack::isTrackMuted(muteState);
    
    if (willMute != this->isTrackMuted())
    {
        this->setTrackMuted(willMute);
    }
}

void AutomationTrackTreeItem::resetColourDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerColour);
    const String &colourString(state->getStringAttribute(Serialization::VCS::delta));
    const Colour &colour(Colour::fromString(colourString));

    if (colour != this->getTrackColour())
    {
        this->setTrackColour(colour);
    }
}

void AutomationTrackTreeItem::resetInstrumentDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerInstrument);
    const String &instrumentId(state->getStringAttribute(Serialization::VCS::delta));
    this->setTrackInstrumentId(instrumentId);
}

void AutomationTrackTreeItem::resetControllerDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerController);
    const int ccNumber(state->getIntAttribute(Serialization::VCS::delta));
    this->setTrackControllerNumber(ccNumber);
}

void AutomationTrackTreeItem::resetEventsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::eventsAdded);

    this->reset();
    this->getSequence()->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::event)
    {
        this->getSequence()->silentImport(AutomationEvent(this->getSequence()).withParameters(*e));
    }
}

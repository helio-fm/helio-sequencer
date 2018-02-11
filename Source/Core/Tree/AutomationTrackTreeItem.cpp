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

ValueTree AutomationTrackTreeItem::serialize() const
{
    ValueTree tree(Serialization::Core::treeItem);

    this->serializeVCSUuid(*xml);

    tree.setProperty(Serialization::Core::treeItemType, this->type);
    tree.setProperty(Serialization::Core::treeItemName, this->name);

    this->serializeTrackProperties(*xml);

    tree.appendChild(this->layer->serialize());
    tree.appendChild(this->pattern->serialize());

    TreeItemChildrenSerializer::serializeChildren(*this, *xml);

    return tree;
}

void AutomationTrackTreeItem::deserialize(const ValueTree &tree)
{
    this->reset();

    this->deserializeVCSUuid(tree);
    this->deserializeTrackProperties(tree);

    // он все равно должен быть один, но так короче
    forEachXmlChildElementWithTagName(tree, e, Serialization::Core::automation)
    {
        this->layer->deserialize(*e);
    }

    forEachXmlChildElementWithTagName(tree, e, Serialization::Core::pattern)
    {
        this->pattern->deserialize(*e);
    }

    // Proceed with basic properties and children
    TreeItem::deserialize(tree);
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

// вот все это можно вынести в LayerTreeItem

XmlElement *AutomationTrackTreeItem::serializePathDelta() const
{
    ValueTree tree(AutoSequenceDeltas::layerPath);
    tree.setProperty(Serialization::VCS::delta, this->getTrackName());
    return tree;
}

XmlElement *AutomationTrackTreeItem::serializeMuteDelta() const
{
    ValueTree tree(AutoSequenceDeltas::layerMute);
    tree.setProperty(Serialization::VCS::delta, this->getTrackMuteStateAsString());
    return tree;
}

XmlElement *AutomationTrackTreeItem::serializeColourDelta() const
{
    ValueTree tree(AutoSequenceDeltas::layerColour);
    tree.setProperty(Serialization::VCS::delta, this->getTrackColour().toString());
    return tree;
}

XmlElement *AutomationTrackTreeItem::serializeInstrumentDelta() const
{
    ValueTree tree(AutoSequenceDeltas::layerInstrument);
    tree.setProperty(Serialization::VCS::delta, this->getTrackInstrumentId());
    return tree;
}

XmlElement *AutomationTrackTreeItem::serializeControllerDelta() const
{
    ValueTree tree(AutoSequenceDeltas::layerController);
    tree.setProperty(Serialization::VCS::delta, this->getTrackControllerNumber());
    return tree;
}

XmlElement *AutomationTrackTreeItem::serializeEventsDelta() const
{
    ValueTree tree(AutoSequenceDeltas::eventsAdded);

    for (int i = 0; i < this->getSequence()->size(); ++i)
    {
        const MidiEvent *event = this->getSequence()->getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}


void AutomationTrackTreeItem::resetPathDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerPath);
    const String &path(state.getProperty(Serialization::VCS::delta));
    this->setXPath(path);
}

void AutomationTrackTreeItem::resetMuteDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerMute);
    const String &muteState(state.getProperty(Serialization::VCS::delta));
    const bool willMute = MidiTrack::isTrackMuted(muteState);
    
    if (willMute != this->isTrackMuted())
    {
        this->setTrackMuted(willMute, false);
    }
}

void AutomationTrackTreeItem::resetColourDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerColour);
    const String &colourString(state.getProperty(Serialization::VCS::delta));
    const Colour &colour(Colour::fromString(colourString));

    if (colour != this->getTrackColour())
    {
        this->setTrackColour(colour, false);
    }
}

void AutomationTrackTreeItem::resetInstrumentDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerInstrument);
    const String &instrumentId(state.getProperty(Serialization::VCS::delta));
    this->setTrackInstrumentId(instrumentId, false);
}

void AutomationTrackTreeItem::resetControllerDelta(const XmlElement *state)
{
    jassert(state->getTagName() == AutoSequenceDeltas::layerController);
    const int ccNumber(state->getIntAttribute(Serialization::VCS::delta));
    this->setTrackControllerNumber(ccNumber, false);
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

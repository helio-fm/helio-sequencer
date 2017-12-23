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
#include "PianoTrackTreeItem.h"
#include "PianoSequence.h"
#include "ProjectTreeItem.h"
#include "TreeItemChildrenSerializer.h"
#include "TreeItemComponent.h"
#include "Icons.h"
#include "Pattern.h"

#include "MainLayout.h"
#include "Instrument.h"
#include "OrchestraPit.h"

#include "Delta.h"
#include "PatternDeltas.h"
#include "PianoSequenceDeltas.h"
#include "MidiTrackDeltas.h"
#include "PianoTrackDiffLogic.h"

PianoTrackTreeItem::PianoTrackTreeItem(const String &name) :
    MidiTrackTreeItem(name, Serialization::Core::pianoLayer)
{
    this->layer = new PianoSequence(*this, *this);
    this->pattern = new Pattern(*this, *this);

    // this will be set by transport
    //this->layer->setInstrumentId(this->workspace.getDefaultInstrument()->getInstrumentID());

    this->vcsDiffLogic = new VCS::PianoTrackDiffLogic(*this);

    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), MidiTrackDeltas::trackPath));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), MidiTrackDeltas::trackMute));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), MidiTrackDeltas::trackColour));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), MidiTrackDeltas::trackInstrument));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PianoSequenceDeltas::notesAdded));
    this->deltas.add(new VCS::Delta(VCS::DeltaDescription(""), PatternDeltas::clipsAdded));
}

Image PianoTrackTreeItem::getIcon() const
{
    return Icons::findByName(Icons::layer, TREE_ICON_HEIGHT);
}


int PianoTrackTreeItem::getNumDeltas() const
{
    return this->deltas.size();
}

void PianoTrackTreeItem::selectAllPianoSiblings(PianoTrackTreeItem *layerItem)
{
    // select all layers in the project
    const auto pianoTreeItems = layerItem->getProject()->findChildrenOfType<PianoTrackTreeItem>();
    
    for (PianoTrackTreeItem *siblingPianoItem : pianoTreeItems)
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

VCS::Delta *PianoTrackTreeItem::getDelta(int index) const
{
    if (this->deltas[index]->getType() == PianoSequenceDeltas::notesAdded)
    {
        const int numEvents = this->getSequence()->size();

        if (numEvents == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty layer"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} notes", numEvents));
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

XmlElement *PianoTrackTreeItem::createDeltaDataFor(int index) const
{
    if (this->deltas[index]->getType() == MidiTrackDeltas::trackPath)
    {
        return this->serializePathDelta();
    }
    if (this->deltas[index]->getType() == MidiTrackDeltas::trackMute)
    {
        return this->serializeMuteDelta();
    }
    else if (this->deltas[index]->getType() == MidiTrackDeltas::trackColour)
    {
        return this->serializeColourDelta();
    }
    else if (this->deltas[index]->getType() == MidiTrackDeltas::trackInstrument)
    {
        return this->serializeInstrumentDelta();
    }
    else if (this->deltas[index]->getType() == PianoSequenceDeltas::notesAdded)
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

VCS::DiffLogic *PianoTrackTreeItem::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void PianoTrackTreeItem::resetStateTo(const VCS::TrackedItem &newState)
{
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        ScopedPointer<XmlElement> newDeltaData(newState.createDeltaDataFor(i));
        
        if (newDelta->getType() == MidiTrackDeltas::trackPath)
        {
            this->resetPathDelta(newDeltaData);
        }
        else if (newDelta->getType() == MidiTrackDeltas::trackMute)
        {
            this->resetMuteDelta(newDeltaData);
        }
        else if (newDelta->getType() == MidiTrackDeltas::trackColour)
        {
            this->resetColourDelta(newDeltaData);
        }
        else if (newDelta->getType() == MidiTrackDeltas::trackInstrument)
        {
            this->resetInstrumentDelta(newDeltaData);
        }
        // the current layer state is supposed to have
        // a single note delta of type PianoSequenceDeltas::notesAdded
        else if (newDelta->getType() == PianoSequenceDeltas::notesAdded)
        {
            this->resetEventsDelta(newDeltaData);
        }
        // same rule applies to clips state:
        else if (newDelta->getType() == PatternDeltas::clipsAdded)
        {
            this->resetClipsDelta(newDeltaData);
        }
    }
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *PianoTrackTreeItem::serialize() const
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

void PianoTrackTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    this->deserializeVCSUuid(xml);
    this->deserializeTrackProperties(xml);

    // он все равно должен быть один, но так короче
    forEachXmlChildElementWithTagName(xml, e, Serialization::Core::track)
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

XmlElement *PianoTrackTreeItem::serializePathDelta() const
{
    auto xml = new XmlElement(MidiTrackDeltas::trackPath);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackName());
    return xml;
}

XmlElement *PianoTrackTreeItem::serializeMuteDelta() const
{
    auto xml = new XmlElement(MidiTrackDeltas::trackMute);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackMuteStateAsString());
    return xml;
}

XmlElement *PianoTrackTreeItem::serializeColourDelta() const
{
    auto xml = new XmlElement(MidiTrackDeltas::trackColour);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackColour().toString());
    return xml;
}

XmlElement *PianoTrackTreeItem::serializeInstrumentDelta() const
{
    auto xml = new XmlElement(MidiTrackDeltas::trackInstrument);
    xml->setAttribute(Serialization::VCS::delta, this->getTrackInstrumentId());
    return xml;
}

XmlElement *PianoTrackTreeItem::serializeEventsDelta() const
{
    auto xml = new XmlElement(PianoSequenceDeltas::notesAdded);

    // да, дублируется сериализация :( причем 2 раза
    for (int i = 0; i < this->getSequence()->size(); ++i)
    {
        const MidiEvent *event = this->getSequence()->getUnchecked(i);
        xml->addChildElement(event->serialize());
    }

    return xml;
}


void PianoTrackTreeItem::resetPathDelta(const XmlElement *state)
{
    jassert(state->getTagName() == MidiTrackDeltas::trackPath);
    const String &path(state->getStringAttribute(Serialization::VCS::delta));
    this->setXPath(path);
}

void PianoTrackTreeItem::resetMuteDelta(const XmlElement *state)
{
    jassert(state->getTagName() == MidiTrackDeltas::trackMute);
    const String &muteState(state->getStringAttribute(Serialization::VCS::delta));
    const bool willMute = MidiTrack::isTrackMuted(muteState);
    
    if (willMute != this->isTrackMuted())
    {
        this->setTrackMuted(willMute, false);
    }
}

void PianoTrackTreeItem::resetColourDelta(const XmlElement *state)
{
    jassert(state->getTagName() == MidiTrackDeltas::trackColour);
    const String &colourString(state->getStringAttribute(Serialization::VCS::delta));
    const Colour &colour(Colour::fromString(colourString));

    if (colour != this->getTrackColour())
    {
        this->setTrackColour(colour, false);
    }
}

void PianoTrackTreeItem::resetInstrumentDelta(const XmlElement *state)
{
    jassert(state->getTagName() == MidiTrackDeltas::trackInstrument);
    const String &instrumentId(state->getStringAttribute(Serialization::VCS::delta));
    this->setTrackInstrumentId(instrumentId, false);
}

void PianoTrackTreeItem::resetEventsDelta(const XmlElement *state)
{
    jassert(state->getTagName() == PianoSequenceDeltas::notesAdded);

    //this->reset(); // TODO test
    this->getSequence()->reset();

    forEachXmlChildElementWithTagName(*state, e, Serialization::Core::note)
    {
        this->getSequence()->silentImport(Note(this->getSequence()).withParameters(*e));
    }
}

// TODO manage clip deltas

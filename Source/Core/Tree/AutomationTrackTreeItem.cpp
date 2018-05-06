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
#include "TreeItemChildrenSerializer.h"
#include "Icons.h"
#include "TreeItemComponentCompact.h"
#include "TreeItemComponentDefault.h"
#include "Pattern.h"

using namespace Serialization::VCS;

AutomationTrackTreeItem::AutomationTrackTreeItem(const String &name) :
    MidiTrackTreeItem(name, Serialization::Core::automationTrack)
{
    this->sequence = new AutomationSequence(*this, *this);
    this->pattern = new Pattern(*this, *this);

    this->vcsDiffLogic = new VCS::AutomationTrackDiffLogic(*this);

    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackPath));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackMute));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackColour));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackInstrument));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackController));
    this->deltas.add(new VCS::Delta({}, AutoSequenceDeltas::eventsAdded));
    this->deltas.add(new VCS::Delta({}, PatternDeltas::clipsAdded));
    
#if HELIO_MOBILE
    // для мобил выключаю автоматизации нафиг, неюзабельно будет совершенно
    this->setVisible(false);
#endif
}

Image AutomationTrackTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::automationTrack, HEADLINE_ICON_SIZE);
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
    if (this->deltas[index]->hasType(AutoSequenceDeltas::eventsAdded))
    {
        const int numEvents = this->getSequence()->size();

        if (numEvents == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty sequence"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} events", numEvents));
        }
    }
    else if (this->deltas[index]->hasType(PatternDeltas::clipsAdded))
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

ValueTree AutomationTrackTreeItem::serializeDeltaData(int deltaIndex) const
{
    if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackPath))
    {
        return this->serializePathDelta();
    }
    if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackMute))
    {
        return this->serializeMuteDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackColour))
    {
        return this->serializeColourDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackInstrument))
    {
        return this->serializeInstrumentDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackController))
    {
        return this->serializeControllerDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(AutoSequenceDeltas::eventsAdded))
    {
        return this->serializeEventsDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(PatternDeltas::clipsAdded))
    {
        return this->serializeClipsDelta();
    }

    jassertfalse;
    return {};
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
        const auto newDeltaData(newState.serializeDeltaData(i));
        
        if (newDelta->hasType(MidiTrackDeltas::trackPath))
        {
            this->resetPathDelta(newDeltaData);
        }
        else if (newDelta->hasType(MidiTrackDeltas::trackMute))
        {
            this->resetMuteDelta(newDeltaData);
        }
        else if (newDelta->hasType(MidiTrackDeltas::trackColour))
        {
            this->resetColourDelta(newDeltaData);
        }
        else if (newDelta->hasType(MidiTrackDeltas::trackInstrument))
        {
            this->resetInstrumentDelta(newDeltaData);
        }
        else if (newDelta->hasType(MidiTrackDeltas::trackController))
        {
            this->resetControllerDelta(newDeltaData);
        }
        else if (newDelta->hasType(AutoSequenceDeltas::eventsAdded))
        {
            this->resetEventsDelta(newDeltaData);
        }
        else if (newDelta->hasType(PatternDeltas::clipsAdded))
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

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::Core::treeItemType, this->type, nullptr);
    tree.setProperty(Serialization::Core::treeItemName, this->name, nullptr);

    this->serializeTrackProperties(tree);

    tree.appendChild(this->sequence->serialize(), nullptr);
    tree.appendChild(this->pattern->serialize(), nullptr);

    TreeItemChildrenSerializer::serializeChildren(*this, tree);

    return tree;
}

void AutomationTrackTreeItem::deserialize(const ValueTree &tree)
{
    this->reset();

    this->deserializeVCSUuid(tree);
    this->deserializeTrackProperties(tree);

    forEachValueTreeChildWithType(tree, e, Serialization::Midi::automation)
    {
        this->sequence->deserialize(e);
    }

    forEachValueTreeChildWithType(tree, e, Serialization::Midi::pattern)
    {
        this->pattern->deserialize(e);
    }

    // Proceed with basic properties and children
    TreeItem::deserialize(tree);
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

// TODO move this in MidiTrackTreeItem

ValueTree AutomationTrackTreeItem::serializePathDelta() const
{
    ValueTree tree(MidiTrackDeltas::trackPath);
    tree.setProperty(Serialization::VCS::delta, this->getTrackName(), nullptr);
    return tree;
}

ValueTree AutomationTrackTreeItem::serializeMuteDelta() const
{
    ValueTree tree(MidiTrackDeltas::trackMute);
    tree.setProperty(Serialization::VCS::delta, this->getTrackMuteStateAsString(), nullptr);
    return tree;
}

ValueTree AutomationTrackTreeItem::serializeColourDelta() const
{
    ValueTree tree(MidiTrackDeltas::trackColour);
    tree.setProperty(Serialization::VCS::delta, this->getTrackColour().toString(), nullptr);
    return tree;
}

ValueTree AutomationTrackTreeItem::serializeInstrumentDelta() const
{
    ValueTree tree(MidiTrackDeltas::trackInstrument);
    tree.setProperty(Serialization::VCS::delta, this->getTrackInstrumentId(), nullptr);
    return tree;
}

ValueTree AutomationTrackTreeItem::serializeControllerDelta() const
{
    ValueTree tree(MidiTrackDeltas::trackController);
    tree.setProperty(Serialization::VCS::delta, this->getTrackControllerNumber(), nullptr);
    return tree;
}

ValueTree AutomationTrackTreeItem::serializeEventsDelta() const
{
    ValueTree tree(AutoSequenceDeltas::eventsAdded);

    for (int i = 0; i < this->getSequence()->size(); ++i)
    {
        const MidiEvent *event = this->getSequence()->getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }

    return tree;
}


void AutomationTrackTreeItem::resetPathDelta(const ValueTree &state)
{
    jassert(state.hasType(MidiTrackDeltas::trackPath));
    const String &path(state.getProperty(Serialization::VCS::delta));
    this->setXPath(path);
}

void AutomationTrackTreeItem::resetMuteDelta(const ValueTree &state)
{
    jassert(state.hasType(MidiTrackDeltas::trackMute));
    const String &muteState(state.getProperty(Serialization::VCS::delta));
    const bool willMute = MidiTrack::isTrackMuted(muteState);
    
    if (willMute != this->isTrackMuted())
    {
        this->setTrackMuted(willMute, false);
    }
}

void AutomationTrackTreeItem::resetColourDelta(const ValueTree &state)
{
    jassert(state.hasType(MidiTrackDeltas::trackColour));
    const String &colourString(state.getProperty(Serialization::VCS::delta));
    const Colour &colour(Colour::fromString(colourString));

    if (colour != this->getTrackColour())
    {
        this->setTrackColour(colour, false);
    }
}

void AutomationTrackTreeItem::resetInstrumentDelta(const ValueTree &state)
{
    jassert(state.hasType(MidiTrackDeltas::trackInstrument));
    const String &instrumentId(state.getProperty(Serialization::VCS::delta));
    this->setTrackInstrumentId(instrumentId, false);
}

void AutomationTrackTreeItem::resetControllerDelta(const ValueTree &state)
{
    jassert(state.hasType(MidiTrackDeltas::trackController));
    const int ccNumber(state.getProperty(Serialization::VCS::delta));
    this->setTrackControllerNumber(ccNumber, false);
}

void AutomationTrackTreeItem::resetEventsDelta(const ValueTree &state)
{
    jassert(state.hasType(AutoSequenceDeltas::eventsAdded));

    this->reset();
    this->getSequence()->reset();

    forEachValueTreeChildWithType(state, e, Serialization::Midi::automationEvent)
    {
        this->getSequence()->silentImport(AutomationEvent(this->getSequence()).withParameters(e));
    }
}

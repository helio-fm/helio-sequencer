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
#include "PianoTrackNode.h"
#include "PianoSequence.h"
#include "ProjectNode.h"
#include "TreeNodeSerializer.h"
#include "Icons.h"
#include "Pattern.h"

#include "MainLayout.h"
#include "Instrument.h"
#include "OrchestraPit.h"

#include "Delta.h"
#include "PianoTrackDiffLogic.h"

PianoTrackNode::PianoTrackNode(const String &name) :
    MidiTrackNode(name, Serialization::Core::pianoTrack)
{
    this->sequence = new PianoSequence(*this, *this);
    this->pattern = new Pattern(*this, *this);

    // this will be set by transport
    //this->layer->setInstrumentId(this->workspace.getDefaultInstrument()->getInstrumentID());

    this->vcsDiffLogic = new VCS::PianoTrackDiffLogic(*this);

    using namespace Serialization::VCS;
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackPath));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackMute));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackColour));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackInstrument));
    this->deltas.add(new VCS::Delta({}, PianoSequenceDeltas::notesAdded));
    this->deltas.add(new VCS::Delta({}, PatternDeltas::clipsAdded));
}

Image PianoTrackNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::pianoTrack, HEADLINE_ICON_SIZE);
}

int PianoTrackNode::getNumDeltas() const
{
    return this->deltas.size();
}

//===----------------------------------------------------------------------===//
// VCS stuff
//===----------------------------------------------------------------------===//

VCS::Delta *PianoTrackNode::getDelta(int index) const
{
    using namespace Serialization::VCS;
    if (this->deltas[index]->hasType(PianoSequenceDeltas::notesAdded))
    {
        const int numEvents = this->getSequence()->size();

        if (numEvents == 0)
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("empty sequence"));
        }
        else
        {
            this->deltas[index]->setDescription(VCS::DeltaDescription("{x} notes", numEvents));
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

ValueTree PianoTrackNode::getDeltaData(int deltaIndex) const
{
    using namespace Serialization::VCS;
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
    else if (this->deltas[deltaIndex]->hasType(PianoSequenceDeltas::notesAdded))
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

VCS::DiffLogic *PianoTrackNode::getDiffLogic() const
{
    return this->vcsDiffLogic;
}

void PianoTrackNode::resetStateTo(const VCS::TrackedItem &newState)
{
    using namespace Serialization::VCS;
    for (int i = 0; i < newState.getNumDeltas(); ++i)
    {
        const VCS::Delta *newDelta = newState.getDelta(i);
        const auto newDeltaData(newState.getDeltaData(i));
        
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
        // the current layer state is supposed to have
        // a single note delta of type PianoSequenceDeltas::notesAdded
        else if (newDelta->hasType(PianoSequenceDeltas::notesAdded))
        {
            this->resetEventsDelta(newDeltaData);
        }
        // same rule applies to clips state:
        else if (newDelta->hasType(PatternDeltas::clipsAdded))
        {
            this->resetClipsDelta(newDeltaData);
        }
    }
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree PianoTrackNode::serialize() const
{
    ValueTree tree(Serialization::Core::treeNode);

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::Core::treeNodeType, this->type, nullptr);
    tree.setProperty(Serialization::Core::treeNodeName, this->name, nullptr);

    this->serializeTrackProperties(tree);

    tree.appendChild(this->sequence->serialize(), nullptr);
    tree.appendChild(this->pattern->serialize(), nullptr);

    TreeNodeSerializer::serializeChildren(*this, tree);

    return tree;
}

void PianoTrackNode::deserialize(const ValueTree &tree)
{
    this->reset();

    this->deserializeVCSUuid(tree);
    this->deserializeTrackProperties(tree);

    forEachValueTreeChildWithType(tree, e, Serialization::Midi::track)
    {
        this->sequence->deserialize(e);
    }

    forEachValueTreeChildWithType(tree, e, Serialization::Midi::pattern)
    {
        this->pattern->deserialize(e);
    }

    // Proceed with basic properties and children
    TreeNode::deserialize(tree);
}


//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

ValueTree PianoTrackNode::serializePathDelta() const
{
    using namespace Serialization::VCS;
    ValueTree tree(MidiTrackDeltas::trackPath);
    tree.setProperty(delta, this->getTrackName(), nullptr);
    return tree;
}

ValueTree PianoTrackNode::serializeMuteDelta() const
{
    using namespace Serialization::VCS;
    ValueTree tree(MidiTrackDeltas::trackMute);
    tree.setProperty(delta, this->getTrackMuteStateAsString(), nullptr);
    return tree;
}

ValueTree PianoTrackNode::serializeColourDelta() const
{
    using namespace Serialization::VCS;
    ValueTree tree(MidiTrackDeltas::trackColour);
    tree.setProperty(delta, this->getTrackColour().toString(), nullptr);
    return tree;
}

ValueTree PianoTrackNode::serializeInstrumentDelta() const
{
    using namespace Serialization::VCS;
    ValueTree tree(MidiTrackDeltas::trackInstrument);
    tree.setProperty(delta, this->getTrackInstrumentId(), nullptr);
    return tree;
}

ValueTree PianoTrackNode::serializeEventsDelta() const
{
    ValueTree tree(Serialization::VCS::PianoSequenceDeltas::notesAdded);
    for (int i = 0; i < this->getSequence()->size(); ++i)
    {
        const MidiEvent *event = this->getSequence()->getUnchecked(i);
        tree.appendChild(event->serialize(), nullptr);
    }

    return tree;
}

void PianoTrackNode::resetPathDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackPath));
    const String &path(state.getProperty(Serialization::VCS::delta));
    this->setXPath(path, false);
}

void PianoTrackNode::resetMuteDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackMute));
    const String &muteState(state.getProperty(Serialization::VCS::delta));
    const bool willMute = MidiTrack::isTrackMuted(muteState);
    
    if (willMute != this->isTrackMuted())
    {
        this->setTrackMuted(willMute, false);
    }
}

void PianoTrackNode::resetColourDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackColour));
    const String &colourString(state.getProperty(Serialization::VCS::delta));
    const Colour &colour(Colour::fromString(colourString));

    if (colour != this->getTrackColour())
    {
        this->setTrackColour(colour, false);
    }
}

void PianoTrackNode::resetInstrumentDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackInstrument));
    const String &instrumentId(state.getProperty(Serialization::VCS::delta));
    this->setTrackInstrumentId(instrumentId, false);
}

void PianoTrackNode::resetEventsDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::PianoSequenceDeltas::notesAdded));

    this->getSequence()->reset();
    forEachValueTreeChildWithType(state, e, Serialization::Midi::note)
    {
        this->getSequence()->checkoutEvent<Note>(e);
    }

    this->getSequence()->updateBeatRange(false);
}

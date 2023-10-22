/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "AutomationTrackNode.h"
#include "AutomationSequence.h"
#include "TreeNodeSerializer.h"
#include "PatternEditorNode.h"
#include "Icons.h"
#include "Pattern.h"

AutomationTrackNode::AutomationTrackNode(const String &name) :
    MidiTrackNode(name, Serialization::Core::automationTrack)
{
    this->sequence = make<AutomationSequence>(*this, *this);
    this->pattern = make<Pattern>(*this, *this);

    this->vcsDiffLogic = make<VCS::AutomationTrackDiffLogic>(*this);

    using namespace Serialization::VCS;
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackPath));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackColour));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackChannel));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackInstrument));
    this->deltas.add(new VCS::Delta({}, MidiTrackDeltas::trackController));
    this->deltas.add(new VCS::Delta({}, AutoSequenceDeltas::eventsAdded));
    this->deltas.add(new VCS::Delta({}, PatternDeltas::clipsAdded));
    this->deltas.add(new VCS::Delta({}, TimeSignatureDeltas::timeSignaturesChanged));
}

Image AutomationTrackNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::automationTrack, Globals::UI::headlineIconSize);
}

void AutomationTrackNode::showPage()
{
    jassertfalse; // broken config? should select PatternEditorNode instead

    if (auto *parentProject = this->findParentOfType<ProjectNode>())
    {
        parentProject->showPatternEditor(
            parentProject->findChildOfType<PatternEditorNode>());
    }
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

int AutomationTrackNode::getNumDeltas() const
{
    return this->deltas.size();
}

VCS::Delta *AutomationTrackNode::getDelta(int index) const
{
    using namespace Serialization::VCS;
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

SerializedData AutomationTrackNode::getDeltaData(int deltaIndex) const
{
    using namespace Serialization::VCS;
    if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackPath))
    {
        return this->serializePathDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackColour))
    {
        return this->serializeColourDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackChannel))
    {
        return this->serializeChannelDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackInstrument))
    {
        return this->serializeInstrumentDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackController))
    {
        return this->serializeControllerDelta();
    }
    else if (this->deltas[deltaIndex]->hasType(TimeSignatureDeltas::timeSignaturesChanged))
    {
        return this->serializeTimeSignatureDelta();
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

bool AutomationTrackNode::deltaHasDefaultData(int deltaIndex) const
{
    using namespace Serialization::VCS;
    if (this->deltas[deltaIndex]->hasType(TimeSignatureDeltas::timeSignaturesChanged))
    {
        return !this->hasTimeSignatureOverride();
    }
    else if (this->deltas[deltaIndex]->hasType(MidiTrackDeltas::trackChannel))
    {
        return this->getTrackChannel() == 1;
    }

    return false;
}

VCS::DiffLogic *AutomationTrackNode::getDiffLogic() const
{
    return this->vcsDiffLogic.get();
}

void AutomationTrackNode::resetStateTo(const VCS::TrackedItem &newState)
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
        else if (newDelta->hasType(MidiTrackDeltas::trackColour))
        {
            this->resetColourDelta(newDeltaData);
        }
        else if (newDelta->hasType(MidiTrackDeltas::trackChannel))
        {
            this->resetChannelDelta(newDeltaData);
        }
        else if (newDelta->hasType(MidiTrackDeltas::trackInstrument))
        {
            this->resetInstrumentDelta(newDeltaData);
        }
        else if (newDelta->hasType(MidiTrackDeltas::trackController))
        {
            this->resetControllerDelta(newDeltaData);
        }
        else if (newDelta->hasType(TimeSignatureDeltas::timeSignaturesChanged))
        {
            this->resetTimeSignatureDelta(newDeltaData);
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

SerializedData AutomationTrackNode::serialize() const
{
    SerializedData tree(Serialization::Core::treeNode);

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::Core::treeNodeType, this->type);
    tree.setProperty(Serialization::Core::treeNodeName, this->name);

    this->serializeTrackProperties(tree);

    tree.appendChild(this->sequence->serialize());
    tree.appendChild(this->pattern->serialize());

    if (this->hasTimeSignatureOverride())
    {
        tree.appendChild(this->timeSignatureOverride.serialize());
    }

    TreeNodeSerializer::serializeChildren(*this, tree);

    return tree;
}

void AutomationTrackNode::deserialize(const SerializedData &data)
{
    this->reset();

    this->deserializeVCSUuid(data);
    this->deserializeTrackProperties(data);

    forEachChildWithType(data, e, Serialization::Midi::automation)
    {
        this->sequence->deserialize(e);
    }

    forEachChildWithType(data, e, Serialization::Midi::pattern)
    {
        this->pattern->deserialize(e);
    }

    forEachChildWithType(data, e, Serialization::Midi::timeSignature)
    {
        this->timeSignatureOverride.deserialize(e);
    }

    // Proceed with basic properties and children
    MidiTrackNode::deserialize(data);
}

//===----------------------------------------------------------------------===//
// Deltas
//===----------------------------------------------------------------------===//

SerializedData AutomationTrackNode::serializeControllerDelta() const
{
    using namespace Serialization::VCS;
    SerializedData tree(MidiTrackDeltas::trackController);
    tree.setProperty(delta, this->getTrackControllerNumber());
    return tree;
}

SerializedData AutomationTrackNode::serializeEventsDelta() const
{
    SerializedData tree(Serialization::VCS::AutoSequenceDeltas::eventsAdded);

    for (int i = 0; i < this->getSequence()->size(); ++i)
    {
        const MidiEvent *event = this->getSequence()->getUnchecked(i);
        tree.appendChild(event->serialize());
    }

    return tree;
}

void AutomationTrackNode::resetControllerDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackController));
    const int ccNumber = state.getProperty(Serialization::VCS::delta);
    this->setTrackControllerNumber(ccNumber, dontSendNotification);
}

void AutomationTrackNode::resetEventsDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::AutoSequenceDeltas::eventsAdded));
    this->getSequence()->reset();

    forEachChildWithType(state, e, Serialization::Midi::automationEvent)
    {
        this->getSequence()->checkoutEvent<AutomationEvent>(e);
    }

    this->getSequence()->updateBeatRange(false);
}

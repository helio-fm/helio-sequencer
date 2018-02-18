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
#include "Arpeggiator.h"
#include "SerializationKeys.h"
#include "PianoRollToolbox.h"
#include "Note.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"

Arpeggiator::Arpeggiator() :
    reversedMode(false),
    relativeMappingMode(true),
    limitToChordMode(false),
    scale(1.f) {}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

String Arpeggiator::getId() const
{
    return this->id.toString();
}

String Arpeggiator::getName() const
{
    return this->name;
}

bool Arpeggiator::isReversed() const
{
    return this->reversedMode;
}

bool Arpeggiator::hasRelativeMapping() const
{
    return this->relativeMappingMode;
}

bool Arpeggiator::limitsToChord() const
{
    return this->limitToChordMode;
}

bool Arpeggiator::isEmpty() const
{
    return (this->sequence.size() == 0);
}

float Arpeggiator::getScale() const
{
    return this->scale;
}

String Arpeggiator::exportSequenceAsTrack() const
{
    ValueTree sequence(Serialization::Core::track);
    
    for (int i = 0; i < this->sequence.size(); ++i)
    {
        sequence.appendChild(this->sequence.getUnchecked(i).serialize());
    }

    String doc;
    XmlSerializer serializer;
    serializer.saveToString(doc, sequence);
    return doc;
}


//===----------------------------------------------------------------------===//
// Chains
//===----------------------------------------------------------------------===//

Arpeggiator Arpeggiator::withName(const String &newName) const
{
    Arpeggiator arp(*this);
    arp.name = newName;
    return arp;
}

Arpeggiator Arpeggiator::withSequenceFromString(const String &data) const
{
    Arpeggiator arp(*this);
    const auto tree = DocumentHelpers::read<XmlSerializer>(data);
    const auto root = tree.hasType(Serialization::Core::track) ?
        tree : tree.getChildWithName(Serialization::Core::track);
    
    if (!root.isValid()) { return arp; }
    
    Array<Note> xmlPattern;
    
    forEachValueTreeChildWithType(root, e, Serialization::Core::note)
    {
        xmlPattern.add(Note().withParameters(e).copyWithNewId());
    }
    
    if (xmlPattern.size() == 0)
    {
        return arp;
    }
    
    arp.sequence = xmlPattern;
    return arp;
}

Arpeggiator Arpeggiator::withSequence(const Array<Note> &arpSequence) const
{
    Arpeggiator arp(*this);
    arp.sequence = arpSequence;
    return arp;
}

Arpeggiator Arpeggiator::reversed(bool shouldBeReversed) const
{
    Arpeggiator arp(*this);
    arp.reversedMode = shouldBeReversed;
    return arp;
}

Arpeggiator Arpeggiator::mappedRelative(bool shouldBeMappedRelative) const
{
    Arpeggiator arp(*this);
    arp.relativeMappingMode = shouldBeMappedRelative;
    return arp;
}

Arpeggiator Arpeggiator::limitedToChord(bool shouldLimitToChord) const
{
    Arpeggiator arp(*this);
    arp.limitToChordMode = shouldLimitToChord;
    return arp;
}

Arpeggiator Arpeggiator::withScale(float newScale) const
{
    Arpeggiator arp(*this);
    arp.scale = newScale;
    return arp;
}


//===----------------------------------------------------------------------===//
// Sequence parsing
//===----------------------------------------------------------------------===//

Array<Arpeggiator::Key> Arpeggiator::createArpKeys() const
{
    Array<Arpeggiator::Key> arpKeys;
    
    // todo change! get root note in sequence
    Array<Note> sortedArp(this->sequence);
    const Note &&n0 = sortedArp[0];
    sortedArp.sort(n0);
    
    const int arpRootKey = sortedArp[0].getKey();
    const float arpStartBeat = PianoRollToolbox::findStartBeat(sortedArp);
    const float arpEndBeat = PianoRollToolbox::findEndBeat(sortedArp);
    
    for (int i = 0; i < sortedArp.size(); ++i)
    {
        const Note n(sortedArp.getUnchecked(i));
        const bool keyIsBelow = (n.getKey() < arpRootKey);
        
        Arpeggiator::Key ak;
        const int upperOctavesShift = int(floorf(float(n.getKey() - arpRootKey) / 12.f) * 12);
        const int lowerOctavesShift = int(ceilf(float(arpRootKey - n.getKey()) / 12.f) * -12);
        
        ak.octaveShift = keyIsBelow ? lowerOctavesShift : upperOctavesShift;
        ak.keyIndex = ((n.getKey() - ak.octaveShift) - arpRootKey) % 12;
        ak.absStart = this->scale * (n.getBeat() - arpStartBeat) / (arpEndBeat - arpStartBeat);
        ak.absLength = this->scale * n.getLength() / (arpEndBeat - arpStartBeat);
        ak.beatStart = this->scale * (n.getBeat() - arpStartBeat);
        ak.beatLength = this->scale * n.getLength();
        ak.sequenceLength = this->scale * (arpEndBeat - arpStartBeat);
        ak.velocity = n.getVelocity();
        
        arpKeys.add(ak);
    }
    
    return arpKeys;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Arpeggiator::serialize() const
{
    ValueTree tree(Serialization::Arps::arpeggiator);
    ValueTree seq(Serialization::Arps::sequence);
    
    for (int i = 0; i < this->sequence.size(); ++i)
    {
        seq.appendChild(this->sequence.getUnchecked(i).serialize());
    }
    
    tree.appendChild(seq);
    
    tree.setProperty(Serialization::Arps::isReversed, this->reversedMode);
    tree.setProperty(Serialization::Arps::relativeMapping, this->relativeMappingMode);
    tree.setProperty(Serialization::Arps::limitsToChord, this->limitToChordMode);
    tree.setProperty(Serialization::Arps::scale, this->scale);

    tree.setProperty(Serialization::Arps::id, this->id.toString());
    tree.setProperty(Serialization::Arps::name, this->name);

    return tree;
}

void Arpeggiator::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Arps::arpeggiator) ?
        tree : tree.getChildWithName(Serialization::Arps::arpeggiator);
    
    if (!root.isValid()) { return; }
    
    const auto seq = root.getChildWithName(Serialization::Arps::sequence);
    
    Array<Note> xmlPattern;
    
    forEachValueTreeChildWithType(seq, e, Serialization::Core::note)
    {
        xmlPattern.add(Note().withParameters(e).copyWithNewId());
    }
    
    this->sequence = xmlPattern;

    this->reversedMode = tree.getProperty(Serialization::Arps::isReversed, false);
    this->relativeMappingMode = tree.getProperty(Serialization::Arps::relativeMapping, true);
    this->limitToChordMode = tree.getProperty(Serialization::Arps::limitsToChord, false);
    this->scale = float(tree.getProperty(Serialization::Arps::scale, 1.f));
    
    this->id = tree.getProperty(Serialization::Arps::id, this->getId());
    this->name = tree.getProperty(Serialization::Arps::name, this->getName());
}

void Arpeggiator::reset()
{
    this->sequence.clear();
    this->scale = 1.f;
    this->reversedMode = false;
    this->relativeMappingMode = true;
    this->limitToChordMode = false;
}

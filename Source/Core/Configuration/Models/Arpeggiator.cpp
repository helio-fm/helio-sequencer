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

// A scenario from user's point:
// 1. User creates a sequence of notes strictly within a certain scale,
// 2. User selects `create arpeggiator from sequence`
// 3. App checks that the entire sequence is within single scale and adds arp model
// 4. User select a number of chords and clicks `arpeggiate`
// 5. User selects arpeggiator fro arp panel (TODO where is arp panel?)
// 

// This one assumes it has a diatonic scale, and target chord has up to 4 notes.
// There are no restrictions though on a target chord's scale or where chord's keys are.
// Arp keys are mapped like this:
// Arp key                  Chord's note
// 1                        1st chord note (the lowest)
// 2                        1st note + 1 (in a chord's scale)
// 3                        2nd note
// 4                        2nd note + 1
// 5                        3rd note
// 6                        3rd note + 1
// 7                        4th note, if any (wrap back to 1st, if only 3 notes)
// 8                        1st note + period
// etc, so basically it tries to treat target chord as a triadic chord (but any other might do as well).

class DiatonicArpMapper final : public Arp::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arp::Key arpKey, const Array<Note> &chord,
        const Scale &chordScale, Note::Key chordRoot) const override
    {
        const auto periodOffset = arpKey.period * chordScale.getBasePeriod();
        switch (arpKey.key)
        {
        case 0: return periodOffset + this->getChordKey(chord, 0);
        case 1: return periodOffset + this->getChordKeyPlus(chord, chordScale, chordRoot, 0, 1);
        case 2: return periodOffset + this->getChordKey(chord, 1);
        case 3: return periodOffset + this->getChordKeyPlus(chord, chordScale, chordRoot, 1, 1);
        case 4: return periodOffset + this->getChordKey(chord, 2);
        case 5: return periodOffset + this->getChordKeyPlus(chord, chordScale, chordRoot, 2, 1);
        case 6: return periodOffset + this->getChordKey(chord, 3);
        default: return periodOffset + chordRoot;
        }
    }
};

// Similar as above:

class PentatonicArpMapper final : public Arp::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arp::Key arpKey, const Array<Note> &chord,
        const Scale &chordScale, Note::Key chordRoot) const override
    {
        const auto periodOffset = arpKey.period * chordScale.getBasePeriod();
        switch (arpKey.key)
        {
        case 0: return periodOffset + this->getChordKey(chord, 0);
        case 1: return periodOffset + this->getChordKeyPlus(chord, chordScale, chordRoot, 0, 1);
        case 2: return periodOffset + this->getChordKey(chord, 1);
        case 3: return periodOffset + this->getChordKey(chord, 2);
        case 4: return periodOffset + this->getChordKeyPlus(chord, chordScale, chordRoot, 2, 1);
        default: return periodOffset + chordRoot;
        }
    }
};

class SimpleTriadicArpMapper final : public Arp::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arp::Key arpKey, const Array<Note> &chord,
        const Scale &chordScale, Note::Key chordRoot) const override
    {
        const int periodOffset = arpKey.period * chordScale.getBasePeriod();
        return periodOffset + this->getChordKey(chord, arpKey.key);
    }
};

class FallbackArpMapper final : public Arp::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arp::Key arpKey, const Array<Note> &chord,
        const Scale &chordScale, Note::Key chordRoot) const override
    {
        jassertfalse; // Should never hit this point
        return chordRoot;
    }
};

static ScopedPointer<Arp::Mapper> createMapperOfType(const Identifier &id)
{
    using namespace Serialization::Arps;
    if (id == Type::simpleTriadic)      { return new SimpleTriadicArpMapper(); }
    else if (id == Type::pentatonic)    { return new PentatonicArpMapper(); }
    else if (id == Type::diatonic)      { return new DiatonicArpMapper(); }
    
    return new FallbackArpMapper();
}

Arp::Arp(const String &name, const Scale &scale, const Array<Note> &sequence, Note::Key rootKey)
{
    auto sequenceStartBeat = FLT_MAX;
    for (const auto &note : sequence)
    {
        sequenceStartBeat = jmin(sequenceStartBeat, note.getBeat());
    }

    for (const auto &note : sequence)
    {
        const auto relativeChromaticKey = note.getKey() - rootKey;

        // Scale key will be limited to a single period
        const auto scaleKey = scale.getScaleKey(relativeChromaticKey);
        const auto period = relativeChromaticKey % scale.getBasePeriod();
        const auto beat = note.getBeat() - sequenceStartBeat;

        // Ignore all non-scale keys (will be -1 if chromatic key is not in a target scale)
        if (scaleKey >= 0)
        {
            this->keys.add({ scaleKey, period, note.getVelocity(), beat, note.getLength() });
        }
    }

    // Try do deduct mapper type, depending on arp's scale size
    // (TODO add more mappers and scales in future)
    using namespace Serialization::Arps;

    switch (scale.getSize())
    {
    case 5:
        this->type = Type::pentatonic;
        break;
    case 7:
        this->type = Type::diatonic;
        break;
    default:
        this->type = Type::simpleTriadic;
        break;
    }

    this->mapper = createMapperOfType(this->type);
    jassert(this->mapper);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Arp::serialize() const
{
    ValueTree tree(Serialization::Arps::arpeggiator);

    tree.setProperty(Serialization::Arps::id, this->id.toString(), nullptr);
    tree.setProperty(Serialization::Arps::name, this->name, nullptr);

    ValueTree seq(Serialization::Arps::sequence);
    for (int i = 0; i < this->keys.size(); ++i)
    {
        // seq.appendChild(this->keys.getUnchecked(i).serialize(), nullptr);
    }

    tree.appendChild(seq, nullptr);

    return tree;
}

void Arp::deserialize(const ValueTree &tree)
{

}

void Arp::reset()
{

}

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
    ValueTree sequence(Serialization::Midi::track);
    
    for (int i = 0; i < this->sequence.size(); ++i)
    {
        sequence.appendChild(this->sequence.getUnchecked(i).serialize(), nullptr);
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
    const auto root = tree.hasType(Serialization::Midi::track) ?
        tree : tree.getChildWithName(Serialization::Midi::track);
    
    if (!root.isValid()) { return arp; }
    
    Array<Note> xmlPattern;
    
    forEachValueTreeChildWithType(root, e, Serialization::Midi::note)
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
    const auto &sorter = sortedArp.getFirst();
    sortedArp.sort(sorter);
    
    const int arpRootKey = sortedArp.getFirst().getKey();
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
        seq.appendChild(this->sequence.getUnchecked(i).serialize(), nullptr);
    }
    
    tree.appendChild(seq, nullptr);
    
    tree.setProperty(Serialization::Arps::isReversed, this->reversedMode, nullptr);
    tree.setProperty(Serialization::Arps::relativeMapping, this->relativeMappingMode, nullptr);
    tree.setProperty(Serialization::Arps::limitsToChord, this->limitToChordMode, nullptr);
    //tree.setProperty(Serialization::Arps::scale, this->scale, nullptr);

    tree.setProperty(Serialization::Arps::id, this->id.toString(), nullptr);
    tree.setProperty(Serialization::Arps::name, this->name, nullptr);

    return tree;
}

void Arpeggiator::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Arps::arpeggiator) ?
        tree : tree.getChildWithName(Serialization::Arps::arpeggiator);
    
    if (!root.isValid()) { return; }
    
    const auto seq = root.getChildWithName(Serialization::Arps::sequence);
    
    Array<Note> xmlPattern;
    
    forEachValueTreeChildWithType(seq, e, Serialization::Midi::note)
    {
        xmlPattern.add(Note().withParameters(e).copyWithNewId());
    }
    
    this->sequence = xmlPattern;

    this->reversedMode = tree.getProperty(Serialization::Arps::isReversed, false);
    this->relativeMappingMode = tree.getProperty(Serialization::Arps::relativeMapping, true);
    this->limitToChordMode = tree.getProperty(Serialization::Arps::limitsToChord, false);
    //this->scale = float(tree.getProperty(Serialization::Arps::scale, 1.f));
    
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

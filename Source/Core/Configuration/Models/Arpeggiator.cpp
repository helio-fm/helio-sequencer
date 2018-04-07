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
// Arpeggiator keys are mapped like this:
// Arpeggiator key                  Chord's note
// 1                        1st chord note (the lowest)
// 2                        1st note + 1 (in a chord's scale)
// 3                        2nd note
// 4                        2nd note + 1
// 5                        3rd note
// 6                        3rd note + 1
// 7                        4th note, if any (wrap back to 1st, if only 3 notes)
// 8                        1st note + period
// etc, so basically it tries to treat target chord as a triadic chord (but any other might do as well).

class DiatonicArpMapper final : public Arpeggiator::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arpeggiator::Key arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key chordRoot) const override
    {
        const auto periodOffset = arpKey.period * chordScale->getBasePeriod();
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

class PentatonicArpMapper final : public Arpeggiator::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arpeggiator::Key arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key chordRoot) const override
    {
        const auto periodOffset = arpKey.period * chordScale->getBasePeriod();
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

class SimpleTriadicArpMapper final : public Arpeggiator::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arpeggiator::Key arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key chordRoot) const override
    {
        const int periodOffset = arpKey.period * chordScale->getBasePeriod();
        return periodOffset + this->getChordKey(chord, arpKey.key);
    }
};

class FallbackArpMapper final : public Arpeggiator::Mapper
{
    Note::Key mapArpKeyIntoChordSpace(Arpeggiator::Key arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key chordRoot) const override
    {
        jassertfalse; // Should never hit this point
        return chordRoot;
    }
};

static ScopedPointer<Arpeggiator::Mapper> createMapperOfType(const Identifier &id)
{
    using namespace Serialization::Arps;
    if (id == Types::simpleTriadic)      { return new SimpleTriadicArpMapper(); }
    else if (id == Types::pentatonic)    { return new PentatonicArpMapper(); }
    else if (id == Types::diatonic)      { return new DiatonicArpMapper(); }
    
    return new FallbackArpMapper();
}

Arpeggiator &Arpeggiator::operator=(const Arpeggiator &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->name = other.name;
    this->type = other.type;
    this->keys = other.keys;
    this->mapper = createMapperOfType(this->type);
    return *this;
}

bool operator==(const Arpeggiator &l, const Arpeggiator &r)
{
    return &l == &r || l.name == r.name;
}

Arpeggiator::Arpeggiator(const String &name, const Scale &scale, const Array<Note> &sequence, Note::Key rootKey)
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
        this->type = Types::pentatonic;
        break;
    case 7:
        this->type = Types::diatonic;
        break;
    default:
        this->type = Types::simpleTriadic;
        break;
    }

    this->mapper = createMapperOfType(this->type);
    jassert(this->mapper);
}

float Arpeggiator::getSequenceLength() const
{
    float length = 0.f;
    for (const auto &key : this->keys)
    {
        length = jmax(length, key.beat + key.length);
    }
    return length;
}

int Arpeggiator::getNumKeys() const noexcept
{
    return this->keys.size();
}

float Arpeggiator::getBeatFor(int arpKeyIndex) const noexcept
{
    jassert(this->keys.size() > 0);
    const int safeKeyIndex = arpKeyIndex % this->getNumKeys();
    return this->keys.getUnchecked(safeKeyIndex).beat;
}

Note Arpeggiator::mapArpKeyIntoChordSpace(int arpKeyIndex, float startBeat,
    const Array<Note> &chord, const Scale::Ptr chordScale, Note::Key chordRoot) const
{
    jassert(chord.size() > 0);
    jassert(this->keys.size() > 0);

    const int safeKeyIndex = arpKeyIndex % this->getNumKeys();
    const auto arpKey = this->keys.getUnchecked(safeKeyIndex);
    const auto newNoteKey =
        this->mapper->mapArpKeyIntoChordSpace(arpKey,
            chord, chordScale, chordRoot);

    return chord.getFirst().
        withKeyBeat(newNoteKey, startBeat + arpKey.beat).
        withLength(arpKey.length).
        withVelocity(arpKey.velocity). // TODO some randomness?
        copyWithNewId();
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Arpeggiator::getResourceId() const
{
    // Assumed to be unique:
    return this->name;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Arpeggiator::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Arps::arpeggiator);

    tree.setProperty(Arps::name, this->name, nullptr);
    tree.setProperty(Arps::type, this->type.toString(), nullptr);

    ValueTree seq(Arps::sequence);
    for (int i = 0; i < this->keys.size(); ++i)
    {
        seq.appendChild(this->keys.getUnchecked(i).serialize(), nullptr);
    }

    tree.appendChild(seq, nullptr);

    return tree;
}

void Arpeggiator::deserialize(const ValueTree &tree)
{
    using namespace Serialization;

    const auto root = tree.hasType(Arps::arpeggiator) ?
        tree : tree.getChildWithName(Arps::arpeggiator);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Arps::name);
    this->type = root.getProperty(Arps::type).toString();
    this->mapper = createMapperOfType(this->type);

    forEachValueTreeChildWithType(root, keyNode, Arps::key)
    {
        Arpeggiator::Key key;
        key.deserialize(keyNode);
        this->keys.add(key);
    }
}

void Arpeggiator::reset()
{
    this->name.clear();
    this->keys.clearQuick();
}

ValueTree Arpeggiator::Key::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Arps::key);
    tree.setProperty(Arps::Keys::key, this->key, nullptr);
    tree.setProperty(Arps::Keys::period, this->period, nullptr);
    tree.setProperty(Arps::Keys::timestamp, roundToInt(this->beat * TICKS_PER_BEAT), nullptr);
    tree.setProperty(Arps::Keys::length, roundToInt(this->length * TICKS_PER_BEAT), nullptr);
    tree.setProperty(Arps::Keys::volume, roundToInt(this->velocity * VELOCITY_SAVE_ACCURACY), nullptr);
    return tree;
}

void Arpeggiator::Key::deserialize(const ValueTree &tree)
{
    using namespace Serialization;
    this->key = tree.getProperty(Arps::Keys::key);
    this->beat = float(tree.getProperty(Arps::Keys::timestamp)) / TICKS_PER_BEAT;
    this->length = float(tree.getProperty(Arps::Keys::length)) / TICKS_PER_BEAT;
    const auto vol = float(tree.getProperty(Arps::Keys::volume)) / VELOCITY_SAVE_ACCURACY;
    this->velocity = jmax(jmin(vol, 1.f), 0.f);
}

void Arpeggiator::Key::reset() {}

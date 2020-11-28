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
#include "SequencerOperations.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Mapper common
//===----------------------------------------------------------------------===//

Note::Key Arpeggiator::Mapper::getChordKey(const Array<Note> &chord, int chordKeyIndex,
    const Scale::Ptr chordScale, Note::Key chordRoot, int scaleOffset) const
{
    const int safeIndex = chordKeyIndex % chord.size();
    const int chordKey = chord.getUnchecked(safeIndex).getKey();
    const int chordKeyNoRootOffset = chordKey - chordRoot;
    jassert(chordKeyNoRootOffset >= 0);
    const int targetScaleKey = chordScale->getScaleKey(chordKeyNoRootOffset) + scaleOffset;
    return (targetScaleKey >= 0) ?
        chordScale->getChromaticKey(targetScaleKey, 0, false) + chordRoot :
        chordKey; // a non-scale key is found in the chord, so just fallback to that key
}

float Arpeggiator::Mapper::getChordVelocity(const Array<Note> &chord, int chordKeyIndex) const
{
    const int safeIndex = chordKeyIndex % chord.size();
    return chord.getUnchecked(safeIndex).getVelocity();
}

//===----------------------------------------------------------------------===//
// Diatonic mapper
//===----------------------------------------------------------------------===//

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
    Note::Key mapArpKeyIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key absChordRoot, int scaleOffset) const override
    {
        const auto periodOffset = arpKey.period * chordScale->getBasePeriod();
        switch (arpKey.key)
        {
        case 0: return periodOffset + this->getChordKey(chord, 0, chordScale, absChordRoot, scaleOffset);
        case 1: return periodOffset + this->getChordKey(chord, 0, chordScale, absChordRoot, scaleOffset + 1);
        case 2: return periodOffset + this->getChordKey(chord, 1, chordScale, absChordRoot, scaleOffset);
        case 3: return periodOffset + this->getChordKey(chord, 1, chordScale, absChordRoot, scaleOffset + 1);
        case 4: return periodOffset + this->getChordKey(chord, 2, chordScale, absChordRoot, scaleOffset);
        case 5: return periodOffset + this->getChordKey(chord, 2, chordScale, absChordRoot, scaleOffset + 1);
        case 6: return (chord.size() <= 3) ?
            periodOffset + this->getChordKey(chord, 2, chordScale, absChordRoot, 2) :
            periodOffset + this->getChordKey(chord, 3, chordScale, absChordRoot, 0);
        default: return periodOffset + absChordRoot;
        }
    }

    float mapArpVelocityIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord) const override
    {
        switch (arpKey.key)
        {
        case 0: return (arpKey.velocity + this->getChordVelocity(chord, 0)) / 2.f;
        case 1: return (arpKey.velocity + this->getChordVelocity(chord, 0)) / 2.f;
        case 2: return (arpKey.velocity + this->getChordVelocity(chord, 1)) / 2.f;
        case 3: return (arpKey.velocity + this->getChordVelocity(chord, 1)) / 2.f;
        case 4: return (arpKey.velocity + this->getChordVelocity(chord, 2)) / 2.f;
        case 5: return (arpKey.velocity + this->getChordVelocity(chord, 2)) / 2.f;
        case 6: return (chord.size() <= 3) ? 
            (arpKey.velocity + this->getChordVelocity(chord, 2)) / 2.f :
            (arpKey.velocity + this->getChordVelocity(chord, 3)) / 2.f;
        default: return arpKey.velocity;
        }
    }
};

//===----------------------------------------------------------------------===//
// Pentatonic mapper, similar to the one above
//===----------------------------------------------------------------------===//

class PentatonicArpMapper final : public Arpeggiator::Mapper
{
    Note::Key mapArpKeyIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key absChordRoot, int scaleOffset) const override
    {
        const auto periodOffset = arpKey.period * chordScale->getBasePeriod();
        switch (arpKey.key)
        {
        case 0: return periodOffset + this->getChordKey(chord, 0, chordScale, absChordRoot, scaleOffset);
        case 1: return periodOffset + this->getChordKey(chord, 0, chordScale, absChordRoot, scaleOffset + 1);
        case 2: return periodOffset + this->getChordKey(chord, 1, chordScale, absChordRoot, scaleOffset);
        case 3: return periodOffset + this->getChordKey(chord, 2, chordScale, absChordRoot, scaleOffset);
        case 4: return periodOffset + this->getChordKey(chord, 2, chordScale, absChordRoot, scaleOffset + 1);
        default: return periodOffset + absChordRoot;
        }
    }

    float mapArpVelocityIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord) const override
    {
        switch (arpKey.key)
        {
        case 0: return (arpKey.velocity + this->getChordVelocity(chord, 0)) / 2.f;
        case 1: return (arpKey.velocity + this->getChordVelocity(chord, 0)) / 2.f;
        case 2: return (arpKey.velocity + this->getChordVelocity(chord, 1)) / 2.f;
        case 3: return (arpKey.velocity + this->getChordVelocity(chord, 2)) / 2.f;
        case 4: return (arpKey.velocity + this->getChordVelocity(chord, 2)) / 2.f;
        default: return arpKey.velocity;
        }
    }
};

//===----------------------------------------------------------------------===//
// Simple mappers
//===----------------------------------------------------------------------===//

class SimpleTriadicArpMapper final : public Arpeggiator::Mapper
{
    Note::Key mapArpKeyIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key absChordRoot, int scaleOffset) const override
    {
        const int periodOffset = arpKey.period * chordScale->getBasePeriod();
        return periodOffset + this->getChordKey(chord, arpKey.key, chordScale, absChordRoot, scaleOffset);
    }

    float mapArpVelocityIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord) const override
    {
        return (arpKey.velocity + this->getChordVelocity(chord, arpKey.key)) / 2.f;
    }
};

class FallbackArpMapper final : public Arpeggiator::Mapper
{
    Note::Key mapArpKeyIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord,
        const Scale::Ptr chordScale, Note::Key absChordRoot, int scaleOffset) const override
    {
        jassertfalse; // Should never hit this point
        return absChordRoot;
    }

    float mapArpVelocityIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord) const override
    {
        return arpKey.velocity;
    }
};

//===----------------------------------------------------------------------===//
// Arpeggiator
//===----------------------------------------------------------------------===//

static UniquePointer<Arpeggiator::Mapper> createMapperOfType(const Identifier &id)
{
    using namespace Serialization::Arps;

    if (id == Types::simpleTriadic)      
    {
        return make<SimpleTriadicArpMapper>();
    }
    else if (id == Types::pentatonic)
    {
        return make<PentatonicArpMapper>();
    }
    else if (id == Types::diatonic)
    {
        return make<DiatonicArpMapper>();
    }
    
    return make<FallbackArpMapper>();
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

Arpeggiator::Arpeggiator(const String &name, const Temperament::Ptr temperament,
    const Scale::Ptr scale, const Array<Note> &sequence, Note::Key rootKey)
{
    auto sequenceMeanKey = 0;
    auto sequenceStartBeat = FLT_MAX;
    for (const auto &note : sequence)
    {
        sequenceStartBeat = jmin(sequenceStartBeat, note.getBeat());
        sequenceMeanKey += note.getKey();
    }

    sequenceMeanKey /= sequence.size();
    const auto absRootKey = SequencerOperations::findAbsoluteRootKey(temperament, rootKey, sequenceMeanKey);

    static Key sorter;
    for (const auto &note : sequence)
    {
        const auto relativeChromaticKey = note.getKey() - absRootKey;

        // Scale key will be limited to a single period
        const auto scaleKey = scale->getScaleKey(relativeChromaticKey);
        const auto period = (relativeChromaticKey < 0) ?
            relativeChromaticKey / scale->getBasePeriod() - 1 :
            relativeChromaticKey / scale->getBasePeriod();

        const auto beat = note.getBeat() - sequenceStartBeat;

        // Ignore all non-scale keys (will be -1 if chromatic key is not in a target scale)
        if (scaleKey >= 0)
        {
            this->keys.addSorted(sorter, Key(scaleKey, period, beat, note.getLength(), note.getVelocity()));
        }
    }

    this->name = name;

    // Try do deduct mapper type, depending on arp's scale size
    // (TODO add more mappers and scales in future)
    using namespace Serialization::Arps;

    switch (scale->getSize())
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

bool Arpeggiator::isKeyIndexValid(int index) const noexcept
{
    return index >= 0 && index < this->getNumKeys();
}

float Arpeggiator::getBeatFor(int arpKeyIndex) const noexcept
{
    jassert(this->keys.size() > 0);
    const int safeKeyIndex = arpKeyIndex % this->getNumKeys();
    return this->keys.getUnchecked(safeKeyIndex).beat;
}

Note Arpeggiator::mapArpKeyIntoChordSpace(const Temperament::Ptr temperament,
    int arpKeyIndex, float startBeat,
    const Array<Note> &chord, const Scale::Ptr chordScale, Note::Key chordRoot,
    bool reversed, float durationMultiplier, float randomness) const
{
    jassert(chord.size() > 0);
    jassert(this->keys.size() > 0);

    const auto safeKeyIndex = arpKeyIndex % this->getNumKeys();

    const auto arpKeyIndexOrReversed = reversed ? this->getNumKeys() - arpKeyIndex - 1 : arpKeyIndex;
    const auto safeKeyIndexOrReversed = arpKeyIndexOrReversed % this->getNumKeys();

    const auto arpKey = this->keys.getUnchecked(safeKeyIndex);
    const auto arpKeyOrReversed = this->keys.getUnchecked(safeKeyIndexOrReversed);

    const auto absChordRoot = SequencerOperations::findAbsoluteRootKey(temperament,
        chordRoot, chord.getUnchecked(0).getKey());

    static Random rng; // add -1, 0 or 1 scale offset randomly:
    const auto randomScaleOffset = int((rng.nextFloat() * randomness * 2.f) - 1.f);

    const auto newNoteKey =
        this->mapper->mapArpKeyIntoChord(arpKeyOrReversed,
            chord, chordScale, absChordRoot, randomScaleOffset);

    const auto newNoteVelocity =
        jlimit(0.f, 1.f, this->mapper->mapArpVelocityIntoChord(arpKeyOrReversed, chord)
            + (rng.nextFloat() * randomness * 0.2f));

    return chord.getFirst()
        .withKeyBeat(newNoteKey, startBeat + (arpKey.beat * durationMultiplier))
        .withLength(arpKey.length * durationMultiplier)
        .withVelocity(newNoteVelocity)
        .copyWithNewId();
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Arpeggiator::getResourceId() const noexcept
{
    // Assumed to be unique:
    return this->name;
}

Identifier Arpeggiator::getResourceType() const noexcept
{
    return Serialization::Resources::arpeggiators;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Arpeggiator::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Arps::arpeggiator);

    tree.setProperty(Arps::name, this->name);
    tree.setProperty(Arps::type, this->type.toString());

    SerializedData seq(Arps::sequence);
    for (int i = 0; i < this->keys.size(); ++i)
    {
        seq.appendChild(this->keys.getUnchecked(i).serialize());
    }

    tree.appendChild(seq);

    return tree;
}

void Arpeggiator::deserialize(const SerializedData &data)
{
    using namespace Serialization;

    const auto root = data.hasType(Arps::arpeggiator) ?
        data : data.getChildWithName(Arps::arpeggiator);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Arps::name);
    this->type = root.getProperty(Arps::type).toString();
    this->mapper = createMapperOfType(this->type);

    const auto sequence = root.getChildWithName(Arps::sequence);
    forEachChildWithType(sequence, keyNode, Arps::key)
    {
        Arpeggiator::Key key;
        key.deserialize(keyNode);
        this->keys.addSorted(key, key);
    }
}

void Arpeggiator::reset()
{
    this->name.clear();
    this->keys.clearQuick();
}

SerializedData Arpeggiator::Key::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Arps::key);
    tree.setProperty(Arps::Keys::key, this->key);
    tree.setProperty(Arps::Keys::period, this->period);
    tree.setProperty(Arps::Keys::timestamp, int(this->beat * Globals::ticksPerBeat));
    tree.setProperty(Arps::Keys::length, int(this->length * Globals::ticksPerBeat));
    tree.setProperty(Arps::Keys::volume, int(this->velocity * Globals::velocitySaveResolution));
    return tree;
}

void Arpeggiator::Key::deserialize(const SerializedData &data)
{
    using namespace Serialization;
    this->key = data.getProperty(Arps::Keys::key);
    this->period = data.getProperty(Arps::Keys::period);
    this->beat = float(data.getProperty(Arps::Keys::timestamp)) / Globals::ticksPerBeat;
    this->length = float(data.getProperty(Arps::Keys::length)) / Globals::ticksPerBeat;
    const auto vol = float(data.getProperty(Arps::Keys::volume)) / Globals::velocitySaveResolution;
    this->velocity = jmax(jmin(vol, 1.f), 0.f);
}

void Arpeggiator::Key::reset() {}

int Arpeggiator::Key::compareElements(const Key &first, const Key &second) noexcept
{
    const float beatDiff = first.beat - second.beat;
    const int beatResult = (beatDiff > 0.f) - (beatDiff < 0.f);
    if (beatResult != 0) { return beatResult; }

    const int keyDiff = first.key - second.key;
    const int keyResult = (keyDiff > 0) - (keyDiff < 0);
    return keyResult;
}

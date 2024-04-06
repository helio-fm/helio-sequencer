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
#include "Arpeggiator.h"
#include "SequencerOperations.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Diatonic mapper
//===----------------------------------------------------------------------===//

Note::Key Arpeggiator::DiatonicMapper::getChordKey(const Array<Note> &chord, int chordKeyIndex,
    const Scale::Ptr scale, Note::Key scaleRootKeyModuloPeriod, int scaleOffset) const
{
    const int safeIndex = chordKeyIndex % chord.size();
    const int absChordKey = chord.getUnchecked(safeIndex).getKey();

    // tech debt warning: duplicates calculations
    // in SequencerOperations::shiftInScaleKeyRelative
    const auto period = (absChordKey - scaleRootKeyModuloPeriod) / scale->getBasePeriod();
    const auto periodOffset = period * scale->getBasePeriod();
    const auto chromaticOffset = (absChordKey - scaleRootKeyModuloPeriod) % scale->getBasePeriod();

    int targetScaleKey = scale->getScaleKey(chromaticOffset);
    if (targetScaleKey < 0 && scaleOffset != 0)
    {
        // the chord's key is out of scale, but we'll need to step up/down
        targetScaleKey = scale->getNearestScaleKey(chromaticOffset);
    }

    if (targetScaleKey >= 0)
    {
        return periodOffset + scaleRootKeyModuloPeriod +
            scale->getChromaticKey(targetScaleKey + scaleOffset, 0, false);
    }

    // just fallback to the raw out-of-scale key
    return absChordKey;
}

float Arpeggiator::DiatonicMapper::getChordVelocity(const Array<Note> &chord, int chordKeyIndex) const
{
    const int safeIndex = chordKeyIndex % chord.size();
    return chord.getUnchecked(safeIndex).getVelocity();
}

// Arpeggiator keys are mapped like this:
// Arpeggiator key          Chord's note
// 1                        1st chord note (the lowest)
// 2                        1st note + 1 step in current scale
// 3                        2nd note
// 4                        2nd note + 1 step in current scale
// 5                        3rd note
// 6                        3rd note + 1 step in current scale
// 7                        4th note, if any, or wraps back to 1st
// 8                        1st note + period
// etc, so basically the target chord is treated as a diatonic triad

Note::Key Arpeggiator::DiatonicMapper::mapArpKeyIntoChord(const Arpeggiator::Key &arpKey,
    const Array<Note> &chord, const Scale::Ptr chordScale, Note::Key scaleRootKey,
    int scaleOffset) const
{
    const auto periodOffset = arpKey.period * chordScale->getBasePeriod();
    switch (arpKey.key)
    {
    case 0: return periodOffset + this->getChordKey(chord, 0, chordScale, scaleRootKey, scaleOffset);
    case 1: return periodOffset + this->getChordKey(chord, 0, chordScale, scaleRootKey, scaleOffset + 1);
    case 2: return periodOffset + this->getChordKey(chord, 1, chordScale, scaleRootKey, scaleOffset);
    case 3: return periodOffset + this->getChordKey(chord, 1, chordScale, scaleRootKey, scaleOffset + 1);
    case 4: return periodOffset + this->getChordKey(chord, 2, chordScale, scaleRootKey, scaleOffset);
    case 5: return periodOffset + this->getChordKey(chord, 2, chordScale, scaleRootKey, scaleOffset + 1);
    case 6: return (chord.size() <= 3) ?
        periodOffset + this->getChordKey(chord, 2, chordScale, scaleRootKey, 2) :
        periodOffset + this->getChordKey(chord, 3, chordScale, scaleRootKey, 0);
    default: jassertfalse; return periodOffset + scaleRootKey;
    }
}

float Arpeggiator::DiatonicMapper::mapArpVelocityIntoChord(const Arpeggiator::Key &arpKey, const Array<Note> &chord) const
{
    switch (arpKey.key)
    {
    case 0: return arpKey.velocity * 0.75f + this->getChordVelocity(chord, 0) * 0.25f;
    case 1: return arpKey.velocity * 0.75f + this->getChordVelocity(chord, 0) * 0.25f;
    case 2: return arpKey.velocity * 0.75f + this->getChordVelocity(chord, 1) * 0.25f;
    case 3: return arpKey.velocity * 0.75f + this->getChordVelocity(chord, 1) * 0.25f;
    case 4: return arpKey.velocity * 0.75f + this->getChordVelocity(chord, 2) * 0.25f;
    case 5: return arpKey.velocity * 0.75f + this->getChordVelocity(chord, 2) * 0.25f;
    case 6: return (chord.size() <= 3) ? 
        arpKey.velocity * 0.75f + this->getChordVelocity(chord, 2) * 0.25f :
        arpKey.velocity * 0.75f + this->getChordVelocity(chord, 3) * 0.25f;
    default: jassertfalse; return arpKey.velocity;
    }
}

//===----------------------------------------------------------------------===//
// Arpeggiator
//===----------------------------------------------------------------------===//

Arpeggiator &Arpeggiator::operator=(const Arpeggiator &other)
{
    if (this == &other)
    {
        return *this;
    }

    this->name = other.name;
    this->keys = other.keys;
    return *this;
}

bool operator==(const Arpeggiator &l, const Arpeggiator &r)
{
    return &l == &r ||
        (l.name == r.name && l.keys == r.keys);
}

bool operator==(const Arpeggiator::Key &l, const Arpeggiator::Key &r)
{
    return l.key == r.key &&
        l.period == r.period &&
        l.beat == r.beat &&
        l.length == r.length &&
        l.velocity == r.velocity &&
        l.isBarStart == r.isBarStart;
}

Arpeggiator::Arpeggiator(const String &name, Array<Key> &&keys) :
    name(name), keys(keys) {}

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

void Arpeggiator::proceedToNextKey(int &arpKeyIndex, float &arpBeatOffset) const
{
    arpKeyIndex++;

    if (!this->isKeyIndexValid(arpKeyIndex))
    {
        arpKeyIndex = 0;
        arpBeatOffset += this->getSequenceLength();
    }
}

void Arpeggiator::skipToBarStart(int &arpKeyIndex, float &arpBeatOffset) const
{
    jassert(this->keys.size() > 0);
    jassert(this->isKeyIndexValid(arpKeyIndex));

    int safeKeyIndex = arpKeyIndex % this->getNumKeys();
    const auto oldBeat = this->keys.getUnchecked(safeKeyIndex).beat;

    while (!this->keys.getUnchecked(safeKeyIndex).isBarStart)
    {
        safeKeyIndex = (safeKeyIndex + 1) % this->getNumKeys();
        if (safeKeyIndex == arpKeyIndex)
        {
            jassertfalse; // went full circle and didn't find a bar flag, why?
            break;
        }
    };

    arpKeyIndex = safeKeyIndex;

    // update arpBeatOffset in a way that the note at new arpKeyIndex
    // will be placed in the same position as the note at old arpKeyIndex
    const auto beatDelta = this->keys.getUnchecked(safeKeyIndex).beat - oldBeat;
    arpBeatOffset -= beatDelta;
}

Note Arpeggiator::mapArpKeyIntoChordSpace(const Temperament::Ptr temperament,
    int arpKeyIndex, float startBeat, const Array<Note> &chord,
    const Scale::Ptr chordScale, Note::Key scaleRootKeyModuloPeriod,
    bool reversed, float durationMultiplier, float randomness) const
{
    jassert(chord.size() > 0);
    jassert(this->keys.size() > 0);

    const auto safeKeyIndex = arpKeyIndex % this->getNumKeys();

    const auto arpKeyIndexOrReversed = reversed ? this->getNumKeys() - arpKeyIndex - 1 : arpKeyIndex;
    const auto safeKeyIndexOrReversed = arpKeyIndexOrReversed % this->getNumKeys();

    const auto arpKey = this->keys.getUnchecked(safeKeyIndex);
    const auto arpKeyOrReversed = this->keys.getUnchecked(safeKeyIndexOrReversed);
    
    // randomly add -1/0/1 scale offset with a random chance:
    static Random rng;
    const auto randomScaleOffset =
        (!arpKey.isBarStart && rng.nextFloat() < randomness) ?
        roundToIntAccurate((rng.nextFloat() * 2.f) - 1.f) : 0;

    const auto newNoteKey =
        this->mapper.mapArpKeyIntoChord(arpKeyOrReversed,
            chord, chordScale, scaleRootKeyModuloPeriod, randomScaleOffset);

    const auto newNoteVelocity = jlimit(0.f, 1.f,
        this->mapper.mapArpVelocityIntoChord(arpKeyOrReversed, chord) +
            (rng.nextFloat() * randomness * 0.2f));

    return chord.getFirst()
        .withKeyBeat(newNoteKey, startBeat + (arpKey.beat * durationMultiplier))
        .withLength(arpKey.length * durationMultiplier)
        .withVelocity(newNoteVelocity)
        .withNewId();
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

    const auto sequence = root.getChildWithName(Arps::sequence);
    jassert(sequence.getNumChildren() > 0);

    bool isNewFormat = false;
    forEachChildWithType(sequence, keyNode, Arps::key)
    {
        Arpeggiator::Key key;
        key.deserialize(keyNode);
        // found at least one bar flag == seems to be in new format
        isNewFormat = isNewFormat || key.isBarStart;
        this->keys.addSorted(key, move(key));
    }

    if (!isNewFormat)
    {
        // for the old arps fill the isBarStart flag using 2/4 time
        for (int i = 0; i < this->keys.size(); ++i)
        {
            auto &key = this->keys.getReference(i);
            key.isBarStart = fmodf(key.beat, float(Globals::beatsPerBar / 2)) == 0.f;
        }
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
    if (this->isBarStart)
    {
        tree.setProperty(Arps::Keys::isBarStart, true);
    }

    return tree;
}

void Arpeggiator::Key::deserialize(const SerializedData &data)
{
    using namespace Serialization;
    this->key = data.getProperty(Arps::Keys::key);
    this->period = data.getProperty(Arps::Keys::period);
    this->beat = float(data.getProperty(Arps::Keys::timestamp)) / Globals::ticksPerBeat;
    this->length = float(data.getProperty(Arps::Keys::length)) / Globals::ticksPerBeat;
    this->velocity = jlimit(0.f, 1.f,
        float(data.getProperty(Arps::Keys::volume)) / Globals::velocitySaveResolution);
    this->isBarStart = data.getProperty(Arps::Keys::isBarStart, false);
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

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
#include "Scale.h"
#include "SerializationKeys.h"

Scale::Scale(const Scale &other) noexcept :
    name(other.name), basePeriod(other.basePeriod), keys(other.keys) {}

Scale::Scale(const String &name, const Array<int> &keys, int basePeriod) noexcept :
    name(name), basePeriod(basePeriod), keys(keys) {}

Scale::Ptr Scale::withName(const String &name) const noexcept
{
    Scale::Ptr s(new Scale(*this));
    s->name = name;
    return s;
}

Scale::Ptr Scale::withKeys(const Array<int> &keys) const noexcept
{
    Scale::Ptr s(new Scale(*this));
    s->keys = keys;
    return s;
}

//===----------------------------------------------------------------------===//
// Hard-coded defaults
//===----------------------------------------------------------------------===//

inline static Array<int> getChromaticKeys()
{
    return { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
}

Scale::Ptr Scale::getChromaticScale()
{
    Scale::Ptr s(new Scale());
    s->keys = getChromaticKeys();
    s->name = TRANS("Chromatic");
    return s;
}

inline static Array<int> getNaturalMinorKeys()
{
    return { 0, 2, 3, 5, 7, 8, 10 };
}

Scale::Ptr Scale::getNaturalMinorScale()
{
    Scale::Ptr s(new Scale());
    s->keys = getNaturalMinorKeys();
    s->name = TRANS("Aeolian");
    return s;
}

inline static Array<int> getNaturalMajorKeys()
{
    return { 0, 2, 4, 5, 7, 9, 11 };
}

Scale::Ptr Scale::getNaturalMajorScale()
{
    Scale::Ptr s(new Scale());
    s->keys = getNaturalMajorKeys();
    s->name = TRANS("Ionian");
    return s;
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

int Scale::getSize() const noexcept
{
    return this->keys.size();
}

bool Scale::isValid() const noexcept
{
    return this->getSize() > 1 && this->getBasePeriod() > 0;
}

String Scale::getLocalizedName() const
{
    return TRANS(this->name);
}

String Scale::getUnlocalizedName() const noexcept
{
    return this->name;
}

const Array<int> &Scale::getKeys() const noexcept
{
    return this->keys;
}

Array<int> Scale::getChord(Chord::Ptr chord, Degree degree, bool oneOctave) const
{
    Array<int> result;
    for (const auto &chordKey : chord->getScaleKeys())
    {
        result.add(this->getChromaticKey(chordKey.getInScaleKey() + int(degree),
            chordKey.getChromaticOffset(), oneOctave));
    }
    return result;
}

Array<int> Scale::getUpScale() const
{
    Array<int> res(this->keys);
    res.add(this->getBasePeriod());
    return res;
}

Array<int> Scale::getDownScale() const
{
    Array<int> res;
    res.add(this->getBasePeriod());
    for (int i = this->keys.size(); i --> 0 ;)
    {
        res.add(this->keys[i]);
    }
    return res;
}

bool Scale::seemsMinor() const noexcept
{
    return this->getChromaticKey(int(Chord::Key::InScale::III), 0, false) == 3;
}

// Wraps a key (may be negative)
static int wrapKey(int key, int const lowerKey, int const upperKey)
{
    const int keyRange = upperKey - lowerKey;
    const int safeKey =
        (key < lowerKey) ?
        key + keyRange * ((lowerKey - key) / keyRange + 1) :
        key;

    return lowerKey + (safeKey - lowerKey) % keyRange;
}

bool Scale::hasKey(int chromaticKey) const
{
    const auto wrappedKey = wrapKey(chromaticKey, 0, this->getBasePeriod());
    return this->keys.contains(wrappedKey);
}

int Scale::getScaleKey(int chromaticKey) const
{
    const auto wrappedKey = wrapKey(chromaticKey, 0, this->getBasePeriod());
    return this->keys.indexOf(wrappedKey);
}

int Scale::getNearestScaleKey(int chromaticKey, ScaleKeyAlignment alignment) const
{
    const auto wrappedTargetKey = wrapKey(chromaticKey, 0, this->getBasePeriod());
    auto minDelta = this->getBasePeriod();
    auto result = wrappedTargetKey;

    for (int key = 0; key < this->keys.size(); ++key)
    {
        const auto chromaticScaleKey = this->keys.getUnchecked(key);
        const auto delta = abs(chromaticScaleKey - wrappedTargetKey);

        if (minDelta > delta)
        {
            minDelta = delta;

            const bool nearestIsBelow = chromaticScaleKey < wrappedTargetKey;
            const bool nearestIsAbove = chromaticScaleKey > wrappedTargetKey;

            if (alignment == ScaleKeyAlignment::Floor && nearestIsAbove)
            {
                result = key - 1;
            }
            else if (alignment == ScaleKeyAlignment::Ceil && nearestIsBelow)
            {
                result = key + 1;
            }
            else
            {
                result = key;
            }
        }
    }
    
    return result;
}

int Scale::getChromaticKey(int inScaleKey, int extraChromaticOffset,
    bool shouldRestrictToOneOctave) const noexcept
{
    const auto wrappedKey = wrapKey(inScaleKey, 0, this->getSize());
    const int index = this->keys[wrappedKey];
    const auto periodsToOffset = inScaleKey / this->getSize();
    const auto scaleToChromatic = shouldRestrictToOneOctave ? index :
        index + (this->getBasePeriod() * (inScaleKey < 0 ? periodsToOffset - 1 : periodsToOffset));
    return scaleToChromatic + extraChromaticOffset;
}

int Scale::getBasePeriod() const noexcept
{
    return this->basePeriod;
}

Scale &Scale::operator=(const Scale &other)
{
    this->name = other.name;
    this->keys = other.keys;
    return *this;
}

bool operator==(const Scale &l, const Scale &r)
{
    return &l == &r || (l.name == r.name && l.keys == r.keys);
}

bool operator!=(const Scale &l, const Scale &r)
{
    return !operator== (l, r);
}

bool Scale::isEquivalentTo(const Scale::Ptr other) const
{
    return this->isEquivalentTo(other.get());
}

bool Scale::isEquivalentTo(const Scale *other) const
{
    if (other != nullptr)
    {
        return this->keys == other->keys;
    }

    return false;
}


int Scale::getDifferenceFrom(const Scale::Ptr other) const
{
    if (other == nullptr)
    {
        return INT_MAX;
    }

    int diff = 0;
    for (int i = 0; i < jmax(this->keys.size(), other->keys.size()); ++i)
    {
        diff += abs(this->keys[i] - other->keys[i]);
    }
    return diff;
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Scale::getResourceId() const noexcept
{
    return this->name + String(this->basePeriod);
}

Identifier Scale::getResourceType() const noexcept
{
    return Serialization::Resources::scales;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

Array<int> getKeysFromIntervals(const String &intervals, int periodSize)
{
    jassert(periodSize > 0);
    Array<int> keys;
    StringArray tokens;
    tokens.addTokens(intervals, true);
    int key = 0;
    for (auto token : tokens)
    {
        keys.add(key);
        key = jlimit(0, periodSize, key + token.getIntValue());
    }
    return keys;
}

SerializedData Scale::serialize() const
{
    SerializedData tree(Serialization::Midi::scale);
    tree.setProperty(Serialization::Midi::scaleName, this->name);
    tree.setProperty(Serialization::Midi::scalePeriod, this->basePeriod);
    tree.setProperty(Serialization::Midi::scaleIntervals, this->getIntervals());
    return tree;
}

void Scale::deserialize(const SerializedData &data)
{
    using namespace Serialization;
    const auto root = data.hasType(Midi::scale) ?
        data : data.getChildWithName(Midi::scale);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Midi::scaleName, this->name);
    this->basePeriod = root.getProperty(Midi::scalePeriod, Globals::twelveTonePeriodSize);

    const String intervals = root.getProperty(Midi::scaleIntervals);
    this->keys = getKeysFromIntervals(intervals, this->basePeriod);
}

void Scale::reset()
{
    this->keys.clearQuick();
    this->name = {};
}

int Scale::hashCode() const noexcept
{
    // use unsigned ints to wrap values around
    constexpr unsigned int prime = 31;
    unsigned int hc = this->keys.size();
    for (const auto k : this->keys)
    {
        hc = hc * prime + k;
    }
    return static_cast<int>(hc);
}

Scale::Ptr Scale::fromIntervalsAndPeriod(const String &intervals, int periodSize)
{
    Scale::Ptr scale(new Scale());
    scale->keys = getKeysFromIntervals(intervals, periodSize);
    scale->basePeriod = periodSize;
    return scale;
}

String Scale::getIntervals() const noexcept
{
    int prevKey = 0;
    String intervals;
    for (const auto &key : this->keys)
    {
        if (key > 0)
        {
            intervals << String(key - prevKey) << " ";
            prevKey = key;
        }
    }

    intervals << String(this->getBasePeriod() - prevKey);
    return intervals;
}

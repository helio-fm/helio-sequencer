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
#include "Scale.h"
#include "SerializationKeys.h"
#include "ResourceCache.h"

Scale::Scale() noexcept :
    basePeriod(CHROMATIC_SCALE_SIZE) {}

Scale::Scale(const String &name) noexcept :
    name(name), basePeriod(CHROMATIC_SCALE_SIZE) {}

Scale::Scale(const Scale &other) noexcept :
    name(other.name), basePeriod(CHROMATIC_SCALE_SIZE), keys(other.keys) {}

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
{ return { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; }

Scale::Ptr Scale::getChromaticScale()
{
    Scale::Ptr s(new Scale());
    s->keys = getChromaticKeys();
    s->name = TRANS("Chromatic");
    return s;
}

inline static Array<int> getNaturalMinorKeys()
{ return { 0, 2, 3, 5, 7, 8, 10 }; }

Scale::Ptr Scale::getNaturalMinorScale()
{
    Scale::Ptr s(new Scale());
    s->keys = getNaturalMinorKeys();
    s->name = TRANS("Aeolian");
    return s;
}

inline static Array<int> getNaturalMajorKeys()
{ return { 0, 2, 4, 5, 7, 9, 11 }; }

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
    return !this->keys.isEmpty() && !this->name.isEmpty();
}

bool Scale::isChromatic() const noexcept
{
    return this->keys == getChromaticKeys();
}

String Scale::getLocalizedName() const
{
    return TRANS(this->name);
}

Array<int> Scale::getChord(Chord::Ptr chord, Function fun, bool oneOctave) const
{
    Array<int> result;
    for (const Chord::Key key : chord->getScaleKeys())
    {
        result.add(this->getChromaticKey(key + fun, oneOctave));
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
    for (int i = this->keys.size(); i --> 0; )
    { res.add(this->keys[i]); }
    return res;
}

bool Scale::seemsMinor() const noexcept
{
    return this->getChromaticKey(Chord::Key::III) == 3;
}

// Wraps a chromatic key (may be negative)
static int wrapKey(int key, int const lowerKey, int const upperKey)
{
    const int keyRange = upperKey - lowerKey;
    const int safeKey =
        (key < lowerKey) ?
        key + keyRange * ((lowerKey - key) / keyRange + 1) :
        key;

    return lowerKey + (safeKey - lowerKey) % keyRange;
}

bool Scale::hasKey(int chormaticKey) const
{
    const auto wrappedKey = wrapKey(chormaticKey, 0, this->getBasePeriod());
    return this->keys.contains(wrappedKey);
}

int Scale::getScaleKey(int chormaticKey) const
{
    const auto wrappedKey = wrapKey(chormaticKey, 0, this->getBasePeriod());
    return this->keys.indexOf(wrappedKey);
}

int Scale::getChromaticKey(int key, bool shouldRestrictToOneOctave /*= false*/) const noexcept
{
    jassert(key >= 0);
    const int idx = this->keys[key % this->getSize()];
    return shouldRestrictToOneOctave ? idx :
        idx + (this->getBasePeriod() * (key / this->getSize()));
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
    if (other != nullptr)
    {
        return this->keys == other->keys;
    }

    return false;
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Scale::getResourceId() const noexcept
{
    // Assumed to be unique:
    return this->name;
}

Identifier Scale::getResourceType() const noexcept
{
    return Serialization::Resources::scales;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Scale::serialize() const
{
    ValueTree tree(Serialization::Midi::scale);
    tree.setProperty(Serialization::Midi::scaleName, this->name, nullptr);
    tree.setProperty(Serialization::Midi::scalePeriod, this->basePeriod, nullptr);

    int prevKey = 0;
    String intervals;
    for (auto key : this->keys)
    {
        if (key > 0)
        {
            intervals += String(key - prevKey) + " ";
            prevKey = key;
        }
    }

    intervals += String(this->getBasePeriod() - prevKey);
    tree.setProperty(Serialization::Midi::scaleIntervals, intervals, nullptr);

    return tree;
}

void Scale::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Midi::scale) ?
        tree : tree.getChildWithName(Serialization::Midi::scale);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Serialization::Midi::scaleName, this->name);
    this->basePeriod = root.getProperty(Serialization::Midi::scalePeriod, CHROMATIC_SCALE_SIZE);

    const String intervals = root.getProperty(Serialization::Midi::scaleIntervals);
    StringArray tokens;
    tokens.addTokens(intervals, true);
    int key = 0;
    for (auto token : tokens)
    {
        this->keys.add(key);
        key = jlimit(0, this->basePeriod, key + token.getIntValue());
    }
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

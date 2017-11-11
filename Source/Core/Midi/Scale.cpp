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

enum Key
{
    I = 0,
    II = 1,
    III = 2,
    IV = 3,
    V = 4,
    VI = 5,
    VII = 6
};

Scale::Scale() {}
Scale::Scale(const String &name) : name(name) {}
Scale::Scale(const Scale &other) : name(other.name), keys(other.keys) {}

Scale Scale::withName(const String &name) const
{
    Scale s(*this);
    s.name = name;
    return s;
}

Scale Scale::withKeys(const Array<int> &keys) const
{
    Scale s(*this);
    s.keys = keys;
    return s;
}

//===----------------------------------------------------------------------===//
// Hard-coded defaults
//===----------------------------------------------------------------------===//

inline static Array<int> getChromaticKeys()
{ return { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; }

Scale Scale::getChromaticScale()
{
    Scale s;
    s.keys = getChromaticKeys();
    s.name = TRANS("Chromatic");
    return s;
}

inline static Array<int> getNaturalMiniorKeys()
{ return { 0, 2, 3, 5, 7, 8, 10 }; }

Scale Scale::getNaturalMiniorScale()
{
    Scale s;
    s.keys = getNaturalMiniorKeys();
    s.name = TRANS("Aeolian");
    return s;
}

inline static Array<int> getNaturalMajorKeys()
{ return { 0, 2, 4, 5, 7, 9, 11 }; }


Scale Scale::getNaturalMajorScale()
{
    Scale s;
    s.keys = getNaturalMajorKeys();
    s.name = TRANS("Ionian");
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

String Scale::getName() const noexcept
{
    return this->name;
}

String Scale::getLocalizedName() const
{
    return TRANS(this->name);
}

Array<int> Scale::getPowerChord(Function fun, bool oneOctave) const
{
    return {
        this->getKey(Key::I + fun, oneOctave),
        this->getKey(Key::V + fun, oneOctave) };
}

Array<int> Scale::getTriad(Function fun, bool oneOctave) const
{
    return {
        this->getKey(Key::I + fun, oneOctave),
        this->getKey(Key::III + fun, oneOctave),
        this->getKey(Key::V + fun, oneOctave) };
}

Array<int> Scale::getSeventhChord(Function fun, bool oneOctave) const
{
    return {
        this->getKey(Key::I + fun, oneOctave),
        this->getKey(Key::III + fun, oneOctave),
        this->getKey(Key::V + fun, oneOctave),
        this->getKey(Key::VII + fun, oneOctave) };
}

Array<int> Scale::getUpScale() const
{
    Array<int> res(this->keys);
    res.add(CHROMATIC_SCALE_SIZE);
    return res;
}

Array<int> Scale::getDownScale() const
{
    Array<int> res;
    res.add(CHROMATIC_SCALE_SIZE);
    for (int i = this->keys.size(); i --> 0; )
    { res.add(this->keys[i]); }
    return res;
}

bool Scale::seemsMinor() const
{
    return this->getKey(Key::III) == 3;
}

bool Scale::hasKey(int key) const
{
    return this->keys.contains(key);
}

int Scale::getKey(int key, bool shouldRestrictToOneOctave /*= false*/) const
{
    jassert(key >= 0);
    const int idx = this->keys[key % this->getSize()];
    return shouldRestrictToOneOctave ? idx :
        idx + (CHROMATIC_SCALE_SIZE * (key / this->getSize()));
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

bool Scale::isEquivalentTo(const Scale &other) const
{
    return this->keys == other.keys;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Scale::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::scale);
    xml->setAttribute(Serialization::Core::scaleName, this->name);

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

    intervals += String(CHROMATIC_SCALE_SIZE - prevKey);
    xml->setAttribute(Serialization::Core::scaleIntervals, intervals);

    return xml;
}

void Scale::deserialize(const XmlElement &xml)
{
    const XmlElement *root = (xml.getTagName() == Serialization::Core::scale) ?
        &xml : xml.getChildByName(Serialization::Core::scale);

    if (root == nullptr) { return; }

    this->reset();

    this->name = root->getStringAttribute(Serialization::Core::scaleName, this->name);
    const String intervals = root->getStringAttribute(Serialization::Core::scaleIntervals);
    StringArray tokens;
    tokens.addTokens(intervals, true);
    int key = 0;
    for (auto token : tokens)
    {
        this->keys.add(key);
        key += token.getIntValue();
    }
}

void Scale::reset()
{
    this->keys.clearQuick();
    this->name = {};
}

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
#include "Chord.h"
#include "SerializationKeys.h"

Chord::Chord() noexcept {}

Chord::Chord(const Chord &other) noexcept :
    name(other.name), scaleKeys(other.scaleKeys) {}

Chord::Chord(const String &name) noexcept :
    name(name) {}

Chord::Ptr Chord::getTriad() noexcept
{
    Chord::Ptr s(new Chord());
    s->scaleKeys = { Key::I, Key::III, Key::V };
    s->name = "3"; // FIXME proper translated names?
    return s;
}

Chord::Ptr Chord::getPowerChord() noexcept
{
    Chord::Ptr s(new Chord());
    s->scaleKeys = { Key::I, Key::V };
    s->name = "5"; // FIXME proper translated names?
    return s;
}

Chord::Ptr Chord::getSeventhChord() noexcept
{
    Chord::Ptr s(new Chord());
    s->scaleKeys = { Key::I, Key::III, Key::V, Key::VII };
    s->name = "7"; // FIXME proper translated names?
    return s;
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Chord::getResourceId() const
{
    // Assumed to be unique:
    return this->name;
}

Identifier Chord::getResourceIdProperty() const
{
    return Serialization::Midi::chordName;
}

const Array<Chord::Key> &Chord::getScaleKeys() const noexcept
{
    return this->scaleKeys;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Chord::serialize() const
{
    ValueTree tree(Serialization::Midi::chord);
    tree.setProperty(Serialization::Midi::chordName, this->name, nullptr);

    String keysString;
    for (auto key : this->scaleKeys)
    {
        keysString += String(key + 1) + " ";
    }

    tree.setProperty(Serialization::Midi::chordScaleKeys, keysString, nullptr);
    return tree;
}

void Chord::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Midi::chord) ?
        tree : tree.getChildWithName(Serialization::Midi::chord);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Serialization::Midi::chordName, this->name);
    const String keysString = root.getProperty(Serialization::Midi::chordScaleKeys);

    StringArray tokens;
    tokens.addTokens(keysString, true);
    for (auto key : tokens)
    {
        this->scaleKeys.add(Key(jlimit(0, 6, key.getIntValue() - 1)));
    }
}

void Chord::reset()
{
    this->scaleKeys.clearQuick();
    this->name = {};
}

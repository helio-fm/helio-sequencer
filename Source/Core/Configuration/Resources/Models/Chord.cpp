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
#include "Chord.h"
#include "SerializationKeys.h"

Chord::Chord() noexcept {}

Chord::Chord(const Chord &other) noexcept :
    name(other.name), scaleKeys(other.scaleKeys) {}

Chord::Chord(const String &name) noexcept :
    name(name) {}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Chord::getResourceId() const noexcept
{
    // Assumed to be unique:
    return this->name;
}

Identifier Chord::getResourceType() const noexcept
{
    return Serialization::Resources::chords;
}

const bool Chord::isValid() const noexcept
{
    return !this->scaleKeys.isEmpty();
}

const String &Chord::getName() const noexcept
{
    return this->name;
}

const Array<Chord::Key> &Chord::getScaleKeys() const noexcept
{
    return this->scaleKeys;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Chord::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Midi::chord);
    tree.setProperty(Midi::chordName, this->name);

    String keysString;
    for (auto &key : this->scaleKeys)
    {
        keysString += key.getStringValue() + " ";
    }

    tree.setProperty(Midi::chordScaleKeys, keysString.trim());
    return tree;
}

void Chord::deserialize(const SerializedData &data)
{
    using namespace Serialization;
    const auto root = data.hasType(Midi::chord) ?
        data : data.getChildWithName(Midi::chord);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(Midi::chordName, this->name);
    const String keysString = root.getProperty(Midi::chordScaleKeys);

    StringArray tokens;
    tokens.addTokens(keysString, true);
    for (auto &key : tokens)
    {
        const bool isAugmented = key.containsChar('#');
        const bool isDiminished = key.containsChar('b');
        const auto cleanedUpKey = key.removeCharacters("#b");
        const auto keyIntValue = Key::InScale(jlimit(0, 13, cleanedUpKey.getIntValue() - 1));
        this->scaleKeys.add(Key(keyIntValue, isAugmented, isDiminished));
    }
}

void Chord::reset()
{
    this->scaleKeys.clearQuick();
    this->name = {};
}

Array<String> Chord::getLocalizedDegreeNames()
{
    static Array<String> names = {
        TRANS(I18n::Popup::chordFunction1),
        TRANS(I18n::Popup::chordFunction2),
        TRANS(I18n::Popup::chordFunction3),
        TRANS(I18n::Popup::chordFunction4),
        TRANS(I18n::Popup::chordFunction5),
        TRANS(I18n::Popup::chordFunction6),
        TRANS(I18n::Popup::chordFunction7)
    };

    return names;
}

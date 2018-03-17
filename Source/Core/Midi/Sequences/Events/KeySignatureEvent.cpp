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
#include "KeySignatureEvent.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"

KeySignatureEvent::KeySignatureEvent() :
    MidiEvent(nullptr, MidiEvent::KeySignature, 0.f),
    rootKey(0),
    scale()
{
    //jassertfalse;
}

KeySignatureEvent::KeySignatureEvent(const KeySignatureEvent &other) :
    MidiEvent(other),
    rootKey(other.rootKey),
    scale(other.scale) {}

KeySignatureEvent::KeySignatureEvent(WeakReference<MidiSequence> owner,
    float newBeat /*= 0.f*/,
    Note::Key key /*= 60*/,
    Scale scale /*= Scale()*/) :
    MidiEvent(owner, MidiEvent::KeySignature, newBeat),
    rootKey(key),
    scale(scale) {}

KeySignatureEvent::KeySignatureEvent(WeakReference<MidiSequence> owner,
    const KeySignatureEvent &parametersToCopy) :
    MidiEvent(owner, parametersToCopy),
    rootKey(parametersToCopy.rootKey),
    scale(parametersToCopy.scale) {}

String KeySignatureEvent::toString() const
{
    const int index = this->rootKey % CHROMATIC_SCALE_SIZE;
    const String keyName = Scale::getKeyNames()[index];
    return keyName + ", " + this->scale.getLocalizedName();
}

Array<MidiMessage> KeySignatureEvent::toMidiMessages() const
{
    // Basically, we can have any non-standard scale here:
    // from "symmetrical nonatonic" or "chromatic permutated diatonic dorian"
    // to any kind of madness a human mind can come up with.

    // But in order to conform midi format, we need to fit it into a circle of fifths
    // (which only represents a number of western major and minor scales),
    // and we have to guess if our scale is major or minor,
    // and then try to determine a number of flats or a number of sharps.

    Array<MidiMessage> result;
    const bool isMinor = this->scale.seemsMinor();

    // Hard-coded number of flats and sharps for major and minor keys in a circle of fifths,
    // where negative numbers represent flats and positive numbers represent sharps,
    // and index is a root key, starting from C:
    static const int majorCircle[] = {  0, 7,  2, -3, 4, -1, 6,  1, -4, 3, -2, 5 };
    static const int minorCircle[] = { -3, 4, -1, -6, 1, -4, 3, -2, -7, 0, -5, 2 };
    const int root = this->rootKey % 12;
    const int flatsOrSharps = isMinor ? minorCircle[root] : majorCircle[root];

    MidiMessage event(MidiMessage::keySignatureMetaEvent(flatsOrSharps, isMinor));
    event.setTimeStamp(round(this->beat * MS_PER_BEAT));
    result.add(event);
    return result;
}

KeySignatureEvent KeySignatureEvent::withDeltaBeat(float beatOffset) const
{
    KeySignatureEvent e(*this);
    e.beat = e.beat + beatOffset;
    return e;
}

KeySignatureEvent KeySignatureEvent::withBeat(float newBeat) const
{
    KeySignatureEvent e(*this);
    e.beat = newBeat;
    return e;
}

KeySignatureEvent KeySignatureEvent::withRootKey(Note::Key key) const
{
    KeySignatureEvent e(*this);
    e.rootKey = key;
    return e;
}

KeySignatureEvent KeySignatureEvent::withScale(Scale scale) const
{
    KeySignatureEvent e(*this);
    e.scale = scale;
    return e;
}

KeySignatureEvent KeySignatureEvent::withParameters(const ValueTree &parameters) const
{
    KeySignatureEvent e(*this);
    e.deserialize(parameters);
    return e;
}

KeySignatureEvent KeySignatureEvent::copyWithNewId() const
{
    KeySignatureEvent e(*this);
    e.id = e.createId();
    return e;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

Note::Key KeySignatureEvent::getRootKey() const noexcept
{
    return this->rootKey;
}

const Scale &KeySignatureEvent::getScale() const noexcept
{
    return this->scale;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree KeySignatureEvent::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Midi::keySignature);
    tree.setProperty(Midi::id, this->id, nullptr);
    tree.setProperty(Midi::key, this->rootKey, nullptr);
    tree.setProperty(Midi::timestamp, roundFloatToInt(this->beat * TICKS_PER_BEAT), nullptr);
    tree.appendChild(this->scale.serialize(), nullptr);
    return tree;
}

void KeySignatureEvent::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;
    this->rootKey = tree.getProperty(Midi::key, 0);
    this->beat = float(tree.getProperty(Midi::timestamp)) / TICKS_PER_BEAT;
    this->id = tree.getProperty(Midi::id);

    // Anyway there is only one child scale for now:
    forEachValueTreeChildWithType(tree, e, Serialization::Core::scale)
    {
        this->scale.deserialize(e);
    }
}

void KeySignatureEvent::reset()
{
    this->rootKey = KEY_C5;
    this->scale = Scale();
}

void KeySignatureEvent::applyChanges(const KeySignatureEvent &parameters)
{
    jassert(this->id == parameters.id);
    this->beat = parameters.beat;
    this->rootKey = parameters.rootKey;
    this->scale = parameters.scale;
}

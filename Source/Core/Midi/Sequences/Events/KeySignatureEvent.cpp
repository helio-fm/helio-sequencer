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

KeySignatureEvent::KeySignatureEvent() noexcept :
    MidiEvent(nullptr, MidiEvent::KeySignature, 0.f),
    rootKey(0),
    scale()
{
    //jassertfalse;
}

KeySignatureEvent::KeySignatureEvent(const KeySignatureEvent &other) noexcept :
    MidiEvent(other),
    rootKey(other.rootKey),
    scale(other.scale) {}

KeySignatureEvent::KeySignatureEvent(WeakReference<MidiSequence> owner,
    Scale::Ptr scale,
    float newBeat /*= 0.f*/,
    Note::Key key /*= 60*/) noexcept :
    MidiEvent(owner, MidiEvent::KeySignature, newBeat),
    rootKey(key),
    scale(scale) {}

KeySignatureEvent::KeySignatureEvent(WeakReference<MidiSequence> owner,
    const KeySignatureEvent &parametersToCopy) noexcept :
    MidiEvent(owner, parametersToCopy),
    rootKey(parametersToCopy.rootKey),
    scale(parametersToCopy.scale) {}

String KeySignatureEvent::toString() const
{
    const int index = this->rootKey % this->scale->getBasePeriod();
    const String keyName = Scale::getKeyNames()[index];
    return keyName + ", " + this->scale->getLocalizedName();
}

void KeySignatureEvent::exportMessages(MidiMessageSequence &outSequence, const Clip &clip, double timeOffset, double timeFactor) const
{
    // Basically, we can have any non-standard scale here:
    // from "symmetrical nonatonic" or "chromatic permutated diatonic dorian"
    // to any kind of madness a human mind can come up with.

    // But in order to conform midi format, we need to fit it into a circle of fifths
    // (which only represents a number of western major and minor scales),
    // and we have to guess if our scale is major or minor,
    // and then try to determine a number of flats or a number of sharps.

    const bool isMinor = this->scale->seemsMinor();

    // Hard-coded number of flats and sharps for major and minor keys in a circle of fifths,
    // where negative numbers represent flats and positive numbers represent sharps,
    // and index is a root key, starting from C:
    static const int majorCircle[] = {  0, 7,  2, -3, 4, -1, 6,  1, -4, 3, -2, 5 };
    static const int minorCircle[] = { -3, 4, -1, -6, 1, -4, 3, -2, -7, 0, -5, 2 };
    const int root = this->rootKey % 12;
    const int flatsOrSharps = isMinor ? minorCircle[root] : majorCircle[root];

    MidiMessage event(MidiMessage::keySignatureMetaEvent(flatsOrSharps, isMinor));
    event.setTimeStamp((this->beat + clip.getBeat()) * timeFactor);
    outSequence.addEvent(event, timeOffset);
}

KeySignatureEvent KeySignatureEvent::withDeltaBeat(float beatOffset) const noexcept
{
    KeySignatureEvent e(*this);
    e.beat = e.beat + beatOffset;
    return e;
}

KeySignatureEvent KeySignatureEvent::withBeat(float newBeat) const noexcept
{
    KeySignatureEvent e(*this);
    e.beat = newBeat;
    return e;
}

KeySignatureEvent KeySignatureEvent::withRootKey(Note::Key key) const noexcept
{
    KeySignatureEvent e(*this);
    e.rootKey = key;
    return e;
}

KeySignatureEvent KeySignatureEvent::withScale(Scale::Ptr scale) const noexcept
{
    KeySignatureEvent e(*this);
    e.scale = scale;
    return e;
}

KeySignatureEvent KeySignatureEvent::withParameters(const ValueTree &parameters) const noexcept
{
    KeySignatureEvent e(*this);
    e.deserialize(parameters);
    return e;
}

KeySignatureEvent KeySignatureEvent::copyWithNewId() const noexcept
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

const Scale::Ptr KeySignatureEvent::getScale() const noexcept
{
    return this->scale;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree KeySignatureEvent::serialize() const noexcept
{
    using namespace Serialization;
    ValueTree tree(Midi::keySignature);
    tree.setProperty(Midi::id, this->id, nullptr);
    tree.setProperty(Midi::key, this->rootKey, nullptr);
    tree.setProperty(Midi::timestamp, int(this->beat * TICKS_PER_BEAT), nullptr);
    tree.appendChild(this->scale->serialize(), nullptr);
    return tree;
}

void KeySignatureEvent::deserialize(const ValueTree &tree) noexcept
{
    this->reset();
    using namespace Serialization;
    this->rootKey = tree.getProperty(Midi::key, 0);
    this->beat = float(tree.getProperty(Midi::timestamp)) / TICKS_PER_BEAT;
    this->id = tree.getProperty(Midi::id);

    this->scale = new Scale();
    this->scale->deserialize(tree);
}

void KeySignatureEvent::reset() noexcept
{
    this->rootKey = MIDDLE_C;

    if (this->scale != nullptr)
    {
        this->scale->reset();
    }
}

void KeySignatureEvent::applyChanges(const KeySignatureEvent &parameters) noexcept
{
    jassert(this->id == parameters.id);
    this->beat = parameters.beat;
    this->rootKey = parameters.rootKey;
    this->scale = parameters.scale;
}

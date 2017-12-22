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
    MidiEvent(other.sequence, MidiEvent::KeySignature, other.beat),
    rootKey(other.rootKey),
    scale(other.scale)
{
    this->id = other.getId();
}

KeySignatureEvent::KeySignatureEvent(WeakReference<MidiSequence> owner,
    float newBeat /*= 0.f*/,
    Note::Key key /*= 60*/,
    Scale scale /*= Scale()*/) :
    MidiEvent(owner, MidiEvent::KeySignature, newBeat),
    rootKey(key),
    scale(scale) {}

KeySignatureEvent::KeySignatureEvent(WeakReference<MidiSequence> owner,
    const KeySignatureEvent &parametersToCopy) :
    MidiEvent(owner, MidiEvent::KeySignature, parametersToCopy.beat),
    rootKey(parametersToCopy.rootKey),
    scale(parametersToCopy.scale)
{
    this->id = parametersToCopy.getId();
}

String KeySignatureEvent::toString() const
{
    const int index = this->rootKey % CHROMATIC_SCALE_SIZE;
    const String keyName = Scale::getKeyNames()[index];
    return keyName + ", " + this->scale.getName();
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
    event.setTimeStamp(this->beat * MS_PER_BEAT);
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

KeySignatureEvent KeySignatureEvent::withParameters(const XmlElement &xml) const
{
    KeySignatureEvent e(*this);
    e.deserialize(xml);
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

XmlElement *KeySignatureEvent::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::keySignature);
    xml->setAttribute("key", this->rootKey);
    xml->setAttribute("beat", this->beat);
    xml->setAttribute("id", this->id);
    xml->addChildElement(this->scale.serialize());
    return xml;
}

void KeySignatureEvent::deserialize(const XmlElement &xml)
{
    this->reset();
    this->rootKey = xml.getIntAttribute("key", 0);
    this->beat = float(xml.getDoubleAttribute("beat"));
    this->id = xml.getStringAttribute("id");

    // Anyway there is only one child scale for now:
    forEachXmlChildElementWithTagName(xml, e, Serialization::Core::scale)
    {
        this->scale.deserialize(*e);
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

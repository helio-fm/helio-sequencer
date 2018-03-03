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
#include "AnnotationEvent.h"
#include "MidiSequence.h"
#include "SerializationKeys.h"

AnnotationEvent::AnnotationEvent() : MidiEvent(nullptr, MidiEvent::Annotation, 0.f)
{
    //jassertfalse;
}

AnnotationEvent::AnnotationEvent(const AnnotationEvent &other) :
    MidiEvent(other),
    description(other.description),
    colour(other.colour) {}

AnnotationEvent::AnnotationEvent(WeakReference<MidiSequence> owner,
    float newBeat, String newDescription, const Colour &newColour) :
    MidiEvent(owner, MidiEvent::Annotation, newBeat),
    description(std::move(newDescription)),
    colour(newColour) {}

AnnotationEvent::AnnotationEvent(WeakReference<MidiSequence> owner,
    const AnnotationEvent &parametersToCopy) :
    MidiEvent(owner, parametersToCopy),
    description(parametersToCopy.description),
    colour(parametersToCopy.colour) {}

Array<MidiMessage> AnnotationEvent::toMidiMessages() const
{
    Array<MidiMessage> result;
    MidiMessage event(MidiMessage::textMetaEvent(1, this->getDescription()));
    event.setTimeStamp(round(this->beat * MS_PER_BEAT));
    result.add(event);
    return result;
}

AnnotationEvent AnnotationEvent::withDeltaBeat(float beatOffset) const
{
    AnnotationEvent ae(*this);
    ae.beat = ae.beat + beatOffset;
    return ae;
}

AnnotationEvent AnnotationEvent::withBeat(float newBeat) const
{
    AnnotationEvent ae(*this);
    ae.beat = newBeat;
    return ae;
}

AnnotationEvent AnnotationEvent::withDescription(const String &newDescription) const
{
    AnnotationEvent ae(*this);
    ae.description = newDescription;
    return ae;
}

AnnotationEvent AnnotationEvent::withColour(const Colour &newColour) const
{
    AnnotationEvent ae(*this);
    ae.colour = newColour;
    return ae;
}

AnnotationEvent AnnotationEvent::withParameters(const ValueTree &parameters) const
{
    AnnotationEvent ae(*this);
    ae.deserialize(parameters);
    return ae;
}

AnnotationEvent AnnotationEvent::copyWithNewId() const
{
    AnnotationEvent ae(*this);
    ae.id = ae.createId();
    return ae;
}


//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

String AnnotationEvent::getDescription() const noexcept
{
    return this->description;
}

Colour AnnotationEvent::getColour() const noexcept
{
    return this->colour;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree AnnotationEvent::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Midi::annotation);
    tree.setProperty(Midi::text, this->description, nullptr);
    tree.setProperty(Midi::colour, this->colour.toString(), nullptr);
    tree.setProperty(Midi::beat, this->beat, nullptr);
    tree.setProperty(Midi::id, this->id, nullptr);
    return tree;
}

void AnnotationEvent::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;
    this->description = tree.getProperty(Midi::text);
    this->colour = Colour::fromString(tree.getProperty(Midi::colour).toString());
    this->beat = roundBeat(tree.getProperty(Midi::beat));
    this->id = tree.getProperty(Midi::id);
}

void AnnotationEvent::reset() {}

void AnnotationEvent::applyChanges(const AnnotationEvent &other)
{
    jassert(this->id == other.id);
    this->description = other.description;
    this->colour = other.colour;
    this->beat = other.beat;
}

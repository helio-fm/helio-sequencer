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
#include "Transport.h"
#include "SerializationKeys.h"


AnnotationEvent::AnnotationEvent() : MidiEvent(nullptr, 0.f)
{
    //jassertfalse;
}

AnnotationEvent::AnnotationEvent(const AnnotationEvent &other) :
    MidiEvent(other.sequence, other.beat),
    description(other.description),
    colour(other.colour)
{
    this->id = other.getId();
}

AnnotationEvent::AnnotationEvent(MidiSequence *owner,
                     float newBeat,
                     String newDescription,
                     const Colour &newColour) :
    MidiEvent(owner, newBeat),
    description(std::move(newDescription)),
    colour(newColour)
{
}

AnnotationEvent::~AnnotationEvent()
{
}

Array<MidiMessage> AnnotationEvent::toMidiMessages() const
{
    Array<MidiMessage> result;
    MidiMessage event(MidiMessage::textMetaEvent(1, this->getDescription()));
    event.setTimeStamp(this->beat * Transport::millisecondsPerBeat);
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

AnnotationEvent AnnotationEvent::withParameters(const XmlElement &xml) const
{
    AnnotationEvent ae(*this);
    ae.deserialize(xml);
    return ae;
}

AnnotationEvent AnnotationEvent::copyWithNewId() const
{
    AnnotationEvent ae(*this);
    ae.id = this->createId();
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

XmlElement *AnnotationEvent::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::annotation);
    xml->setAttribute("text", this->description);
    xml->setAttribute("col", this->colour.toString());
    xml->setAttribute("beat", this->beat);
    xml->setAttribute("id", this->id);
    return xml;
}

void AnnotationEvent::deserialize(const XmlElement &xml)
{
    this->reset();

    // здесь никаких проверок. посмотри, как быстро будет работать сериализация.
    this->description = xml.getStringAttribute("text");
    this->colour = Colour::fromString(xml.getStringAttribute("col"));
    this->beat = float(xml.getDoubleAttribute("beat"));
    this->id = xml.getStringAttribute("id");
}

void AnnotationEvent::reset()
{
    // здесь ничего пока не чистим, это не нужно и вообще, проверь скорость работы сериализации.
}


int AnnotationEvent::hashCode() const noexcept
{
    return this->getDescription().hashCode() +
           this->getId().hashCode();
}

AnnotationEvent &AnnotationEvent::operator=(const AnnotationEvent &right)
{
    //if (this == &right) { return *this; }

    //this->sequence = *right.getLayer(); // never do this
    this->id = right.id;
    this->beat = right.beat;
    this->description = right.description;
    this->colour = right.colour;

    return *this;
}

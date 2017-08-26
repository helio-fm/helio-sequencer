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
#include "Clip.h"
#include "SerializationKeys.h"

Clip::Clip()
{
    // needed for juce's Array to work
    jassertfalse;
}

Clip::Clip(const Clip &other) :
    pattern(other.pattern),
    startBeat(other.startBeat),
    id(other.id)
{
}

Clip::Clip(Pattern *owner, float beatVal) :
    pattern(owner),
    startBeat(beatVal)
{
    id = this->createId();
}

Pattern *Clip::getPattern() const noexcept
{
    jassert(this->pattern != nullptr);
    return this->pattern;
}

float Clip::getStartBeat() const noexcept
{
    return this->startBeat;
}

String Clip::getId() const noexcept
{
    return this->id;
}

Clip Clip::copyWithNewId(Pattern *newOwner) const
{
    Clip c(*this);
    c.id = this->createId();

    if (newOwner != nullptr)
    {
        c.pattern = newOwner;
    }

    return c;
}

Clip Clip::withParameters(const XmlElement &xml) const
{
    Clip c(*this);
    c.deserialize(xml);
    return c;
}

static float roundBeat(float beat)
{
    return roundf(beat * 16.f) / 16.f;
}

Clip Clip::withDeltaBeat(float deltaPosition) const
{
    Clip other(*this);
    other.startBeat = roundBeat(other.startBeat + deltaPosition);
    return other;
}

XmlElement *Clip::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::clip);
    xml->setAttribute("start", this->startBeat);
    xml->setAttribute("id", this->id);
    return xml;
}

void Clip::deserialize(const XmlElement &xml)
{
    this->startBeat = float(xml.getDoubleAttribute("start", this->startBeat));
    this->id = xml.getStringAttribute("id", this->id);
}

void Clip::reset()
{
    this->startBeat = 0.f;
}

Clip &Clip::operator=(const Clip &right)
{
    //if (this == &right) { return *this; }
    //this->pattern = right.pattern; // never do this
    this->id = right.id;
    this->startBeat = right.startBeat;
    return *this;
}

int Clip::compareElements(const Clip &first, const Clip &second)
{
    if (&first == &second) { return 0; }
    if (first.id == second.id) { return 0; }

    const float diff = first.startBeat - second.startBeat;
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    return diffResult;
}

int Clip::hashCode() const noexcept
{
    return this->getId().hashCode();
}

Clip::Id Clip::createId() noexcept
{
    Uuid uuid;
    return uuid.toString();
}

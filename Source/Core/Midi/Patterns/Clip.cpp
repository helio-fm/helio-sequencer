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

Clip::Clip() : startBeat(0.f)
{
	Uuid uuid;
	id = uuid.toString();
}

Clip::Clip(const Clip &other) :
	startBeat(other.startBeat),
	id(other.id)
{
}

float Clip::getStartBeat() const noexcept
{
	return this->startBeat;
}

String Clip::getId() const noexcept
{
	return this->id;
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

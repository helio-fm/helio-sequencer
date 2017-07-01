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

#pragma once

#include "Serializable.h"

// Just an instance of a midi layer on a certain position
class Clip : public Serializable
{
public:

    using Id = String;

	Clip();
	Clip(const Clip &other);

	float getStartBeat() const noexcept;
	String getId() const noexcept;
	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

    Clip &operator=(const Clip &right)
    {
        this->id = right.id;
        this->startBeat = right.startBeat;
        return *this;
    }

	friend inline bool operator==(const Clip &lhs, const Clip &rhs)
	{
		return (&lhs == &rhs || lhs.id == rhs.id);
	}

	static int compareElements(const Clip &first,
		const Clip &second);

	int hashCode() const noexcept;

private:

	float startBeat;
	String id;

	JUCE_LEAK_DETECTOR(Clip);
};

class ClipHashFunction
{
public:

	static int generateHash(Clip clip, const int upperLimit) noexcept
	{
		return static_cast<int>((static_cast<uint32>(clip.hashCode())) % static_cast<uint32>(upperLimit));
	}
};

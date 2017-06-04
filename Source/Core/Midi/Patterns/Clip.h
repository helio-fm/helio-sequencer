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

// Just an instance of a midi track on a certain position
class Clip : public Serializable
{
public:

	Clip();
	Clip(const Clip &other);

	float getStartBeat() const noexcept;
	String getId() const noexcept;
	XmlElement *serialize() const override;
	void deserialize(const XmlElement &xml) override;
	void reset() override;

	friend inline bool operator==(const Clip &lhs, const Clip &rhs)
	{
		return (&lhs == &rhs || lhs.id == rhs.id);
	}

	static int compareElements(const Clip &first,
		const Clip &second);

private:

	float startBeat;
	String id;

	JUCE_LEAK_DETECTOR(Clip);
};

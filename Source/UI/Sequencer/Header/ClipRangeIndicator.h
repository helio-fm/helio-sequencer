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

//[Headers]
//[/Headers]


class ClipRangeIndicator final : public Component
{
public:

    ClipRangeIndicator();
    ~ClipRangeIndicator();

    //[UserMethods]
    float getFirstBeat() const noexcept { return this->firstBeat; }
    float getLastBeat() const noexcept { return this->lastBeat; }

    // Returns true if range is updated and component should be repositioned
    bool updateWith(const Colour &colour, float firstBeat, float lastBeat);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]
    Colour paintColour;
    Colour trackColour;
    float firstBeat;
    float lastBeat;
    //[/UserVariables]


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipRangeIndicator)
};

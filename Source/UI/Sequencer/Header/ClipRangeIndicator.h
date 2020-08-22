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

class ClipRangeIndicator final : public Component
{
public:

    ClipRangeIndicator()
    {
        this->setOpaque(true);
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    float getFirstBeat() const noexcept
    {
        return this->firstBeat;
    }

    float getLastBeat() const noexcept
    {
        return this->lastBeat;
    }

    // Returns true if range is updated and component should be repositioned
    bool updateWith(const Colour &colour, float start, float end)
    {
        bool updatedRange = this->firstBeat != start || this->lastBeat != end;

        if (this->trackColour != colour)
        {
            this->trackColour = colour;
            const auto fgColour = findDefaultColour(Label::textColourId);
            this->paintColour = colour.interpolatedWith(fgColour, 0.5f).withAlpha(0.45f);
            this->repaint();
        }
        
        this->firstBeat = start;
        this->lastBeat = end;

        return updatedRange;
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->paintColour);
        g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
    }

private:

    Colour paintColour;
    Colour trackColour;
    float firstBeat = 0.f;
    float lastBeat = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipRangeIndicator)
};

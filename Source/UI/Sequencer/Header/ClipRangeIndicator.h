/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "HelioTheme.h"

class ClipRangeIndicator final : public Component
{
public:

    ClipRangeIndicator()
    {
        this->setOpaque(false);
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setAccessible(false);
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
    bool updateWith(const Colour &colour, float start, float end, bool shouldBeActive)
    {
        const bool hasRangeUpdates = this->firstBeat != start || this->lastBeat != end;

        if (this->trackColour != colour || this->isActive != shouldBeActive)
        {
            this->trackColour = colour;
            this->isActive = shouldBeActive;
            const auto fgColour = findDefaultColour(Label::textColourId);
            this->paintColour = colour
                .interpolatedWith(fgColour, 0.5f)
                .withAlpha(shouldBeActive ? 0.5f : 0.4f);

            this->repaint();
        }
        
        this->firstBeat = start;
        this->lastBeat = end;

        return hasRangeUpdates;
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->paintColour);

        if (this->isActive)
        {
            g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
        }
        else
        {
            HelioTheme::drawDashedHorizontalLine(g, 0.f, 0.f, float(this->getWidth()));
        }
    }

protected:

    Colour paintColour;
    Colour trackColour;

    float firstBeat = 0.f;
    float lastBeat = 0.f;

    bool isActive = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipRangeIndicator)
};

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
#include "ColourIDs.h"

class SeparatorVerticalSkew final : public Component
{
public:

    SeparatorVerticalSkew()
    {
        this->setOpaque(true);
        this->setInterceptsMouseClicks(false, false);
        this->setAccessible(false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColourB);
        g.fillRect(this->getLocalBounds());

        g.setColour(this->fillColourA);
        g.fillPath(this->skewShape, {});

        g.setColour(this->darkColour);
        g.fillPath(this->line1, {});

        g.setColour(this->lightColour);
        g.fillPath(this->line2, {});
    }

    void resized() override
    {
        const float h = float(this->getHeight());
        const float w = float(this->getWidth());

        this->line1.clear();
        this->line1.addLineSegment({ w, 0.f, 1.f, h }, 0.5f);

        this->line2.clear();
        this->line2.addLineSegment({ w - 1.f, 0.f, 0.f, h }, 0.75f);

        this->skewShape.clear();
        this->skewShape.startNewSubPath(0.f, 0.f);
        this->skewShape.lineTo(w, 0.f);
        this->skewShape.lineTo(0.f, h);
        this->skewShape.closeSubPath();
    }

private:

    Path line1;
    Path line2;
    Path skewShape;

    const Colour fillColourA =
        findDefaultColour(ColourIDs::Panel::pageFillA);
    const Colour fillColourB =
        findDefaultColour(ColourIDs::Panel::pageFillB);
    const Colour lightColour =
        findDefaultColour(ColourIDs::Common::separatorLineLight);
    const Colour darkColour =
        findDefaultColour(ColourIDs::Common::separatorLineDark);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeparatorVerticalSkew)
};

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

#include "ColourIDs.h"

class SeparatorVertical final : public Component
{
public:

    SeparatorVertical()
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->lightColour);
        g.fillRect(0, 0, 1, this->getHeight());

        g.setColour(this->darkColour);
        g.fillRect(1, 0, 1, this->getHeight());
    }

private:

    const Colour lightColour =
        findDefaultColour(ColourIDs::Common::separatorLineLight)
            .withMultipliedAlpha(0.75f);

    const Colour darkColour =
        findDefaultColour(ColourIDs::Common::separatorLineDark)
            .withMultipliedAlpha(0.75f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeparatorVertical)
};

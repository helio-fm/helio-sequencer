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

#include "ColourIDs.h"

class FramePanel final : public Component
{
public:

    FramePanel()
    {
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->colour);

        const float w = float(this->getWidth());
        const float h = float(this->getHeight());

        g.drawVerticalLine(0, 1.f, h - 1.f);
        g.drawVerticalLine(int(w) - 1, 1.f, h - 1.f);
        g.drawHorizontalLine(0, 1.f, w - 1.f);
        g.drawHorizontalLine(int(h) - 1, 1.f, w - 1.f);
    }

private:

    const Colour colour = findDefaultColour(ColourIDs::Panel::border);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FramePanel)
};

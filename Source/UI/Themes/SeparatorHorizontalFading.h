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

class SeparatorHorizontalFading final : public Component
{
public:

    SeparatorHorizontalFading()
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setGradientFill(ColourGradient(Colour(0x35000000),
            float(this->getWidth() / 2), 0.f,
            Colour(0x00000000),
            0.f, 0.f,
            true));

        g.fillRect(0, 0, this->getWidth(), 1);

        g.setGradientFill(ColourGradient(Colour(0x15ffffff),
            float(this->getWidth() / 2), 0.f,
            Colour(0x00ffffff),
            0.f, 0.f,
            true));

        g.fillRect(0, 1, this->getWidth(), 1);
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeparatorHorizontalFading)
};

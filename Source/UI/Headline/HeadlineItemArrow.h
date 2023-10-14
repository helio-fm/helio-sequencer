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

class HeadlineItemArrow final : public Component
{
public:

    HeadlineItemArrow()
    {
        this->setOpaque(false);
        this->setBufferedToImage(true);
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setSize(HeadlineItemArrow::arrowWidth, Globals::UI::headlineHeight);
    }

    void paint(Graphics &g) override
    {
        g.setGradientFill(ColourGradient(this->shadowColour1,
            float(this->getWidth() - 2), float(this->getHeight() - 2),
            this->shadowColour2,
            float(this->getWidth() - HeadlineItemArrow::arrowWidth), 2.0f,
            true));

        g.strokePath(this->shadowPath, PathStrokeType(1.0f));

        g.setGradientFill(ColourGradient(this->arrowColour1,
            float(this->getWidth() - 3), float(this->getHeight() - 2),
            this->arrowColour2,
            float(this->getWidth() - HeadlineItemArrow::arrowWidth), 0.0f,
            true));

        g.strokePath(this->arrowPath, PathStrokeType(0.5f));
    }

    void resized() override
    {
        this->arrowPath.clear();
        this->arrowPath.startNewSubPath(float(this->getWidth() - HeadlineItemArrow::arrowWidth - 1), 0.0f);
        this->arrowPath.lineTo(float(this->getWidth() - 2.5), float(this->getHeight()));

        this->shadowPath.clear();
        this->shadowPath.startNewSubPath(float(this->getWidth() - HeadlineItemArrow::arrowWidth), 0.0f);
        this->shadowPath.lineTo(float(this->getWidth() - 1.5), float(this->getHeight()));
    }

    static constexpr auto arrowWidth = 16;

private:

    Path arrowPath;
    Path shadowPath;

    const Colour arrowColour1 = Colour(0x30ffffff);
    const Colour arrowColour2 = Colour(0x0dffffff);
    const Colour shadowColour1 = Colour(0x77000000);
    const Colour shadowColour2 = Colour(0x11000000);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineItemArrow)
};

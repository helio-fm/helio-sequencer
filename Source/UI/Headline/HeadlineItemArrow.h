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

class HeadlineItemArrow final : public Component
{
public:

    explicit HeadlineItemArrow(int arrowWidth = 11) : arrowWidth(arrowWidth)
    {
        this->setOpaque(false);
        this->setBufferedToImage(true);
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setSize(this->arrowWidth, Globals::UI::headlineHeight);
    }

    void paint(Graphics &g) override
    {
        g.setGradientFill(ColourGradient(this->shadowColour1,
            float(this->getWidth() - 2), float(this->getHeight() - 2),
            this->shadowColour2,
            float(this->getWidth() - this->arrowWidth), 2.f,
            true));

        g.strokePath(this->shadowPath, PathStrokeType(1.f));

        g.setGradientFill(ColourGradient(this->arrowColour1,
            float(this->getWidth() - 3), float(this->getHeight() - 2),
            this->arrowColour2,
            float(this->getWidth() - this->arrowWidth), 0.f,
            true));

        g.strokePath(this->arrowPath, PathStrokeType(0.5f));
    }

    void resized() override
    {
        const auto w = float(this->getWidth());
        const auto h = float(this->getHeight());

        this->arrowPath.clear();
        this->arrowPath.startNewSubPath(w - this->arrowWidth, 0.f);
        this->arrowPath.lineTo(w - 1.5f, h);

        this->shadowPath.clear();
        this->shadowPath.startNewSubPath(w - this->arrowWidth + 1.f, 0.f);
        this->shadowPath.lineTo(w - 0.5f, h);
    }

private:

    Path arrowPath;
    Path shadowPath;

    const int arrowWidth;

    const Colour arrowColour1 = findDefaultColour(ColourIDs::Arrow::lineStart);
    const Colour arrowColour2 = findDefaultColour(ColourIDs::Arrow::lineEnd);
    const Colour shadowColour1 = findDefaultColour(ColourIDs::Arrow::shadowStart);
    const Colour shadowColour2 = findDefaultColour(ColourIDs::Arrow::shadowEnd);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineItemArrow)
};

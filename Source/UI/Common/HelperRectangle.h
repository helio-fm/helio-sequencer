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

class HelperRectangle : public Component
{
public:

    HelperRectangle()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::HelperRectangle::fill));
        g.fillRect(this->getLocalBounds());
    }
};

class HelperRectangleVertical final : public HelperRectangle
{
public:
    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::HelperRectangle::fill));
        g.fillRect(this->getLocalBounds());

        g.setColour(findDefaultColour(ColourIDs::HelperRectangle::outline));
        g.drawVerticalLine(0, 0.f, float(this->getHeight()));
        g.drawVerticalLine(this->getWidth() - 1, 0.f, float(this->getHeight()));
    }
};

class HelperRectangleHorizontal final : public HelperRectangle
{
public:
    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::HelperRectangle::fill));
        g.fillRect(this->getLocalBounds());

        g.setColour(findDefaultColour(ColourIDs::HelperRectangle::outline));
        g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
        g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));
    }
};

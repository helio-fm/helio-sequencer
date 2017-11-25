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

class HelperRectangle : public Component
{
public:

    HelperRectangle()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    enum ColourIds
    {
        fillColourId       = 0x99007000,
        outlineColourId    = 0x99007010,
    };

    void paint(Graphics &g) override
    {
        g.setColour(this->findColour(HelperRectangle::fillColourId));
        g.fillRect(0.f, 0.f, float(this->getWidth()), float(this->getHeight()));
    }

};


class HelperRectangleVertical : public HelperRectangle
{
public:
    void paint(Graphics &g) override
    {
        g.setColour(this->findColour(HelperRectangle::fillColourId));
        g.fillRect(0.f, 0.f, float(this->getWidth()), float(this->getHeight()));

        g.setColour(this->findColour(HelperRectangle::outlineColourId));
        g.drawVerticalLine(0, 0.f, float(this->getHeight()));
        g.drawVerticalLine(this->getWidth() - 1, 0.f, float(this->getHeight()));
    }
};

class HelperRectangleHorizontal : public HelperRectangle
{
public:
    void paint(Graphics &g) override
    {
        g.setColour(this->findColour(HelperRectangle::fillColourId));
        g.fillRect(0.f, 0.f, float(this->getWidth()), float(this->getHeight()));

        g.setColour(this->findColour(HelperRectangle::outlineColourId));
        g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
        g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));
    }
};

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

class FloatBoundsComponent : public virtual Component
{
public:

    inline void setFloatBounds(const Rectangle<float> &b)
    {
        const int bX = int(floorf(b.getX()));
        const int bY = int(floorf(b.getY()));
        const int bW = int(ceilf(b.getWidth()));
        const int bH = int(ceilf(b.getHeight()));

        this->floatLocalBounds.setX(b.getX() - bX);
        this->floatLocalBounds.setWidth(b.getWidth());
        this->floatLocalBounds.setY(b.getY() - bY);
        this->floatLocalBounds.setHeight(b.getHeight());

        this->setBounds(bX, bY, bW, bH);
    }

protected:

    Rectangle<float> floatLocalBounds;

};
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

#include "Icons.h"

class ShapeComponent final : public Component
{
public:
    
    explicit ShapeComponent(Icons::Id iconId, float alpha = 1.f) : shape(Icons::getDrawableByName(iconId))
    {
        this->setInterceptsMouseClicks(false, false);
        this->setAlpha(alpha);
    }

    void resized() override
    {
        this->repaint();
    }
    
    void paint(Graphics &g) override
    {
        g.setColour(Colours::black);
        this->shape->drawWithin(g, this->getLocalBounds().toFloat(), RectanglePlacement::centred, 1.f);
        //g.fillPath(this->shape, this->shape.getTransformToScaleToFit(this->getLocalBounds().toFloat(), true, Justification::centred));
    }
    
protected:
    
    const ScopedPointer<Drawable> shape;
    
};

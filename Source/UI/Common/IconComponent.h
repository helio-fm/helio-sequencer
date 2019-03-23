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

class IconComponent : virtual public Component
{
public:
    
    explicit IconComponent(Icons::Id iconId, float alpha = 1.f) :
        iconId(iconId)
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        
        if (alpha < 1.f)
        { this->setAlpha(alpha); }
    }

    explicit IconComponent(Image targetImage) :
        image(targetImage)
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void setIconName(Icons::Id id)
    {
        this->iconId = id;
        this->repaint();
    }

    void setIconImage(Image targetImage)
    {
        this->image = targetImage;
        this->repaint();
    }

    void resized() override
    {
        this->repaint();
    }
    
    void paint(Graphics &g) override
    {
        g.setColour(Colours::black);
        if (this->image.isNull())
        {
            Image i(Icons::findByName(this->iconId, this->getHeight()));
            Icons::drawImageRetinaAware(i, g, this->getWidth() / 2, this->getHeight() / 2);
        }
        else
        {
            Icons::drawImageRetinaAware(this->image, g, this->getWidth() / 2, this->getHeight() / 2);
        }
    }
    
protected:
    
    Icons::Id iconId;
    Image image;
    
};

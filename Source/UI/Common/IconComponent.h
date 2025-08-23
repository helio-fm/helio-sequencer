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

#include "Icons.h"
#include "ColourIDs.h"

class IconComponent : virtual public Component
{
public:
    
    explicit IconComponent(Icons::Id iconId,
        float alpha = 1.f, Optional<int> iconSize = {}) :
        iconId(iconId),
        alpha(alpha),
        iconSize(iconSize)
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setAccessible(false);
    }

    explicit IconComponent(Image targetImage) :
        image(targetImage)
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setAccessible(false);
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

    // where possible, use this instead of setAlpha, which is extremely slow
    void setIconAlphaMultiplier(float alpha)
    {
        this->alpha = alpha;
        this->repaint();
    }

    void resized() override
    {
        this->repaint();
    }
    
    void paint(Graphics &g) override
    {
        g.setColour(this->colour.withMultipliedAlpha(this->alpha));

        if (this->image.isNull())
        {
            const Image i(Icons::findByName(this->iconId,
                this->iconSize.orFallback(jmin(this->getWidth(), this->getHeight()))));

            Icons::drawImageRetinaAware(i, g, this->getWidth() / 2, this->getHeight() / 2);
        }
        else
        {
            Icons::drawImageRetinaAware(this->image, g, this->getWidth() / 2, this->getHeight() / 2);
        }
    }
    
protected:
    
    float alpha = 1.f;
    Icons::Id iconId = Icons::empty;
    Image image;
    Optional<int> iconSize;
    
    const Colour colour = findDefaultColour(Label::textColourId);

};

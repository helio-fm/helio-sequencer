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

#include "ShadowComponent.h"

class ShadowRightwards final : public ShadowComponent
{
public:

    explicit ShadowRightwards(ShadowType type) : ShadowComponent(type) {}

    void paint(Graphics &g)
    {
        g.setTiledImageFill(this->cachedImage, 0, 0, 1.f);
        g.fillRect(this->getLocalBounds());

        g.setColour(this->lineColour);
        g.fillRect(0, 0, 1, this->getHeight());
    }

    void resized()
    {
        if (this->cachedImage.getWidth() != this->getWidth())
        {
            this->cachedImage = Image(Image::ARGB,
                this->getWidth() + ShadowComponent::cachedImageMargin,
                ShadowComponent::cachedImageSize, true);

            Graphics g(this->cachedImage);

            g.setGradientFill(ColourGradient(this->shadowColour,
                0.f, 0.f, Colours::transparentBlack, float(this->getWidth()), 0.f, false));
            g.fillRect(this->cachedImage.getBounds());

            g.setGradientFill(ColourGradient(this->shadowColour,
                0.f, 0.f, Colours::transparentBlack, float(this->getWidth()) / 2.5f, 0.f, false));
            g.fillRect(this->cachedImage.getBounds());
        }
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShadowRightwards)
};

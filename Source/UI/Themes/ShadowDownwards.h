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

#include "ShadowComponent.h"

class ShadowDownwards final : public ShadowComponent
{
public:

    explicit ShadowDownwards(ShadowType type) : ShadowComponent(type) {}

    void paint(Graphics &g)
    {

        g.setTiledImageFill(this->cachedImage, 0, 0, 1.f);
        g.fillRect(this->getLocalBounds());
    }

    void resized()
    {
        if (this->cachedImage.getHeight() != this->getHeight())
        {
            const auto h = float(this->getHeight());

            this->cachedImage = Image(Image::ARGB, ShadowComponent::cachedImageSize,
                this->getHeight() + ShadowComponent::cachedImageMargin, true);

            Graphics g(this->cachedImage);

            g.setGradientFill(ColourGradient(this->shadowColour,
                0.f, 0.f, Colours::transparentBlack, 0.f, h, false));
            g.fillRect(this->cachedImage.getBounds());

            g.setGradientFill(ColourGradient(this->shadowColour,
                0.f, 0.f, Colours::transparentBlack, 0.f, h / 3.f, false));
            g.fillRect(this->cachedImage.getBounds());
        }
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShadowDownwards)
};

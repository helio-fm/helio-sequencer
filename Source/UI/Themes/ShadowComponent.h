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
#include "HelioTheme.h"

enum class ShadowType : int8
{
    Hard,
    Normal,
    Light
};

class ShadowComponent : public Component
{
public:

    explicit ShadowComponent(ShadowType type)
    {
        this->setOpaque(false);
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

        switch (type)
        {
        case ShadowType::Hard:
            this->shadowColour = findDefaultColour(ColourIDs::Shadows::fillHard);
            this->lineColour = findDefaultColour(ColourIDs::Shadows::borderHard);
            break;
        case ShadowType::Normal:
            this->shadowColour = findDefaultColour(ColourIDs::Shadows::fillNormal);
            this->lineColour = findDefaultColour(ColourIDs::Shadows::borderNormal);
            break;
        case ShadowType::Light:
            this->shadowColour = findDefaultColour(ColourIDs::Shadows::fillLight);
            this->lineColour = findDefaultColour(ColourIDs::Shadows::borderLight);
            break;
        default:
            this->shadowColour = Colours::transparentBlack;
            this->lineColour = findDefaultColour(ColourIDs::Common::borderLineDark);
            break;
        }

        // shadow debug
        //this->shadowColour = Colours::limegreen;
        //this->lineColour = Colours::red;
    }

protected:

    static constexpr auto cachedImageSize = 32;
    // a way to fix OpenGL non-pow-of-2 texture artifacts
    static constexpr auto cachedImageMargin = 4;

    Colour lineColour;
    Colour shadowColour;

    Image cachedImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShadowComponent)
};

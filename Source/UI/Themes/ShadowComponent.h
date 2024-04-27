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

enum class ShadowType : int8
{
    Hard,
    Normal,
    Light
};

class ShadowComponent : public Component
{
public:

    ShadowComponent(ShadowType type)
    {
        this->setOpaque(false);
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

        const auto shadowMultiplier = HelioTheme::getCurrentTheme().isDark() ? 1.f : 0.3f;

        switch (type)
        {
        case ShadowType::Hard:
            this->shadowColour = Colours::black.withAlpha(shadowMultiplier * 0.15f);
            this->lineColour = findDefaultColour(ColourIDs::Common::borderLineDark)
                .withMultipliedAlpha(shadowMultiplier * 0.75f);
            break;
        case ShadowType::Normal:
            this->shadowColour = Colours::black.withAlpha(shadowMultiplier * 0.1f);
            this->lineColour = findDefaultColour(ColourIDs::Common::borderLineDark)
                .withMultipliedAlpha(shadowMultiplier * 0.5f);
            break;
        case ShadowType::Light:
            this->shadowColour = Colours::black.withAlpha(shadowMultiplier * 0.045f);
            this->lineColour = findDefaultColour(ColourIDs::Common::borderLineDark)
                .withMultipliedAlpha(shadowMultiplier * 0.35f);
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

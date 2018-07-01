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

enum ShadowType
{
    Normal,
    Light
};

class ShadowComponent : public Component
{
public:

    ShadowComponent(ShadowType type)
    {
        this->setOpaque(false);
        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
        this->setMouseClickGrabsKeyboardFocus(false);

        switch (type)
        {
        case Normal:
            this->shadowColour = Colour(0x16000000);
            break;
        case Light:
            this->shadowColour = Colour(0x09000000);
            break;
        default:
            this->shadowColour = Colours::transparentBlack;
            break;
        }
    }

protected:

    Colour shadowColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShadowComponent)
};

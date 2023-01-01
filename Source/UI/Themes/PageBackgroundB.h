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

#include "HelioTheme.h"
#include "ColourIDs.h"

class PageBackgroundB final : public Component
{
public:

    PageBackgroundB()
    {
        this->setOpaque(true);
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        const auto &theme = HelioTheme::getCurrentTheme();
        //if (theme.getBgCacheB().isValid())
        //{
        g.setTiledImageFill(theme.getPageBackgroundB(), 0, 0, 1.f);
        g.fillRect(this->getLocalBounds());
        //}
        //else
        //{
        //    g.setColour(findDefaultColour(ColourIDs::BackgroundB::fill));
        //    g.fillRect(this->getLocalBounds());
        //}
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PageBackgroundB)
};

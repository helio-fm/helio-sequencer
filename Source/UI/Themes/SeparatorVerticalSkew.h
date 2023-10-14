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

#include "HelioTheme.h"

class SeparatorVerticalSkew final : public Component
{
public:

    SeparatorVerticalSkew()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        const auto &theme = HelioTheme::getCurrentTheme();

        if (theme.getPageBackgroundA().isValid())
        {
            g.setTiledImageFill(theme.getPageBackgroundA(), 0, 0, 1.f);
            g.fillPath(this->shape2, {});
        }

        if (theme.getPageBackgroundB().isValid())
        {
            g.setTiledImageFill(theme.getPageBackgroundB(), 0, 0, 1.f);
            g.fillPath(this->shape1, {});
        }

        g.setColour(Colours::black.withAlpha(45.f / 255.f));
        g.fillPath(this->line1, {});

        g.setColour(Colours::white.withAlpha(15.f / 255.f));
        g.fillPath(this->line2, {});
    }

    void resized() override
    {
        const float h = float(this->getHeight());
        const float w = float(this->getWidth());

        this->line1.clear();
        this->line1.addLineSegment({ w, 0.f, 1.f, h }, 0.5f);

        this->line2.clear();
        this->line2.addLineSegment({ w - 1.f, 0.f, 0.f, h }, 0.75f);

        this->shape1.clear();
        this->shape1.startNewSubPath(float(this->getWidth()), 0.f);
        this->shape1.lineTo(float(this->getWidth()), float(this->getHeight()));
        this->shape1.lineTo(-1.f, float(this->getHeight()));
        this->shape1.closeSubPath();

        this->shape2.clear();
        this->shape2.startNewSubPath(float(this->getWidth() + 1), 0.f);
        this->shape2.lineTo(0.f, 0.f);
        this->shape2.lineTo(0.f, float(this->getHeight()));
        this->shape2.closeSubPath();
    }

private:

    Path line1;
    Path line2;

    Path shape1;
    Path shape2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeparatorVerticalSkew)
};

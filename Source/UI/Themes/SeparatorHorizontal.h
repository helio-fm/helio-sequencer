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

class SeparatorHorizontal final : public Component
{
public:

    SeparatorHorizontal()
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    ~SeparatorHorizontal() override = default;

    void setAlphaMultiplier(float a)
    {
        this->alphaFactor = a;
        this->repaint();
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::black.withAlpha(11.f / 255.f * this->alphaFactor));
        g.fillRect(0, 0, this->getWidth(), 1);

        g.setColour(Colours::white.withAlpha(9.f / 255.f * this->alphaFactor));
        g.fillRect(0, 1, this->getWidth(), 1);
    }
    
private:

    float alphaFactor = 1.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeparatorHorizontal)
};

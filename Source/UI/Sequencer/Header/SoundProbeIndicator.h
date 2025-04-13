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

class SoundProbeIndicator final : public Component
{
public:

    SoundProbeIndicator() :
        colour(findDefaultColour(ColourIDs::RollHeader::soundProbe)),
        shadowColour(findDefaultColour(ColourIDs::Common::borderLineDark).withMultipliedAlpha(0.5f))
    {
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
        this->setAlwaysOnTop(true);
        this->setSize(3, 3);
    }

    ~SoundProbeIndicator() override
    {
        App::animateComponent(this,
            this->getBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
    }

    double getAnchor() const noexcept
    {
        return this->absPosition;
    }

    inline void setAnchoredAt(double absX)
    {
        this->absPosition = absX;
        this->updateBounds();
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->shadowColour);
        g.fillRect(0, 1, 1, this->getHeight());
        g.fillRect(2, 1, 1, this->getHeight());

        g.setColour(this->colour);
        g.fillRect(0, 1, 2, this->getHeight());
    }

    void parentHierarchyChanged() override
    {
        this->updateBounds();
    }

    void parentSizeChanged() override
    {
        this->updateBounds();
    }

private:

    const Colour colour;
    const Colour shadowColour;

    double absPosition = 0.0;

    void updateBounds()
    {
        const int x = int(round(double(this->getParentWidth()) *
            this->absPosition - double(this->getWidth()) / 2.0) + 1.f);

        this->setBounds(x, 0, this->getWidth(), this->getParentHeight());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundProbeIndicator)
};

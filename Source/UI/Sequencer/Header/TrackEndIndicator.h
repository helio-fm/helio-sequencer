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

#include "ShadowRightwards.h"

class TrackEndIndicator final : public Component
{
public:

    TrackEndIndicator()
    {
        this->shadow = make<ShadowRightwards>(ShadowType::Light);
        this->addAndMakeVisible(this->shadow.get());

        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
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
        g.setColour(Colour(0x1a000000));
        g.fillRect(this->getLocalBounds());
    }

    void resized() override
    {
        this->shadow->setBounds(0, 0, 12, this->getHeight());
    }

    void parentHierarchyChanged() override
    {
        this->updateBounds();
    }

private:

    double absPosition = 0.0;

    void updateBounds()
    {
        this->toFront(false);
        this->setBounds(int(double(this->getParentWidth()) * this->absPosition + 1.0), 0,
            int(double(this->getParentWidth()) * (1.0 - this->absPosition) + 1.0),
            this->getParentHeight());
    }

    UniquePointer<ShadowRightwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackEndIndicator)
};

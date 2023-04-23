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

#include "CenteredTooltipComponent.h"
#include "ColourIDs.h"
#include "Icons.h"

class FailTooltip final : public CenteredTooltipComponent,
                          private Timer
{
public:

    FailTooltip() : iconShape(Icons::getPathByName(Icons::fail))
    {
        this->setSize(FailTooltip::tooltipSize, FailTooltip::tooltipSize);
        this->startTimer(FailTooltip::onScreenTimeMs);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->backgroundColour);
        g.fillRoundedRectangle(this->getLocalBounds().toFloat(), 15.f);

        g.setColour(this->textColour);
        const Rectangle<int> imageBounds(0, 0, FailTooltip::imageSize, FailTooltip::imageSize);
        g.fillPath(this->iconShape, this->iconShape.getTransformToScaleToFit(
            imageBounds.withCentre(this->getLocalBounds().getCentre()).toFloat(),
            true, Justification::centred));
    }

    void parentHierarchyChanged() override
    {
        this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
    }

private:

    static constexpr auto onScreenTimeMs = 1000;
    static constexpr auto tooltipSize = 96;
    static constexpr auto imageSize = 40;

    const Colour backgroundColour =
        findDefaultColour(ColourIDs::Tooltip::failIconFill);

    const Colour textColour =
        findDefaultColour(ColourIDs::Tooltip::failIconForeground);

    void timerCallback() override
    {
        this->stopTimer();
        this->dismiss();
    }

    Path iconShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FailTooltip)
};

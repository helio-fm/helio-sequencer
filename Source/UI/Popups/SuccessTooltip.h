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

#include "CenteredTooltipComponent.h"
#include "ColourIDs.h"
#include "Icons.h"

class SuccessTooltip final : public CenteredTooltipComponent,
                             private Timer
{
public:

    SuccessTooltip() : iconShape(Icons::getPathByName(Icons::success))
    {
        this->setSize(SuccessTooltip::tooltipSize, SuccessTooltip::tooltipSize);
        this->startTimer(SuccessTooltip::onScreenTimeMs);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->backgroundColour);
        g.fillRoundedRectangle(this->getLocalBounds().toFloat(), 15.f);

        g.setColour(this->textColour);
        Rectangle<int> imageBounds(0, 0, SuccessTooltip::imageSize, SuccessTooltip::imageSize);
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
        findDefaultColour(ColourIDs::Tooltip::okIconFill);

    const Colour textColour =
        findDefaultColour(ColourIDs::Tooltip::okIconForeground);

    void timerCallback() override
    {
        this->stopTimer();
        this->dismiss();
    }

    Path iconShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuccessTooltip)
};

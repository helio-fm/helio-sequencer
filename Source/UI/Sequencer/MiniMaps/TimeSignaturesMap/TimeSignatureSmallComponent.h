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

#include "ColourIDs.h"
#include "TimeSignatureComponent.h"

class TimeSignatureSmallComponent final : public TimeSignatureComponent
{
public:

    TimeSignatureSmallComponent(TimeSignaturesProjectMap &parent,
        const TimeSignatureEvent &targetEvent);

    ~TimeSignatureSmallComponent();

    void updateContent() override;
    void setRealBounds(const Rectangle<float> bounds) override;

    void paint(Graphics &g) override;
    void parentHierarchyChanged() override;

    float getTextWidth() const noexcept override
    {
        // fixed size (it doesn't matter much on the mini-map):
        return 16;
    };

private:

    UniquePointer<Label> signatureLabel;

    Rectangle<float> boundsOffset;

    const Colour colour = findDefaultColour(ColourIDs::TrackScroller::scrollerFill);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignatureSmallComponent)
};

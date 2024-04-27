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

#include "TimeSignatureComponent.h"

class TimeSignatureSmallComponent final : public TimeSignatureComponent
{
public:

    explicit TimeSignatureSmallComponent(TimeSignaturesProjectMap &parent);
    ~TimeSignatureSmallComponent();

    void updateContent(const TimeSignatureEvent &newEvent) override;

    void paint(Graphics &g) override;
    void parentHierarchyChanged() override;

    float getTextWidth() const noexcept override
    {
        // fixed size (it doesn't matter much on the mini-map):
        return 16;
    };

private:

    UniquePointer<Label> signatureLabel;

    Colour colour;
    Path triangleShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignatureSmallComponent)
};

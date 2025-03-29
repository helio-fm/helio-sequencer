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

class TimeSignatureLargeComponent final : public TimeSignatureComponent
{
public:

    explicit TimeSignatureLargeComponent(TimeSignaturesProjectMap &parent);
    ~TimeSignatureLargeComponent();

    void updateContent(const TimeSignatureEvent &newEvent) override;
    float getTextWidth() const noexcept override;

    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;

private:

    ComponentDragger dragger;

    bool draggingState = false;
    bool draggingHadCheckpoint = false;
    float draggingAnchorBeat = 0.f;

    Colour colour;
    Path triangleShape;

    static constexpr float fillUnfocusedAlpha = 0.6f;
    static constexpr float fillFocusedAlpha = 0.8f;

    float fillAlpha = fillUnfocusedAlpha;

    UniquePointer<Label> signatureLabel;
    float textWidth = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignatureLargeComponent)
};

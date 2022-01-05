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

#include "TimeSignatureComponent.h"

class TimeSignatureLargeComponent final : public TimeSignatureComponent
{
public:

    explicit TimeSignatureLargeComponent(TimeSignaturesProjectMap &parent);
    ~TimeSignatureLargeComponent();

    void updateContent(const TimeSignatureEvent &newEvent) override;
    void setRealBounds(const Rectangle<float> bounds) override;
    float getTextWidth() const noexcept override;

    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    ComponentDragger dragger;
    TimeSignatureEvent anchor;
    
    Rectangle<float> boundsOffset;
    Point<int> clickOffset;

    bool draggingState = false;
    bool draggingHadCheckpoint = false;

    // workaround странного поведения juce
    // возможна ситуация, когда mousedown'а не было, а mouseup срабатывает
    bool mouseDownWasTriggered = false;

    UniquePointer<Label> signatureLabel;
    float textWidth = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeSignatureLargeComponent)
};

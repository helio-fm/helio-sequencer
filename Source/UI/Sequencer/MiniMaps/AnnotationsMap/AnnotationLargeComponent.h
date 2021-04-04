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

#include "AnnotationComponent.h"

class AnnotationLargeComponent final : public AnnotationComponent
{
public:

    AnnotationLargeComponent(AnnotationsProjectMap &parent, const AnnotationEvent &targetEvent);
    ~AnnotationLargeComponent();

    enum class State : uint8
    {
        None,
        Dragging,
        ResizingRight
    };

    float getTextWidth() const noexcept override;
    void updateContent() override;
    void setRealBounds(const Rectangle<float> bounds) override;

    void paint(Graphics &g) override;
    void mouseMove(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

    static constexpr auto annotationHeight = 32;

private:

    static constexpr auto borderResizingArea = 10;

    ComponentDragger dragger;

    Rectangle<float> boundsOffset;
    Point<int> clickOffset;

    State state = State::None;
    bool hadCheckpoint = false;

    Font font;
    String text;
    float textWidth = 0.f;

    // workaround странного поведения juce
    // возможна ситуация, когда mousedown'а не было, а mouseup срабатывает
    bool mouseDownWasTriggered = false;

    bool canResize() const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnnotationLargeComponent)
};

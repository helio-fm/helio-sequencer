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

class TooltipContainer final : public Component, private Timer
{
public:

    TooltipContainer();
    ~TooltipContainer();

    void showWithComponent(UniquePointer<Component> targetComponent,
        int timeOutMs = -1);

    void showWithComponent(UniquePointer<Component> targetComponent,
        Rectangle<int> callerScreenBounds, int timeOutMs = -1);

    void hide();

    void paint(Graphics &g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;

private:

    int hideTimeout = -1;
    int timeCounter = -1;
    bool alignedToBottom = true;
    int clicksCountOnStart = 0;

    ComponentAnimator animator;

    void timerCallback() override;
    void updatePosition();

    UniquePointer<Component> tooltipComponent;

    static constexpr auto timerMs = 100;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TooltipContainer)
};

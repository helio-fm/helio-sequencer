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

//[Headers]
//[/Headers]


class TooltipContainer final : public Component,
                               private Timer
{
public:

    TooltipContainer();
    ~TooltipContainer();

    //[UserMethods]

    // set -1 timeout if it should not hide
    void showWithComponent(ScopedPointer<Component> newTargetComponent, int timeOutMs = -1);

    void showWithComponent(ScopedPointer<Component> newTargetComponent,
                           Rectangle<int> callerScreenBounds, int timeOutMs = -1);

    void hide();

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;


private:

    //[UserVariables]

    int hideTimeout;
    int timeCounter;
    bool alignedToBottom;
    int clicksCountOnStart;

    ComponentAnimator animator;

    void timerCallback() override;
    void updatePosition();

    //[/UserVariables]

    UniquePointer<Component> tooltipComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipContainer)
};

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

#include "AutomationEvent.h"
#include "FineTuningComponentDragger.h"
#include "FineTuningValueIndicator.h"
#include "ComponentFader.h"

class AutomationCurveClipComponent;

class AutomationCurveHelper final : public Component
{
public:

    AutomationCurveHelper(const AutomationEvent &event, const AutomationCurveClipComponent &editor, Component *target1, Component *target2);

    float getCurvature() const;

    void paint(Graphics &g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    const AutomationEvent &event;
    const AutomationCurveClipComponent &editor;

    Point<int> anchor;
    float curveAnchor;
    bool draggingState;
    FineTuningComponentDragger dragger;

    ScopedPointer<FineTuningValueIndicator> tuningIndicator;
    ComponentFader fader;

    SafePointer<Component> component1;
    SafePointer<Component> component2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationCurveHelper)
};

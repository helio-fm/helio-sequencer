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
#include "AutomationEvent.h"

class ComponentConnectorCurve;
class AutomationTrackMap;
//[/Headers]


class AutomationCurveHelper  : public Component
{
public:

    AutomationCurveHelper (AutomationTrackMap &parent, const AutomationEvent &targetEvent, Component *target1, Component *target2);

    ~AutomationCurveHelper();

    //[UserMethods]
    float getCurvature() const;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    bool hitTest (int x, int y) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;


private:

    //[UserVariables]

    const AutomationEvent &event;
    AutomationTrackMap &editor;

    ComponentDragger dragger;
    bool draggingState;
    Point<int> anchor;
    float curveAnchor;

    float constrainPosition();

    SafePointer<Component> component1;
    SafePointer<Component> component2;

    //[/UserVariables]


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationCurveHelper)
};

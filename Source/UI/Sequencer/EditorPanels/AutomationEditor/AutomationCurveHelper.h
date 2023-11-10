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

#include "AutomationEvent.h"
#include "AutomationEditorBase.h"
#include "FineTuningComponentDragger.h"
#include "FineTuningValueIndicator.h"
#include "ComponentFader.h"

class AutomationCurveHelper final : public Component
{
public:

    AutomationCurveHelper(const AutomationEvent &event,
        AutomationEditorBase::EventComponentBase *target1,
        AutomationEditorBase::EventComponentBase *target2);

    float getCurvature() const;

    void setEditable(bool shouldBeEditable);

    void paint(Graphics &g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    
private:

    const AutomationEvent &event;

    Point<int> anchor;
    float curveAnchor = 0.f;

    FineTuningComponentDragger dragger;

    bool isDragging = false;
    bool isEditable = true;

    UniquePointer<FineTuningValueIndicator> tuningIndicator;
    ComponentFader fader;

    SafePointer<AutomationEditorBase::EventComponentBase> component1;
    SafePointer<AutomationEditorBase::EventComponentBase> component2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationCurveHelper)
};

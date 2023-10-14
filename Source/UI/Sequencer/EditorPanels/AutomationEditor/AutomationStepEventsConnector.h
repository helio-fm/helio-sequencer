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

#include "AutomationEditorBase.h"

class AutomationStepEventsConnector final : public Component
{
public:

    AutomationStepEventsConnector(AutomationEditorBase::EventComponentBase *c1,
        AutomationEditorBase::EventComponentBase *c2,
        bool isEventTriggered);
    
    void retargetAndUpdate(AutomationEditorBase::EventComponentBase *c1,
        AutomationEditorBase::EventComponentBase *c2,
        bool isEventTriggered);

    void resizeToFit(bool isEventTriggered);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void paint(Graphics &g) override;
    //void mouseMove(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;

private:

    void getPoints(float &x1, float &x2, float &y1, float &y2) const;

    SafePointer<AutomationEditorBase::EventComponentBase> component1;
    SafePointer<AutomationEditorBase::EventComponentBase> component2;
    
    AutomationEditorBase::EventComponentBase *firstAliveEventComponent() const;

    bool isEventTriggered = false;
    bool isHighlighted = false;

    Rectangle<float> realBounds;
    
    ComponentAnimator animator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationStepEventsConnector)
};

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

class AutomationStepEventsConnector;
class AutomationStepsClipComponent;

#define STEP_EVENT_POINT_OFFSET (2.5f)
#define STEP_EVENT_MIN_LENGTH_IN_BEATS (0.25f)
#define STEP_EVENT_MARGIN_TOP (16.f)
#define STEP_EVENT_MARGIN_BOTTOM (16.f)
#define STEP_EVENT_THICK_LINES 0

class AutomationStepEventComponent final : public Component
{
public:

    AutomationStepEventComponent(AutomationStepsClipComponent &parent, const AutomationEvent &targetEvent);
    
    inline AutomationStepsClipComponent *getEditor() const noexcept { return &this->editor; }
    inline const AutomationEvent &getEvent() const noexcept { return this->event; };

    bool isPedalDownEvent() const noexcept;
    float getBeat() const noexcept;

    void setRealBounds(const Rectangle<float> bounds);
    Rectangle<float> getRealBounds() const noexcept;
    inline bool hasCompactMode() const noexcept
    { return this->realBounds.getWidth() <= 2.f; }

    void updateConnector();
    void setNextNeighbour(AutomationStepEventComponent *next);
    void setPreviousNeighbour(AutomationStepEventComponent *prev);

    static int compareElements(const AutomationStepEventComponent *first,
                               const AutomationStepEventComponent *second)
    {
        if (first == second) { return 0; }

        const float diff = first->event.getBeat() - second->event.getBeat();
        const int diffResult = (diff > 0.f) - (diff < 0.f);
        if (diffResult != 0) { return diffResult; }

        return first->event.getId().compare(second->event.getId());
    }

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void moved() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;

private:

    void drag(float targetBeat);
    void dragByDelta(float deltaBeat);

    const AutomationEvent &event;
    AutomationStepsClipComponent &editor;

    Rectangle<float> realBounds;

    ComponentDragger dragger;
    bool isDragging;
    bool isHighlighted;

    void recreateConnector();

    ScopedPointer<AutomationStepEventsConnector> connector;
    SafePointer<AutomationStepEventComponent> nextEventHolder;
    SafePointer<AutomationStepEventComponent> prevEventHolder;

    friend class AutomationStepEventsConnector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationStepEventComponent)
};

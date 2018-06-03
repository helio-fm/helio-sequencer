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

class AutomationStepEventsConnector;
class AutomationStepsClipComponent;
//[/Headers]


class AutomationStepEventComponent final : public Component
{
public:

    AutomationStepEventComponent(AutomationStepsClipComponent &parent, const AutomationEvent &targetEvent);
    ~AutomationStepEventComponent();

    //[UserMethods]

    static float getAnchor();

    inline AutomationStepsClipComponent *getEditor() const noexcept { return &this->editor; }
    inline const AutomationEvent &getEvent() const noexcept { return this->event; };

    bool isPedalDownEvent() const;
    float getBeat() const;

    void setRealBounds(const Rectangle<float> bounds);
    Rectangle<float> getRealBounds() const;

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

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void moved() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;


private:

    //[UserVariables]

    void drag(float targetBeat);
    void dragByDelta(float deltaBeat);

    const AutomationEvent &event;
    AutomationStepsClipComponent &editor;

    Rectangle<float> realBounds;

    ComponentDragger dragger;
    bool draggingState;

    void recreateConnector();

    ScopedPointer<AutomationStepEventsConnector> connector;
    SafePointer<AutomationStepEventComponent> nextEventHolder;
    SafePointer<AutomationStepEventComponent> prevEventHolder;

    friend class AutomationStepEventsConnector;

    //[/UserVariables]

    Path internalPath1;
    Path internalPath2;
    Path internalPath3;
    Path internalPath4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationStepEventComponent)
};

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

class TriggerEventConnector;
class TriggersTrackMap;
//[/Headers]


class TriggerEventComponent  : public Component
{
public:

    TriggerEventComponent (TriggersTrackMap &parent, const AutomationEvent &targetEvent);

    ~TriggerEventComponent();

    //[UserMethods]

    static float getAnchor();

    TriggersTrackMap *getEditor() const
    {
        return &this->editor;
    }

    bool isPedalDownEvent() const;
    float getBeat() const;

    void setRealBounds(const Rectangle<float> bounds);
    Rectangle<float> getRealBounds() const;

    void updateConnector();
    void setNextNeighbour(TriggerEventComponent *next);
    void setPreviousNeighbour(TriggerEventComponent *prev);

    static int compareElements(const TriggerEventComponent *first,
                               const TriggerEventComponent *second)
    {
        if (first == second) { return 0; }

        const float diff = first->event.getBeat() - second->event.getBeat();
        const int diffResult = (diff > 0.f) - (diff < 0.f);
        if (diffResult != 0) { return diffResult; }

        return first->event.getID().compare(second->event.getID());
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
    TriggersTrackMap &editor;

    Rectangle<float> realBounds;

    ComponentDragger dragger;
    bool draggingState;

    void recreateConnector();

    ScopedPointer<TriggerEventConnector> connector;
    SafePointer<TriggerEventComponent> nextEventHolder;
    SafePointer<TriggerEventComponent> prevEventHolder;

    friend class TriggerEventConnector;

    //[/UserVariables]

    Path internalPath1;
    Path internalPath2;
    Path internalPath3;
    Path internalPath4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriggerEventComponent)
};

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

class AutomationEventsConnector;
class AutomationCurveHelper;
class AutomationTrackMap;
//[/Headers]


class AutomationEventComponent  : public Component
{
public:

    AutomationEventComponent (AutomationTrackMap &parent, const AutomationEvent &targetEvent);

    ~AutomationEventComponent();

    //[UserMethods]

    inline float getControllerValue() const
    { return this->event.getControllerValue(); }

    inline float getBeat() const
    { return this->event.getBeat(); }

    void updateConnector();
    void updateHelper();
    void setNextNeighbour(AutomationEventComponent *next);

    static int compareElements(const AutomationEventComponent *first, const AutomationEventComponent *second)
    {
        if (first == second) { return 0; }

        const float beatDiff = first->getBeat() - second->getBeat();
        const int beatResult = (beatDiff > 0.f) - (beatDiff < 0.f);
        if (beatResult != 0) { return beatResult; }

        const float cvDiff = first->getControllerValue() - second->getControllerValue();
        const int cvResult = (cvDiff > 0.f) - (cvDiff < 0.f); // sorted by cv, if beats are the same
        if (cvResult != 0) { return cvResult; }

        return first->event.getId().compare(second->event.getId());
    }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void moved() override;
    bool hitTest (int x, int y) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;


private:

    //[UserVariables]

    void startDragging();
    bool isDragging() const;
    bool getDraggingDelta(const MouseEvent &e, float &deltaBeat, float &deltaValue);
    AutomationEvent continueDragging(const float deltaBeat, const float deltaValue);
    void endDragging();

    friend class AutomationTrackMap;

    const AutomationEvent &event;
    AutomationTrackMap &editor;

    ComponentDragger dragger;
    AutomationEvent anchor;

    Point<int> clickOffset;
    bool draggingState;

    void recreateConnector();
    void recreateHelper();

    ScopedPointer<AutomationEventsConnector> connector;
    ScopedPointer<AutomationCurveHelper> helper;
    SafePointer<AutomationEventComponent> nextEventHolder;

    //[/UserVariables]


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationEventComponent)
};

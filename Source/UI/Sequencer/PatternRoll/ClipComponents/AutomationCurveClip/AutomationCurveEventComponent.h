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

class AutomationCurveEventsConnector;
class AutomationCurveHelper;
class AutomationCurveClipComponent;

class AutomationCurveEventComponent final : public Component
{
public:

    AutomationCurveEventComponent(AutomationCurveClipComponent &parent, const AutomationEvent &targetEvent);

    inline float getBeat() const noexcept { return this->event.getBeat(); }
    inline float getControllerValue() const noexcept { return this->event.getControllerValue(); }
    inline const AutomationEvent &getEvent() const noexcept { return this->event; };

    void updateConnector();
    void updateHelper();
    void setNextNeighbour(AutomationCurveEventComponent *next);

    static int compareElements(const AutomationCurveEventComponent *first,
        const AutomationCurveEventComponent *second);

    void paint (Graphics& g) override;
    bool hitTest (int x, int y) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

private:

    void startDragging();
    bool isDragging() const;
    AutomationEvent continueDragging(const float deltaBeat, const float deltaValue);
    void getDraggingDeltas(const MouseEvent &e,
        float &deltaBeat, float &deltaValue,
        bool &beatChanged, bool &valueChanged);
    void endDragging();

    friend class AutomationCurveClipComponent;

    const AutomationEvent &event;
    AutomationCurveClipComponent &editor;

    AutomationEvent anchor;
    FineTuningComponentDragger dragger;

    ScopedPointer<FineTuningValueIndicator> tuningIndicator;
    ComponentFader fader;

    Point<int> clickOffset;
    bool draggingState;

    const int controllerNumber;
    bool isTempoCurve() const noexcept;

    void recreateConnector();
    void recreateHelper();

    ScopedPointer<AutomationCurveEventsConnector> connector;
    ScopedPointer<AutomationCurveHelper> helper;
    SafePointer<AutomationCurveEventComponent> nextEventHolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationCurveEventComponent)
};

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

#include "Clip.h"
#include "AutomationEvent.h"
#include "AutomationEditorBase.h"
#include "FineTuningComponentDragger.h"
#include "FineTuningValueIndicator.h"
#include "ComponentFader.h"

class AutomationCurveEventsConnector;
class AutomationCurveHelper;

class AutomationCurveEventComponent final :
    public AutomationEditorBase::EventComponentBase
{
public:

    AutomationCurveEventComponent(AutomationEditorBase &editor,
        const AutomationEvent &event,
        const Clip &clip);

    inline float getControllerValue() const noexcept
    {
        return this->event.getControllerValue();
    }

    bool isInEditMode() const;
    MouseCursor startEditingNewEvent(const MouseEvent &e);

    void startDragging();
    AutomationEvent continueDragging(const float deltaBeat, const float deltaValue);
    void getDraggingDeltas(const MouseEvent &e,
        float &deltaBeat, float &deltaValue,
        bool &beatChanged, bool &valueChanged);
    void endDragging();

    //===------------------------------------------------------------------===//
    // EventComponentBase
    //===------------------------------------------------------------------===//

    const Clip &getClip() const noexcept override
    {
        return this->clip;
    };

    const AutomationEvent &getEvent() const noexcept override
    {
        return this->event;
    };

    const Colour &getColour() const noexcept override
    {
        return this->colour;
    }

    void setEditable(bool shouldBeEditable) override;

    void setNextNeighbour(EventComponentBase *next) override;
    void setPreviousNeighbour(EventComponentBase *prev) override;

    SafePointer<EventComponentBase> getNextNeighbour() const noexcept override
    {
        return this->nextEventHolder;
    }

    SafePointer<EventComponentBase> getPreviousNeighbour() const noexcept override
    {
        return this->prevEventHolder;
    }

    void updateChildrenBounds() override
    {
        this->updateConnector();
        this->updateHelper();
    }

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    bool hitTest(int x, int y) override;
    void parentHierarchyChanged() override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    AutomationEditorBase &editor;

    const AutomationEvent &event;
    const Clip &clip;

    AutomationEvent anchor;

    Colour colour;
    void updateColour() override;

    FineTuningComponentDragger dragger;
    UniquePointer<FineTuningValueIndicator> tuningIndicator;
    ComponentFader fader;

    Point<int> clickOffset;
    bool isDragging = false;
    bool anyChangeDone = false;

    bool isHighlighted = false;
    bool isEditable = true;

    const int controllerNumber;
    bool isTempoCurve() const noexcept;

    void updateConnector();
    void updateHelper();

    void recreateConnector();
    void recreateHelper();

    UniquePointer<AutomationCurveEventsConnector> connector;
    UniquePointer<AutomationCurveHelper> helper;

    SafePointer<EventComponentBase> nextEventHolder;
    SafePointer<EventComponentBase> prevEventHolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationCurveEventComponent)
};

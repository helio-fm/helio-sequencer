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

class AutomationStepEventsConnector;

class AutomationStepEventComponent final :
    public AutomationEditorBase::EventComponentBase
{
public:

    AutomationStepEventComponent(AutomationEditorBase &editor,
        const AutomationEvent &event,
        const Clip &clip);

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
    }

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void moved() override;
    bool hitTest(int x, int y) override;
    void parentHierarchyChanged() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
    
    static constexpr auto pointOffset = 2.5f;
    static constexpr auto minLengthInBeats = 0.25f;
    static constexpr auto marginTop = 20.f;
    static constexpr auto marginBottom = 18.f;

private:

    void drag(float targetBeat);
    void dragByDelta(float deltaBeat);

    AutomationEditorBase &editor;

    const AutomationEvent &event;
    const Clip &clip;

    Colour colour;
    void updateColour() override;

    ComponentDragger dragger;
    bool isDragging = false;
    bool anyChangeDone = false;

    bool isHighlighted = false;
    bool isEditable = true;

    void updateConnector();
    void recreateConnector();

    UniquePointer<AutomationStepEventsConnector> connector;

    SafePointer<EventComponentBase> nextEventHolder;
    SafePointer<EventComponentBase> prevEventHolder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomationStepEventComponent)
};

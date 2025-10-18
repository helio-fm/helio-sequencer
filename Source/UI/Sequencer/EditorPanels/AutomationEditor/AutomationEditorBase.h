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
#include "FloatBoundsComponent.h"
#include "RollEditMode.h"

// Inherited by clip components in the pattern roll
// and by the automation editor in the bottom panel:
class AutomationEditorBase
{
public:

    virtual ~AutomationEditorBase() = default;

    virtual Colour getColour(const AutomationEvent &event) const = 0;
    virtual Rectangle<float> getEventBounds(const AutomationEvent &event, const Clip &clip) const = 0;

    // return beat relative to sequence so it can be used in sequence->change(...)
    virtual float getBeatByPosition(int x, const Clip &clip) const = 0;
    virtual void getBeatValueByPosition(int x, int y, const Clip &clip, float &outValue, float &outBeat) const = 0;

    virtual bool hasEditMode(RollEditMode::Mode mode) const noexcept = 0;
    virtual bool isMultiTouchEvent(const MouseEvent &event) const noexcept = 0;

    // All common stuff for automation event components:
    // first, they maintain connector components between them,
    // so they need a way to know who are their neighbours,
    // also the editor keeps them sorted for faster access,
    // hence the comparator method and some getters
    class EventComponentBase : public virtual FloatBoundsComponent
    {
    public:

        virtual ~EventComponentBase() = default;

        virtual void setNextNeighbour(EventComponentBase *next) = 0;
        virtual void setPreviousNeighbour(EventComponentBase *prev) = 0;

        virtual SafePointer<EventComponentBase> getNextNeighbour() const noexcept = 0;
        virtual SafePointer<EventComponentBase> getPreviousNeighbour() const noexcept = 0;

        virtual const Clip &getClip() const noexcept = 0;
        virtual const AutomationEvent &getEvent() const noexcept = 0;

        virtual const Colour &getColour() const noexcept = 0;
        virtual void updateColour() = 0;

        virtual void setEditable(bool editable) = 0;

        // resize all connectors and other helpers
        virtual void updateChildrenBounds() = 0;

        static int compareElements(const EventComponentBase *first,
            const EventComponentBase *second)
        {
            if (first == second) { return 0; }

            const float beatDiff = (first->getEvent().getBeat() + first->getClip().getBeat()) -
                (second->getEvent().getBeat() + second->getClip().getBeat());

            const int diffResult = (beatDiff > 0.f) - (beatDiff < 0.f);
            if (diffResult != 0) { return diffResult; }

            const auto cvDiff = first->getEvent().getControllerValue() -
                second->getEvent().getControllerValue();
            const int cvResult = (cvDiff > 0) - (cvDiff < 0);
            if (cvResult != 0) { return cvResult; }

            return first->getEvent().getId() - second->getEvent().getId();
        }
    };
};

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

#include "Common.h"
#include "DraggingListBoxComponent.h"
#include "ViewportKineticSlider.h"

DraggingListBoxComponent::DraggingListBoxComponent(Viewport *parent, bool disablesAllChildren) :
    HighlightedComponent(),
    parentViewport(parent),
    shouldDisableAllChildren(disablesAllChildren)
{
    if (this->parentViewport != nullptr)
    {
#if PLATFORM_DESKTOP
        this->parentViewport->setScrollBarThickness(2);
#elif PLATFORM_MOBILE
        this->parentViewport->setScrollBarThickness(1);
#endif
        this->parentViewport->setScrollOnDragMode(Viewport::ScrollOnDragMode::never);
    }
    
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void DraggingListBoxComponent::childrenChanged()
{
    if (this->shouldDisableAllChildren)
    {
        for (auto *child : this->getChildren())
        {
            child->setInterceptsMouseClicks(false, false);
        }
    }
}

void DraggingListBoxComponent::mouseDown(const MouseEvent &event)
{
    if (!event.mods.isLeftButtonDown())
    {
        return;
    }
    
    if (this->parentViewport == nullptr)
    {
        this->setSelected(true);
        return;
    }

    this->maxDragDistance = 0;
    this->dragStartMilliseconds = Time::getMillisecondCounterHiRes();

    if (this->listCanBeScrolled())
    {
        jassert(this->parentViewport != nullptr);
        this->viewportStartPosY = this->parentViewport->getViewPosition().getY();
        ViewportKineticSlider::instance().stopAnimationForViewport(this->parentViewport);
    }
}

void DraggingListBoxComponent::mouseUp(const MouseEvent &event)
{
    if (!event.mods.isLeftButtonDown())
    {
        return;
    }

    if (this->parentViewport == nullptr)
    {
        return; // have already reacted on mouseDown
    }

    if (this->maxDragDistance < DraggingListBoxComponent::dragStartThreshold)
    {
        this->setSelected(true);
        return;
    }

    if (this->listCanBeScrolled())
    {
        jassert(this->parentViewport != nullptr);
        const float dragTimeMs = float(Time::getMillisecondCounterHiRes() - this->dragStartMilliseconds);
        if (dragTimeMs < 300)
        {
            const float dragDistance = float(event.getOffsetFromDragStart().getY());
            const float pixelsPerMs = dragDistance / dragTimeMs;
            ViewportKineticSlider::instance().startAnimationForViewport(this->parentViewport,
                Point<float>(0.f, pixelsPerMs / DraggingListBoxComponent::dragSpeed));
        }
    }
}

void DraggingListBoxComponent::mouseDrag(const MouseEvent &event)
{
    if (!event.mods.isLeftButtonDown())
    {
        return;
    }

    this->maxDragDistance = jmax(abs(event.getDistanceFromDragStartY()), this->maxDragDistance);
    
    if (this->listCanBeScrolled())
    {
        jassert(this->parentViewport != nullptr);
        this->parentViewport->setViewPosition(0, this->viewportStartPosY - event.getDistanceFromDragStartY());
        ViewportKineticSlider::instance().calculateDragSpeedForViewport(this->parentViewport, event.getOffsetFromDragStart().toFloat());
    }
}

void DraggingListBoxComponent::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    const float forwardWheel = wheel.deltaY *
        (wheel.isReversed ? -DraggingListBoxComponent::dragSpeed : DraggingListBoxComponent::dragSpeed);
    
    if (this->parentViewport != nullptr)
    {
        const BailOutChecker checker(this);
        this->parentViewport->setViewPosition(0, this->parentViewport->getViewPosition().getY() - int(forwardWheel));
        
        // If viewport is owned by Listbox,
        // the Listbox has just updated its contents here,
        // and the component may be deleted:
        if (!checker.shouldBailOut())
        {
            ViewportKineticSlider::instance().startAnimationForViewport(this->parentViewport, Point<float>(0.f, forwardWheel));
            
            const bool eventWasUsed =
                (wheel.deltaX != 0 && this->parentViewport->getHorizontalScrollBar().isVisible()) ||
                (wheel.deltaY != 0 && this->parentViewport->getVerticalScrollBar().isVisible());
            
            if (!eventWasUsed)
            {
                Component::mouseWheelMove(event, wheel);
            }
        }
    }
}

bool DraggingListBoxComponent::listCanBeScrolled() const
{
    if (this->parentViewport == nullptr)
    {
        return false;
    }
    
    if (this->parentViewport->getViewedComponent() == nullptr)
    {
        return false;
    }
    
    return this->parentViewport->getViewHeight() <
        this->parentViewport->getViewedComponent()->getHeight();
}

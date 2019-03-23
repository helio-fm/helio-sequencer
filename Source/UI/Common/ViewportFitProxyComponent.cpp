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

#include "Common.h"
#include "ViewportFitProxyComponent.h"
#include "CommandIDs.h"

#define DRAG_SPEED 1
#define PADDING 30

ViewportFitProxyComponent::ViewportFitProxyComponent(Viewport &parentViewport,
        Component *child, bool deleteChildOnRelease /*= true*/) :
    viewport(parentViewport),
    shouldDeleteChild(deleteChildOnRelease),
    target(child),
    viewportDragStart(0, 0)
{
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->addAndMakeVisible(this->target);
    this->setSize(this->target->getWidth(), this->target->getHeight());
    this->centerTargetToViewport();
    
#if HELIO_MOBILE
    this->viewport.setScrollBarThickness(2);
#endif
}

ViewportFitProxyComponent::~ViewportFitProxyComponent()
{
    this->removeChildComponent(this->target);

    if (this->shouldDeleteChild)
    {
        delete this->target;
    }
}

void ViewportFitProxyComponent::centerTargetToViewport()
{
    // не хочу разбираться, почему одной итерации мало
    // пусть остается костыль.
    for (int i = 0; i < 2; ++i)
    {
        const int tH = this->target->getHeight();
        const int pH = this->viewport.getMaximumVisibleHeight();

        if (tH < pH)
        {
            this->setSize(this->getWidth(), pH);
            this->target->setTopLeftPosition(this->target->getX(), (pH / 2) - (tH / 2));
        }
        else
        {
            this->setSize(this->getWidth(), tH + PADDING);
            this->target->setTopLeftPosition(this->target->getX(), PADDING / 2);
        }

        const int tW = this->target->getWidth();
        const int pW = this->viewport.getMaximumVisibleWidth();

        if (tW < pW)
        {
            this->setSize(pW, this->getHeight());
            this->target->setTopLeftPosition((pW / 2) - (tW / 2), this->target->getY());
        }
        else
        {
            this->setSize(tW + PADDING, this->getHeight());
            this->target->setTopLeftPosition(PADDING / 2, this->target->getY());
        }
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void ViewportFitProxyComponent::mouseMove(const MouseEvent &e)
{
    if (meFitsViewport())
    {
        this->setMouseCursor(MouseCursor::NormalCursor);
    }
    else
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
    }
}

void ViewportFitProxyComponent::mouseDown(const MouseEvent &event)
{
    this->viewportDragStart = this->viewport.getViewPosition();
    this->target->postCommandMessage(CommandIDs::StartDragViewport);
}

void ViewportFitProxyComponent::mouseDrag(const MouseEvent &event)
{
    if (meFitsViewport()) { return; }

    const Point<int> dragDelta(event.getDistanceFromDragStartX() * DRAG_SPEED,
                               event.getDistanceFromDragStartY() * DRAG_SPEED);

    this->viewport.setViewPosition(this->viewportDragStart - dragDelta);
}

void ViewportFitProxyComponent::mouseUp(const MouseEvent &event)
{
    target->postCommandMessage(CommandIDs::EndDragViewport);
}

void ViewportFitProxyComponent::parentSizeChanged()
{
    this->centerTargetToViewport();
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

bool ViewportFitProxyComponent::meFitsViewport() const
{
    return (this->getHeight() <= this->viewport.getMaximumVisibleHeight() &&
            this->getWidth() <= this->viewport.getMaximumVisibleWidth());
}

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
#include "MultiTouchController.h"

MultiTouchController::MultiTouchController(MultiTouchListener &parent) :
    listener(parent) {}

void MultiTouchController::mouseDown(const MouseEvent &event)
{
    if (event.source.getIndex() > 1)
    {
        return; // we only support 2-fingers multi-touch
    }

    if (event.source.getIndex() == 0)
    {
        this->hasFinger0 = true;
        this->relativePosition1 = this->listener.getMultiTouchRelativeAnchor(event);
        this->relativePositionAnchor1 = this->listener.getMultiTouchRelativeAnchor(event);
        this->absolutePositionAnchor1 = this->listener.getMultiTouchAbsoluteAnchor(event);
    }
    else if (event.source.getIndex() == 1)
    {
        this->hasFinger1 = true;
        this->relativePosition2 = this->listener.getMultiTouchRelativeAnchor(event);
        this->relativePositionAnchor2 = this->listener.getMultiTouchRelativeAnchor(event);
        this->absolutePositionAnchor2 = this->listener.getMultiTouchAbsoluteAnchor(event);
    }

    if (this->hasMultiTouch())
    {
        this->listener.multiTouchStartZooming(event);
    }
}

void MultiTouchController::mouseDrag(const MouseEvent &event)
{
    if (event.source.getIndex() > 1)
    {
        return; // we only support 2-fingers multi-touch
    }

    if (!this->hasMultiTouch())
    {
        // simply update the anchors
        if (event.source.getIndex() == 0)
        {
            this->relativePosition1 = this->listener.getMultiTouchRelativeAnchor(event);
            this->relativePositionAnchor1 = this->listener.getMultiTouchRelativeAnchor(event);
            this->absolutePositionAnchor1 = this->listener.getMultiTouchAbsoluteAnchor(event);
        }
        else if (event.source.getIndex() == 1)
        {
            this->relativePosition2 = this->listener.getMultiTouchRelativeAnchor(event);
            this->relativePositionAnchor2 = this->listener.getMultiTouchRelativeAnchor(event);
            this->absolutePositionAnchor2 = this->listener.getMultiTouchAbsoluteAnchor(event);
        }

        return;
    }

    this->processContinueZoomingEvent(event);
}

void MultiTouchController::mouseUp(const MouseEvent &event)
{
    if (this->hasMultiTouch())
    {
        this->processContinueZoomingEvent(event);
    }

    if (event.source.getIndex() == 0)
    {
        const auto finger1Offset = (this->relativePosition2 - this->relativePosition1).toInt();
        this->listener.multiTouchEndZooming(event
            .withNewPosition(event.getPosition().translated(finger1Offset.x, finger1Offset.y)));
        this->hasFinger0 = false;
    }
    else if (event.source.getIndex() == 1)
    {
        const auto finger2Offset = (this->relativePosition1 - this->relativePosition2).toInt();
        this->listener.multiTouchEndZooming(event
            .withNewPosition(event.getPosition().translated(finger2Offset.x, finger2Offset.y)));
        this->hasFinger1 = false;
    }
}

void MultiTouchController::processContinueZoomingEvent(const MouseEvent &event)
{
    if (event.source.getIndex() == 0)
    {
        this->relativePosition1 = this->listener.getMultiTouchRelativeAnchor(event);
    }
    else if (event.source.getIndex() == 1)
    {
        this->relativePosition2 = this->listener.getMultiTouchRelativeAnchor(event);
    }

    const Rectangle<float> relativePositions(this->relativePosition1, this->relativePosition2);
    const Rectangle<float> relativeAnchor(this->relativePositionAnchor1, this->relativePositionAnchor2);
    const Rectangle<float> absoluteAnchor(this->absolutePositionAnchor1, this->absolutePositionAnchor2);

    const auto finger2Offset = (this->relativePosition1 - this->relativePosition2).toInt();
    const auto firstFingerEvent = event.source.getIndex() == 0 ? event :
        event.withNewPosition(event.getPosition().translated(finger2Offset.x, finger2Offset.y));

    this->listener.multiTouchContinueZooming(firstFingerEvent,
        relativePositions, relativeAnchor, absoluteAnchor);
}

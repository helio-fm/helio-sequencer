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
    jassert(event.source.getIndex() <= 1); // we only support 2-fingers multi-touch

    if (event.source.getIndex() == 0)
    {
        this->finger1On = true;
        this->relativePosition1 = this->listener.getMultiTouchRelativeAnchor(event.position);
        this->relativePositionAnchor1 = this->listener.getMultiTouchRelativeAnchor(event.position);
        this->absolutePositionAnchor1 = this->listener.getMultiTouchAbsoluteAnchor(event.position);
    }
    else if (event.source.getIndex() == 1)
    {
        this->finger2On = true;
        this->relativePosition2 = this->listener.getMultiTouchRelativeAnchor(event.position);
        this->relativePositionAnchor2 = this->listener.getMultiTouchRelativeAnchor(event.position);
        this->absolutePositionAnchor2 = this->listener.getMultiTouchAbsoluteAnchor(event.position);
    }

    if (this->hasMultitouch())
    {
        this->listener.multiTouchStartZooming();
    }
}

void MultiTouchController::mouseDrag(const MouseEvent &event)
{
    if (!this->finger2On && event.source.getIndex() == 0)
    {
        // simply update the anchors
        this->relativePosition1 = this->listener.getMultiTouchRelativeAnchor(event.position);
        this->relativePositionAnchor1 = this->listener.getMultiTouchRelativeAnchor(event.position);
        this->absolutePositionAnchor1 = this->listener.getMultiTouchAbsoluteAnchor(event.position);
        return;
    }
    
    if (event.source.getIndex() == 0)
    {
        this->relativePosition1 = this->listener.getMultiTouchRelativeAnchor(event.position);
    }
    else if (event.source.getIndex() == 1)
    {
        this->relativePosition2 = this->listener.getMultiTouchRelativeAnchor(event.position);
    }

    const Rectangle<float> relativePositions(this->relativePosition1, this->relativePosition2);
    const Rectangle<float> relativeAnchor(this->relativePositionAnchor1, this->relativePositionAnchor2);
    const Rectangle<float> absoluteAnchor(this->absolutePositionAnchor1, this->absolutePositionAnchor2);

    this->listener.multiTouchContinueZooming(relativePositions, relativeAnchor, absoluteAnchor);
}

void MultiTouchController::mouseUp(const MouseEvent &event)
{
    if (event.source.getIndex() == 0)
    {
        this->finger1On = false;
    }
    else if (event.source.getIndex() == 1)
    {
        this->finger2On = false;
    }
}

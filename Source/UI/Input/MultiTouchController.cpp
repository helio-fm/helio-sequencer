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

#if PLATFORM_DESKTOP
#   define ZOOM_THRESHOLD 0.5
#   define DRAG_THRESHOLD 10
#   define ZOOM_H_SPEED 0.005f
#   define ZOOM_V_SPEED 0.01f
#elif JUCE_IOS
#   define ZOOM_THRESHOLD 0.1f
#   define DRAG_THRESHOLD 25
#   define ZOOM_H_SPEED 0.0095f
#   define ZOOM_V_SPEED 0.02f
#elif JUCE_ANDROID
#   define ZOOM_THRESHOLD 0.15f
#   define DRAG_THRESHOLD 25
#   define ZOOM_H_SPEED 0.0025f
#   define ZOOM_V_SPEED 0.005f
#endif

MultiTouchController::MultiTouchController(MultiTouchListener &parent) :
    listener(parent),
    zoomDiff(0, 0),
    dragDiff(0, 0),
    finger1Position(0, 0),
    finger2Position(0, 0),
    center1(0, 0),
    center2(0, 0) {}

void MultiTouchController::mouseDown(const MouseEvent &event)
{
    if (event.source.getIndex() > 1)
    {
        return;
    }

    if (event.source.getIndex() == 0)
    {
        this->finger1On = true;
        this->finger1Position = event.source.getScreenPosition();
        this->center1 = this->listener.getMultiTouchOrigin(event.position);
    }
    else if (event.source.getIndex() == 1)
    {
        this->finger2On = true;
        this->finger2Position = event.source.getScreenPosition();
        this->center2 = this->listener.getMultiTouchOrigin(event.position);
    }

    if (!this->hasMultitouch())
    {
        this->listener.multiTouchCancelPan();
        this->listener.multiTouchCancelZoom();
    }

    const float xLength = fabsf(this->finger2Position.getX() - this->finger1Position.getX());
    const float yLength = fabsf(this->finger2Position.getY() - this->finger1Position.getY());
    this->zoomDiff.setXY(xLength, yLength);
}

void MultiTouchController::mouseDrag(const MouseEvent &event)
{
    if (!this->hasMultitouch())
    {
        return;
    }
    
    if (event.source.getIndex() == 0)
    {
        this->finger1Position = event.source.getScreenPosition();
        this->center1 = this->listener.getMultiTouchOrigin(event.position);
    }
    else if (event.source.getIndex() == 1)
    {
        this->finger2Position = event.source.getScreenPosition();
        this->center2 = this->listener.getMultiTouchOrigin(event.position);
    }
        
    const float vZoomLength = fabsf(this->finger2Position.getX() - this->finger1Position.getX());
    const float hZoomLength = fabsf(this->finger2Position.getY() - this->finger1Position.getY());
    const auto zoomOffset = (Point<float>(vZoomLength, hZoomLength) - this->zoomDiff) * Point<float>(ZOOM_V_SPEED, ZOOM_H_SPEED);

    const Point<float> diffV(0.f, zoomOffset.getY());
    this->listener.multiTouchCancelPan();
    this->listener.multiTouchZoomEvent((this->center1 + this->center2) / 2.f, diffV);

    const Point<float> diffH(zoomOffset.getX(), 0.f);
    this->listener.multiTouchCancelPan();
    this->listener.multiTouchZoomEvent((this->center1 + this->center2) / 2.f, diffH);
        
    this->zoomDiff.setXY(vZoomLength, hZoomLength);
}

void MultiTouchController::mouseUp(const MouseEvent &event)
{
    if (this->hasMultitouch())
    {
        this->finger1On = false;
        this->finger2On = false;
        this->zoomDiff.setXY(0, 0);
        this->dragDiff.setXY(0, 0);
        this->listener.multiTouchCancelPan();
        this->listener.multiTouchCancelZoom();
    }
}

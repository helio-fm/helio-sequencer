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
#include "MultiTouchController.h"

MultiTouchController::MultiTouchController(MultiTouchListener &parent) :
    listener(parent) {}

MultiTouchController::TouchData MultiTouchController::getAllTouchData(const MouseEvent &e) const
{
    TouchData result;
    result.relativePosition = this->listener.getMultiTouchRelativeAnchor(e);
    result.relativePositionAnchor = this->listener.getMultiTouchRelativeAnchor(e);
    result.absolutePositionAnchor = this->listener.getMultiTouchAbsoluteAnchor(e);
    return result;
}

void MultiTouchController::mouseDown(const MouseEvent &event)
{
    jassert(!this->touches.contains(event.source.getIndex())); // really shouldn't happen

    const auto touchMode = this->touches.size() >= 2 ? TouchUsage::Unused :
        ((this->touches.size() == 1 && this->touches.begin()->second == TouchUsage::Finger1) ?
            TouchUsage::Finger2 : TouchUsage::Finger1);

    this->touches[event.source.getIndex()] = touchMode;

    const auto touchData = this->getAllTouchData(event);
    this->anchorsCache[event.source.getIndex()] = touchData;

    //this->dump();

    if (touchMode == TouchUsage::Unused)
    {
        return;
    }

    if (touchMode == TouchUsage::Finger1)
    {
        this->finger1 = touchData;
    }
    else if (touchMode == TouchUsage::Finger2)
    {
        this->finger2 = touchData;
    }

    if (this->hasMultiTouch())
    {
        this->listener.multiTouchStartZooming();
    }
}

void MultiTouchController::mouseDrag(const MouseEvent &event)
{
    jassert(this->touches.contains(event.source.getIndex()));
    const auto touchMode = this->touches[event.source.getIndex()];

    const auto touchData = this->getAllTouchData(event);
    this->anchorsCache[event.source.getIndex()] = touchData;

    if (touchMode == TouchUsage::Unused)
    {
        return;
    }

    if (!this->hasMultiTouch())
    {
        // no multitouch yet, simply update the anchors
        if (touchMode == TouchUsage::Finger1)
        {
            this->finger1 = touchData;
        }
        else if (touchMode == TouchUsage::Finger2)
        {
            this->finger2 = touchData;
        }

        //this->dump();
        return;
    }

    this->processContinueZoomingEvent(event);
}

void MultiTouchController::mouseUp(const MouseEvent &event)
{
    jassert(this->touches.contains(event.source.getIndex()));

    if (this->hasMultiTouch())
    {
        this->processContinueZoomingEvent(event);
    }

    const auto touchMode = this->touches[event.source.getIndex()];

    if (touchMode == TouchUsage::Finger1)
    {
        const auto offset = (this->finger2.relativePosition - this->finger1.relativePosition).toInt();
        const auto otherFingerAnchor = event.withNewPosition(event.getPosition() + offset);
        this->listener.multiTouchEndZooming(otherFingerAnchor);

        for (const auto &touch : this->touches)
        {
            if (touch.second == TouchUsage::Unused)
            {
                const auto touchId = touch.first;
                this->touches[touchId] = TouchUsage::Finger1;
                this->finger1 = this->anchorsCache[touchId];
                this->listener.multiTouchStartZooming();
                break;
            }
        }
    }
    else if (touchMode == TouchUsage::Finger2)
    {
        const auto offset = (this->finger1.relativePosition - this->finger2.relativePosition).toInt();
        const auto otherFingerAnchor = event.withNewPosition(event.getPosition() + offset);
        this->listener.multiTouchEndZooming(otherFingerAnchor);

        for (const auto &touch : this->touches)
        {
            if (touch.second == TouchUsage::Unused)
            {
                const auto touchId = touch.first;
                this->touches[touchId] = TouchUsage::Finger2;
                this->finger2 = this->anchorsCache[touchId];
                this->listener.multiTouchStartZooming();
                break;
            }
        }
    }

    this->touches.erase(event.source.getIndex());
    this->anchorsCache.erase(event.source.getIndex());
    //this->dump();
}

void MultiTouchController::processContinueZoomingEvent(const MouseEvent &event)
{
    jassert(this->touches.contains(event.source.getIndex()));
    const auto touchMode = this->touches[event.source.getIndex()];

    if (touchMode == TouchUsage::Finger1)
    {
        this->finger1.relativePosition = this->listener.getMultiTouchRelativeAnchor(event);
    }
    else if (touchMode == TouchUsage::Finger2)
    {
        this->finger2.relativePosition = this->listener.getMultiTouchRelativeAnchor(event);
    }

    const Rectangle<float> relativePositions(this->finger1.relativePosition, this->finger2.relativePosition);
    const Rectangle<float> relativeAnchor(this->finger1.relativePositionAnchor, this->finger2.relativePositionAnchor);
    const Rectangle<float> absoluteAnchor(this->finger1.absolutePositionAnchor, this->finger2.absolutePositionAnchor);

    this->listener.multiTouchContinueZooming(relativePositions, relativeAnchor, absoluteAnchor);
}

/*
void MultiTouchController::dump() const
{
    String debug;
    debug << String(this->touches.size()) << " touche(s) :: ";

    for (const auto &touch : this->touches)
    {
        debug << "id " << touch.first << ": " << (touch.second == TouchUsage::Finger1 ? "f1" :
            touch.second == TouchUsage::Finger2 ? "f2" : "unused") << ", ";
    }

    DBG(debug);
}
*/

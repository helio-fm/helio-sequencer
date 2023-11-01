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
#if JUCE_ANDROID
    // normally, we should be registering all touch events here,
    // but Android is never simple: some shells (looking at you, Realme UI),
    // have strange multi-touch behavior in which all touches except the first
    // are broken in a way that the touch down position and subsequent drag position
    // for the same pointer id will sometimes differ a lot for whatever reason
    // (is this a JUCE bug?), resulting in glitchy behavior due to instantly jumping
    // far away from the anchors set in the down event handler when dragging begins;
    // to get around this, we'll just rely on drag events, which always seem to be
    // consistent with each other, and register new pointer events there;
    // this hack shouldn't have any noticeable effects anyway
    if (this->touches.empty())
    {
        this->registerTouchEvent(event);
    }
#else
    this->registerTouchEvent(event);
#endif
}

void MultiTouchController::mouseDrag(const MouseEvent &event)
{
    if (!this->touches.contains(event.source.getIndex()))
    {
        // the mouseDown workaround
        this->registerTouchEvent(event);
    }

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
    if (!this->touches.contains(event.source.getIndex()))
    {
        jassertfalse; // dragging never started
        return;
    }

    const auto touchMode = this->touches[event.source.getIndex()];

    if (this->hasMultiTouch() &&
        touchMode != TouchUsage::Unused)
    {
        this->processContinueZoomingEvent(event);
    }

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

void MultiTouchController::registerTouchEvent(const MouseEvent &event)
{
    if (this->touches.contains(event.source.getIndex()))
    {
        jassertfalse; // pointer events out of order?
        this->anchorsCache[event.source.getIndex()] = this->getAllTouchData(event);
        return;
    }

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

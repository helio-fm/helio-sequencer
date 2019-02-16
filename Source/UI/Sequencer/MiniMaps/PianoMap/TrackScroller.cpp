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
#include "TrackScroller.h"
#include "TrackScrollerScreen.h"
#include "Playhead.h"
#include "Transport.h"
#include "HybridRoll.h"
#include "PanelBackgroundC.h"
#include "Origami.h"
#include "ColourIDs.h"
#include "HelioTheme.h"
#include "PianoProjectMap.h"

TrackScroller::TrackScroller(Transport &transportRef, HybridRoll *targetRoll) :
    transport(transportRef),
    roll(targetRoll),
    mapShouldGetStretched(true)
{
    this->setPaintingIsUnclipped(true);
    this->setOpaque(true);

    this->playhead = new Playhead(*this->roll, this->transport);
    
    this->helperRectangle = new HorizontalDragHelper(*this);
    this->addAndMakeVisible(this->helperRectangle);
    
    this->screenRange = new TrackScrollerScreen(*this);
    this->addAndMakeVisible(this->screenRange);
    
    this->resized();
}

TrackScroller::~TrackScroller()
{
    this->disconnectPlayhead();
}

void TrackScroller::addOwnedMap(Component *newTrackMap, bool shouldBringToFront)
{
    this->trackMaps.add(newTrackMap);
    this->addAndMakeVisible(newTrackMap);
    
    // playhead is always tied to the first map:
    if (this->trackMaps.size() == 1)
    {
        this->disconnectPlayhead();
        newTrackMap->addAndMakeVisible(this->playhead);
    }
    
    // fade-in if not the first child
    if (this->trackMaps.size() > 1)
    {
        newTrackMap->setVisible(false);
        this->fader.fadeIn(newTrackMap, 200);
    }
    
    if (shouldBringToFront)
    {
        this->helperRectangle->toFront(false);
        this->screenRange->toFront(false);
        newTrackMap->toFront(false);
    }
    else
    {
        newTrackMap->toFront(false);
        this->helperRectangle->toFront(false);
        this->screenRange->toFront(false);
    }
    
    this->resized();
}

void TrackScroller::removeOwnedMap(Component *existingTrackMap)
{
    if (this->trackMaps.contains(existingTrackMap))
    {
        // fadeout causes weird OpenGL errors for the large maps:
        //this->fader.fadeOut(existingTrackMap, 150);
        this->removeChildComponent(existingTrackMap);
        this->trackMaps.removeObject(existingTrackMap);

        this->resized();
    }
}

void TrackScroller::disconnectPlayhead()
{
    if (this->playhead->getParentComponent())
    {
        this->playhead->getParentComponent()->removeChildComponent(this->playhead);
    }
}

//===----------------------------------------------------------------------===//
// TrackScroller
//===----------------------------------------------------------------------===//

void TrackScroller::horizontalDragByUser(Component *component, const Rectangle<int> &bounds)
{
    const Rectangle<float> &screenRangeBounds = this->screenRange->getRealBounds();
    this->screenRange->setRealBounds(screenRangeBounds.withX(float(component->getX())));
    this->xMoveByUser();
}

void TrackScroller::xyMoveByUser()
{
    if (this->roll != nullptr)
    {
        const auto screenRangeBounds = this->screenRange->getRealBounds();

        const float mw = float(this->getWidth()) - screenRangeBounds.getWidth();
        const float propX = screenRangeBounds.getTopLeft().getX() / mw;
        const float mh = float(this->getHeight()) - screenRangeBounds.getHeight();
        const float propY = screenRangeBounds.getTopLeft().getY() / mh;

        // fixes for header height delta
        const float hh = float(HYBRID_ROLL_HEADER_HEIGHT);
        const float rollHeight = float(this->roll->getHeight());
        const float propY2 = roundf(((rollHeight - hh) * propY) - hh) / rollHeight;
        this->roll->panProportionally(propX, propY2);

        const auto p = this->getIndicatorBounds();
        const auto hp = p.toType<int>();
        this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
        this->screenRange->setRealBounds(p);

        for (int i = 0; i < this->trackMaps.size(); ++i)
        {
            this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
        }
    }
}

void TrackScroller::xMoveByUser()
{
    if (this->roll != nullptr)
    {
        const auto screenRangeBounds = this->screenRange->getRealBounds();

        const float mw = float(this->getWidth()) - screenRangeBounds.getWidth();
        const float propX = screenRangeBounds.getTopLeft().getX() / mw;
        const float propY = float(this->roll->getViewport().getViewPositionY()) /
            float(this->roll->getHeight() - this->roll->getViewport().getViewHeight());

        this->roll->panProportionally(propX, propY);

        const auto p = this->getIndicatorBounds();
        const auto hp = p.toType<int>();
        this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
        this->screenRange->setRealBounds(p);

        for (int i = 0; i < this->trackMaps.size(); ++i)
        {
            this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
        }
    }
}

void TrackScroller::resizeByUser()
{
    if (this->roll != nullptr)
    {
        const float w = float(this->screenRange->getWidth());
        const float h = float(this->screenRange->getHeight());

        const float mw = float(this->getWidth());
        const float propX = (w / mw);

        const float mh = float(this->getHeight());
        const float propY = (h / mh);

        const Point<float> proportional(propX, propY);
        this->roll->zoomAbsolute(proportional);

        const auto p = this->getIndicatorBounds();
        const auto hp = p.toType<int>();
        this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
        this->screenRange->setRealBounds(p);

        for (int i = 0; i < this->trackMaps.size(); ++i)
        {
            this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
        }
    }
}

void TrackScroller::toggleStretchingMapAlaSublime()
{
    this->mapShouldGetStretched = !this->mapShouldGetStretched;
    this->resized();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void TrackScroller::resized()
{
    const auto p = this->getIndicatorBounds();
    const auto hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
}

void TrackScroller::paint(Graphics &g)
{
    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    g.setFillType({ theme.getBgCacheC(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineDark));
    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));

    g.setColour(findDefaultColour(ColourIDs::TrackScroller::borderLineLight));
    g.drawHorizontalLine(1, 0.f, float(this->getWidth()));
}

void TrackScroller::mouseDrag(const MouseEvent &event)
{
    if (! this->mapShouldGetStretched)
    {
        this->screenRange->setRealBounds(this->screenRange->getRealBounds().withCentre(event.position));
        this->xyMoveByUser();
    }
}

void TrackScroller::mouseUp(const MouseEvent &event)
{
    if (event.getDistanceFromDragStart() < 5)
    {
        this->toggleStretchingMapAlaSublime();
    }
}

void TrackScroller::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    if (this->roll != nullptr)
    {
        this->roll->mouseWheelMove(event.getEventRelativeTo(this->roll), wheel);
    }
}


//===----------------------------------------------------------------------===//
// MidiRollListener
//===----------------------------------------------------------------------===//

void TrackScroller::onMidiRollMoved(HybridRoll *targetRoll)
{
    if (this->roll == targetRoll && !this->isTimerRunning())
    {
        this->triggerAsyncUpdate();
    }
}

void TrackScroller::onMidiRollResized(HybridRoll *targetRoll)
{
    if (this->roll == targetRoll && !this->isTimerRunning())
    {
        this->triggerAsyncUpdate();
    }
}

// Starts quick and dirty animation from one bounds to another
void TrackScroller::switchToRoll(HybridRoll *targetRoll)
{
    this->oldAreaBounds = this->getIndicatorBounds();
    this->oldMapBounds = this->getMapBounds().toFloat();
    this->roll = targetRoll;
    this->startTimerHz(60);
}

//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

static Rectangle<float> lerpRectangle(const Rectangle<float> &r1,
    const Rectangle<float> &r2, float factor)
{
    const float x1 = r1.getX();
    const float y1 = r1.getY();
    const float x2 = r1.getBottomRight().getX();
    const float y2 = r1.getBottomRight().getY();

    const float dx1 = r2.getX() - x1;
    const float dy1 = r2.getY() - y1;
    const float dx2 = r2.getBottomRight().getX() - x2;
    const float dy2 = r2.getBottomRight().getY() - y2;

    const float lx1 = x1 + dx1 * factor;
    const float ly1 = y1 + dy1 * factor;
    const float lx2 = x2 + dx2 * factor;
    const float ly2 = y2 + dy2 * factor;

    return { lx1, ly1, lx2 - lx1, ly2 - ly1 };
}

static float getRectangleDistance(const Rectangle<float> &r1,
    const Rectangle<float> &r2)
{
    return fabs(r1.getX() - r2.getX()) +
        fabs(r1.getY() - r2.getY()) +
        fabs(r1.getWidth() - r2.getWidth()) +
        fabs(r1.getHeight() - r2.getHeight());
}

void TrackScroller::timerCallback()
{
    const auto mb = this->getMapBounds().toFloat();
    const auto mbLerp = lerpRectangle(this->oldMapBounds, mb, 0.2f);
    const auto ib = this->getIndicatorBounds();
    const auto ibLerp = lerpRectangle(this->oldAreaBounds, ib, 0.2f);
    const bool shouldStop = getRectangleDistance(this->oldAreaBounds, ib) < 0.5f;
    const auto targetAreaBounds = shouldStop ? ib : ibLerp;
    const auto targetMapBounds = shouldStop ? mb : mbLerp;

    this->oldAreaBounds = targetAreaBounds;
    this->oldMapBounds = targetMapBounds;

    const auto helperBounds = targetAreaBounds.toType<int>();
    this->helperRectangle->setBounds(helperBounds.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(targetAreaBounds);

    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(targetMapBounds.toType<int>());
    }

    if (shouldStop)
    {
        this->stopTimer();
    }
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void TrackScroller::handleAsyncUpdate()
{
    const auto p = this->getIndicatorBounds();
    const auto hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(0).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    for (int i = 0; i < this->trackMaps.size(); ++i)
    {
        this->trackMaps.getUnchecked(i)->setBounds(this->getMapBounds());
    }
    
    this->playhead->parentSizeChanged(); // a hack: also update playhead position
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

#define INDICATOR_FIXED_WIDTH (150)

Rectangle<float> TrackScroller::getIndicatorBounds() const
{
    if (this->roll != nullptr)
    {
        const float viewX = float(this->roll->getViewport().getViewPositionX());
        const float viewWidth = float(this->roll->getViewport().getViewWidth());
        const float rollWidth = float(this->roll->getWidth());
        const float rollInvisibleArea = rollWidth - viewWidth;
        const float trackWidth = float(this->getWidth());
        const float trackInvisibleArea = float(this->getWidth() - INDICATOR_FIXED_WIDTH);
        const float mapWidth = ((INDICATOR_FIXED_WIDTH * rollWidth) / viewWidth);

        const float zoomFactorY = this->roll->getZoomFactorY();
        const float rollHeaderHeight = float(HYBRID_ROLL_HEADER_HEIGHT);
        const float rollHeight = float(this->roll->getHeight() - rollHeaderHeight);
        const float viewY = float(this->roll->getViewport().getViewPositionY() + rollHeaderHeight);
        const float trackHeight = float(this->getHeight());
        const float trackHeaderHeight = float(rollHeaderHeight * trackHeight / rollHeight);

        const float rY = roundf(trackHeight * (viewY / rollHeight)) - trackHeaderHeight;
        const float rH = (trackHeight * zoomFactorY);

        if (mapWidth <= trackWidth || !this->mapShouldGetStretched)
        {
            const float rX = ((trackWidth * viewX) / rollWidth);
            const float rW = (trackWidth * this->roll->getZoomFactorX());
            return { rX, rY, rW, rH };
        }

        const float rX = ((trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth));
        const float rW = INDICATOR_FIXED_WIDTH;
        return { rX, rY, rW, rH };
    }

    return { 0.f, 0.f, 0.f, 0.f };
}

Rectangle<int> TrackScroller::getMapBounds() const
{
    if (this->roll != nullptr)
    {
        const float viewX = float(this->roll->getViewport().getViewPositionX());
        const float viewWidth = float(this->roll->getViewport().getViewWidth());
        const float rollWidth = float(this->roll->getWidth());
        const float rollInvisibleArea = rollWidth - viewWidth;
        const float trackWidth = float(this->getWidth());
        const float trackInvisibleArea = float(this->getWidth() - INDICATOR_FIXED_WIDTH);
        const float mapWidth = ((INDICATOR_FIXED_WIDTH * rollWidth) / viewWidth);

        if (mapWidth <= trackWidth || !this->mapShouldGetStretched)
        {
            return { 0, 0, int(trackWidth), this->getHeight() };
        }

        const float rX = ((trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth));
        const float dX = (viewX * mapWidth) / rollWidth;
        return { int(rX - dX), 0, int(mapWidth), this->getHeight() };
    }

    return { 0, 0, 0, 0 };
}

//===----------------------------------------------------------------------===//
// Additional horizontal dragger
//===----------------------------------------------------------------------===//

TrackScroller::HorizontalDragHelper::HorizontalDragHelper(TrackScroller &scrollerRef) :
    colour(findDefaultColour(ColourIDs::TrackScroller::scrollerFill)),
    scroller(scrollerRef)
{
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->toBack();

    this->moveConstrainer = new MoveConstrainer(this->scroller);
    this->moveConstrainer->setMinimumSize(4, 4);
    this->moveConstrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);
}

void TrackScroller::HorizontalDragHelper::mouseDown(const MouseEvent &e)
{
    this->dragger.startDraggingComponent(this, e);
}

void TrackScroller::HorizontalDragHelper::mouseDrag(const MouseEvent &e)
{
    this->dragger.dragComponent(this, e, this->moveConstrainer);
}

void TrackScroller::HorizontalDragHelper::paint(Graphics &g)
{
    g.setColour(this->colour);
    g.fillRect(this->getLocalBounds());
}

void TrackScroller::HorizontalDragHelper::MoveConstrainer::
    applyBoundsToComponent(Component &component, Rectangle<int> bounds)
{
    ComponentBoundsConstrainer::applyBoundsToComponent(component, bounds);
    this->scroller.horizontalDragByUser(&component, bounds);
}

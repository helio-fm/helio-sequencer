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
#include "ProjectMapScroller.h"
#include "ProjectMapScrollerScreen.h"
#include "PianoProjectMap.h"
#include "Playhead.h"
#include "Transport.h"
#include "HybridRoll.h"
#include "ColourIDs.h"
#include "HelioTheme.h"

ProjectMapScroller::ProjectMapScroller(Transport &transportRef, SafePointer<HybridRoll> roll) :
    transport(transportRef),
    roll(roll),
    borderLineDark(findDefaultColour(ColourIDs::TrackScroller::borderLineDark)),
    borderLineLight(findDefaultColour(ColourIDs::TrackScroller::borderLineLight))
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);

    this->playhead = make<Playhead>(*this->roll, this->transport);

    this->helperRectangle = make<HorizontalDragHelper>(*this);
    this->addAndMakeVisible(this->helperRectangle.get());

    this->screenRange = make<ProjectMapScrollerScreen>(*this);
    this->addAndMakeVisible(this->screenRange.get());
}

ProjectMapScroller::~ProjectMapScroller()
{
    this->disconnectPlayhead();
}

void ProjectMapScroller::addOwnedMap(Component *newTrackMap, bool shouldBringToFront)
{
    this->trackMaps.add(newTrackMap);
    this->addAndMakeVisible(newTrackMap);

    // playhead is always tied to the first map:
    if (this->trackMaps.size() == 1)
    {
        this->disconnectPlayhead();
        newTrackMap->addAndMakeVisible(this->playhead.get());
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
}

void ProjectMapScroller::removeOwnedMap(Component *existingTrackMap)
{
    if (this->trackMaps.contains(existingTrackMap))
    {
        this->removeChildComponent(existingTrackMap);
        this->trackMaps.removeObject(existingTrackMap);

        this->resized();
    }
}

void ProjectMapScroller::disconnectPlayhead()
{
    if (this->playhead->getParentComponent())
    {
        this->playhead->getParentComponent()->removeChildComponent(this->playhead.get());
    }
}

//===----------------------------------------------------------------------===//
// TrackScroller
//===----------------------------------------------------------------------===//

void ProjectMapScroller::horizontalDragByUser(Component *component, const Rectangle<int> &bounds)
{
    const Rectangle<float> &screenRangeBounds = this->screenRange->getRealBounds();
    this->screenRange->setRealBounds(screenRangeBounds.withX(float(component->getX())));
    this->xMoveByUser();
}

void ProjectMapScroller::xyMoveByUser()
{
    jassert(this->roll != nullptr);
     
    const auto screenRangeBounds = this->screenRange->getRealBounds();

    const float mw = float(this->getWidth()) - screenRangeBounds.getWidth();
    const float propX = screenRangeBounds.getTopLeft().getX() / mw;
    const float mh = float(this->getHeight()) - screenRangeBounds.getHeight();
    const float propY = screenRangeBounds.getTopLeft().getY() / mh;

    // fixes for header height delta
    const float hh = float(Globals::UI::rollHeaderHeight);
    const float rollHeight = float(this->roll->getHeight());
    const float propY2 = roundf(((rollHeight - hh) * propY) - hh) / rollHeight;
    this->roll->panProportionally(propX, propY2);

    const auto p = this->getIndicatorBounds();
    const auto hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(1).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);

    const auto mapBounds = this->getMapBounds();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(mapBounds);
    }
}

void ProjectMapScroller::xMoveByUser()
{
    jassert(this->roll != nullptr);

    const auto screenRangeBounds = this->screenRange->getRealBounds();

    const float mw = float(this->getWidth()) - screenRangeBounds.getWidth();
    const float propX = screenRangeBounds.getTopLeft().getX() / mw;
    const float propY = float(this->roll->getViewport().getViewPositionY()) /
        float(this->roll->getHeight() - this->roll->getViewport().getViewHeight());

    this->roll->panProportionally(propX, propY);

    const auto p = this->getIndicatorBounds();
    const auto hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(1).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);

    const auto mapBounds = this->getMapBounds();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(mapBounds);
    }
}

void ProjectMapScroller::toggleStretchingMapAlaSublime()
{
    this->stretchedMapsFlag = !this->stretchedMapsFlag;
    this->resized();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void ProjectMapScroller::resized()
{
    const auto p = this->getIndicatorBounds();
    const auto hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(1).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);
    
    const auto mapBounds = this->getMapBounds();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(mapBounds);
    }
}

void ProjectMapScroller::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getBgCacheC(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(this->borderLineDark);
    g.fillRect(0, 0, this->getWidth(), 1);

    g.setColour(this->borderLineLight);
    g.fillRect(0, 1, this->getWidth(), 1);
}

void ProjectMapScroller::mouseDrag(const MouseEvent &event)
{
    if (! this->stretchedMode())
    {
        this->screenRange->setRealBounds(this->screenRange->getRealBounds().withCentre(event.position));
        this->xyMoveByUser();
    }
}

void ProjectMapScroller::mouseUp(const MouseEvent &event)
{
    if (event.getDistanceFromDragStart() < 5)
    {
        this->toggleStretchingMapAlaSublime();
    }
}

void ProjectMapScroller::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    jassert(this->roll != nullptr);
    this->roll->mouseWheelMove(event.getEventRelativeTo(this->roll), wheel);
}


//===----------------------------------------------------------------------===//
// MidiRollListener
//===----------------------------------------------------------------------===//

void ProjectMapScroller::onMidiRollMoved(HybridRoll *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll && !this->isTimerRunning())
    {
        this->triggerAsyncUpdate();
    }
}

void ProjectMapScroller::onMidiRollResized(HybridRoll *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll && !this->isTimerRunning())
    {
        this->triggerAsyncUpdate();
    }
}

// Starts quick and dirty animation from one bounds to another
void ProjectMapScroller::switchToRoll(SafePointer<HybridRoll> roll)
{
    this->oldAreaBounds = this->getIndicatorBounds();
    this->oldMapBounds = this->getMapBounds().toFloat();
    this->roll = roll;
    this->startTimerHz(this->animationTimerFrequencyHz);
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

void ProjectMapScroller::timerCallback()
{
    const auto mb = this->getMapBounds().toFloat();
    const auto mbLerp = lerpRectangle(this->oldMapBounds, mb, 0.35f);
    const auto ib = this->getIndicatorBounds();
    const auto ibLerp = lerpRectangle(this->oldAreaBounds, ib, 0.35f);
    const bool shouldStop = getRectangleDistance(this->oldAreaBounds, ib) < 0.5f;
    const auto targetAreaBounds = shouldStop ? ib : ibLerp;
    const auto targetMapBounds = shouldStop ? mb : mbLerp;

    this->oldAreaBounds = targetAreaBounds;
    this->oldMapBounds = targetMapBounds;

    const auto helperBounds = targetAreaBounds.toType<int>();
    this->helperRectangle->setBounds(helperBounds.withTop(1).withBottom(this->getHeight()));
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

void ProjectMapScroller::handleAsyncUpdate()
{
    this->updateAllBounds();
}

void ProjectMapScroller::updateAllBounds()
{
    const auto p = this->getIndicatorBounds();
    const auto hp = p.toType<int>();
    this->helperRectangle->setBounds(hp.withTop(1).withBottom(this->getHeight()));
    this->screenRange->setRealBounds(p);

    const auto mapBounds = this->getMapBounds();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(mapBounds);
    }

    this->playhead->parentSizeChanged(); // a hack: also update playhead position
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Rectangle<float> ProjectMapScroller::getIndicatorBounds() const noexcept
{
    jassert(this->roll != nullptr);

    const float viewX = float(this->roll->getViewport().getViewPositionX());
    const float viewWidth = float(this->roll->getViewport().getViewWidth());
    const float rollWidth = float(this->roll->getWidth());
    const float rollInvisibleArea = rollWidth - viewWidth;
    const float trackWidth = float(this->getWidth());
    const float trackInvisibleArea = float(this->getWidth() - ProjectMapScroller::screenRangeWidth);
    const float mapWidth = (ProjectMapScroller::screenRangeWidth * rollWidth) / viewWidth;

    const float zoomFactorY = this->roll->getZoomFactorY();
    const float rollHeaderHeight = float(Globals::UI::rollHeaderHeight);
    const float rollHeight = float(this->roll->getHeight() - rollHeaderHeight);
    const float viewY = float(this->roll->getViewport().getViewPositionY() + rollHeaderHeight);
    const float trackHeight = float(this->getHeight());
    const float trackHeaderHeight = float(rollHeaderHeight * trackHeight / rollHeight);

    const float rY = ceilf(trackHeight * (viewY / rollHeight)) - trackHeaderHeight + 1.f;
    const float rH = (trackHeight * zoomFactorY);

    if (mapWidth <= trackWidth || !this->stretchedMode())
    {
        const float rX = ((trackWidth * viewX) / rollWidth);
        const float rW = (trackWidth * this->roll->getZoomFactorX());
        return { rX, rY, rW, rH };
    }

    const float rX = (trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth);
    const float rW = ProjectMapScroller::screenRangeWidth;
    return { rX, rY, rW, rH };
}

Rectangle<int> ProjectMapScroller::getMapBounds() const noexcept
{
    jassert(this->roll != nullptr);

    const float viewX = float(this->roll->getViewport().getViewPositionX());
    const float viewWidth = float(this->roll->getViewport().getViewWidth());
    const float rollWidth = float(this->roll->getWidth());
    const float rollInvisibleArea = rollWidth - viewWidth;
    const float trackWidth = float(this->getWidth());
    const float trackInvisibleArea = float(this->getWidth() - ProjectMapScroller::screenRangeWidth);
    const float mapWidth = (ProjectMapScroller::screenRangeWidth * rollWidth) / viewWidth;

    if (mapWidth <= trackWidth || !this->stretchedMode())
    {
        return { 0, 0, int(trackWidth), this->getHeight() };
    }

    const float rX = (trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth);
    const float dX = (viewX * mapWidth) / rollWidth;
    return { int(rX - dX), 0, int(mapWidth), this->getHeight() };
}

void ProjectMapScroller::setScrollerMode(ScrollerMode mode)
{
    if (this->scrollerMode == mode)
    {
        return;
    }

    this->scrollerMode = mode;
    const auto isFullMap = mode == ScrollerMode::Map;

    for (auto *map : this->trackMaps)
    {
        if (auto *pianoMap = dynamic_cast<PianoProjectMap *>(map))
        {
            pianoMap->setBrightness(isFullMap ? 1.f : 0.75f);
        }
        else
        {
            map->setVisible(isFullMap);
        }
    }

    this->screenRange->setVisible(isFullMap);
    this->screenRange->setEnabled(isFullMap); // to disable mouse interaction

    this->helperRectangle->setBrightness(isFullMap ?
        HorizontalDragHelper::defaultBrightness : 1.f);

    this->updateAllBounds();
}

ProjectMapScroller::ScrollerMode ProjectMapScroller::getScrollerMode() const noexcept
{
    return this->scrollerMode;
}

//===----------------------------------------------------------------------===//
// Additional horizontal dragger
//===----------------------------------------------------------------------===//

class HorizontalDragHelperConstrainer final : public ComponentBoundsConstrainer
{
public:

    explicit HorizontalDragHelperConstrainer(ProjectMapScroller &scrollerRef) :
        scroller(scrollerRef) {}

    void applyBoundsToComponent(Component &component, Rectangle<int> bounds) override
    {
        ComponentBoundsConstrainer::applyBoundsToComponent(component, bounds);
        this->scroller.horizontalDragByUser(&component, bounds);
    }

private:

    ProjectMapScroller &scroller;
};

ProjectMapScroller::HorizontalDragHelper::HorizontalDragHelper(ProjectMapScroller &scrollerRef) :
    scroller(scrollerRef)
{
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->toBack();

    this->setBrightness(HorizontalDragHelper::defaultBrightness);

    this->moveConstrainer = make<HorizontalDragHelperConstrainer>(this->scroller);
    this->moveConstrainer->setMinimumSize(4, 4);
    this->moveConstrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);
}

void ProjectMapScroller::HorizontalDragHelper::setBrightness(float brightness)
{
    this->colour = findDefaultColour(ColourIDs::TrackScroller::scrollerFill).withMultipliedAlpha(brightness);
    this->repaint();
}

void ProjectMapScroller::HorizontalDragHelper::mouseDown(const MouseEvent &e)
{
    this->dragger.startDraggingComponent(this, e);
}

void ProjectMapScroller::HorizontalDragHelper::mouseDrag(const MouseEvent &e)
{
    this->dragger.dragComponent(this, e, this->moveConstrainer.get());
}

void ProjectMapScroller::HorizontalDragHelper::paint(Graphics &g)
{
    g.setColour(this->colour);
    g.fillRect(this->getLocalBounds());
    g.fillRect(0.f, 0.f, 1.f, float(this->getHeight()));
    g.fillRect(float(this->getWidth() - 1.f), 0.f, 1.f, float(this->getHeight()));
}

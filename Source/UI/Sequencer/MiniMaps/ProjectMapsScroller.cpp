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
#include "ProjectMapsScroller.h"
#include "PianoProjectMap.h"
#include "Playhead.h"
#include "ProjectNode.h"
#include "PianoRoll.h"
#include "MultiTouchController.h"
#include "TrackStartIndicator.h"
#include "TrackEndIndicator.h"
#include "HelioTheme.h"
#include "MainLayout.h"

ProjectMapsScroller::ProjectMapsScroller(ProjectNode &project, SafePointer<RollBase> roll) :
    project(project),
    roll(roll)
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setAccessible(false);

    this->playhead = make<PlayheadSmall>(*this->roll, this->project.getTransport());

    this->horizontalRangeRectangle = make<HorizontalRangeRectangle>(*this);
    this->addAndMakeVisible(this->horizontalRangeRectangle.get());

    this->screenRangeRectangle = make<ScreenRangeRectangle>();
    this->addAndMakeVisible(this->screenRangeRectangle.get());

    this->projectStartIndicator = make<TrackStartIndicator>();
    this->addAndMakeVisible(this->projectStartIndicator.get());

    this->projectEndIndicator = make<TrackEndIndicator>();
    this->addAndMakeVisible(this->projectEndIndicator.get());

    this->multiTouchController = make<MultiTouchController>(*this);
    this->addMouseListener(this->multiTouchController.get(), true);

    this->project.addListener(this);
}

ProjectMapsScroller::~ProjectMapsScroller()
{
    this->project.removeListener(this);
    this->disconnectPlayhead();
}

void ProjectMapsScroller::disconnectPlayhead()
{
    if (this->playhead->getParentComponent())
    {
        this->playhead->getParentComponent()->removeChildComponent(this->playhead.get());
    }
}

//===----------------------------------------------------------------------===//
// TrackScroller
//===----------------------------------------------------------------------===//

void ProjectMapsScroller::horizontalDragByUser(Component *component)
{
    this->screenRangeRectangle->setRealBounds(this->screenRangeRectangle->getRealBounds()
        .withX(float(component->getX())));

    this->xMoveByUser();
}

void ProjectMapsScroller::xyMoveByUser()
{
    const auto screenRangeBounds = this->screenRangeRectangle->getRealBounds();

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
    const auto hp = p.toNearestInt();
    this->horizontalRangeRectangle->setBounds(hp.withTop(1).withBottom(this->getHeight()));
    this->screenRangeRectangle->setRealBounds(p);

    const auto mapBounds = this->getMapBounds();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(mapBounds);
    }
}

void ProjectMapsScroller::xMoveByUser()
{
    const auto screenRangeBounds = this->screenRangeRectangle->getRealBounds();

    const float mw = float(this->getWidth()) - screenRangeBounds.getWidth();
    const float propX = screenRangeBounds.getTopLeft().getX() / mw;
    const float propY = float(this->roll->getViewport().getViewPositionY()) /
        float(this->roll->getHeight() - this->roll->getViewport().getViewHeight());

    this->roll->panProportionally(propX, propY);

    const auto p = this->getIndicatorBounds();
    const auto hp = p.toNearestInt();
    this->horizontalRangeRectangle->setBounds(hp.withTop(1).withBottom(this->getHeight()));
    this->screenRangeRectangle->setRealBounds(p);

    const auto mapBounds = this->getMapBounds();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(mapBounds);
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void ProjectMapsScroller::resized()
{
    this->updateAllChildrenBounds();
}

void ProjectMapsScroller::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getBottomPanelBackground(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(this->borderLineDark);
    g.fillRect(0, 0, this->getWidth(), 1);

    g.setColour(this->borderLineLight);
    g.fillRect(0, 1, this->getWidth(), 1);

    if (this->drawingNewScreenRange.hasValue() && !this->drawingNewScreenRange->isEmpty())
    {
        const auto rect = this->getMapBounds()
            .toFloat().getProportion(*this->drawingNewScreenRange);

        g.setColour(this->beatRangeBorderColour);
        g.fillRect(rect);

        g.setColour(this->beatRangeFillColour);
        g.drawRect(rect);
        g.drawRect(rect);
    }
}

void ProjectMapsScroller::mouseDown(const MouseEvent &event)
{
    if (this->isMultiTouchEvent(event))
    {
        return;
    }

    this->panningStart = event.getPosition();
    this->rollViewportPositionAtDragStart = this->roll->getViewport().getViewPosition();

    // this thing feels weird on mobile: panning as the default behaviour is more natural there,
    // while on desktop panning is done via right mouse button everywhere:
    if (!event.source.isTouch() &&
        this->isInStretchedMode() &&
        event.mods.isLeftButtonDown())
    {
        const auto mapBounds = this->getMapBounds();
        this->drawingNewScreenRange = {
            (event.position.x - mapBounds.getX()) / float(mapBounds.getWidth()),
            (event.position.y - mapBounds.getY()) / float(mapBounds.getHeight()), 0, 0 };

        this->repaint();
    }
}

void ProjectMapsScroller::mouseDrag(const MouseEvent &event)
{
    if (this->isMultiTouchEvent(event))
    {
        return;
    }

    if (this->drawingNewScreenRange.hasValue())
    {
        const auto mapBounds = this->getMapBounds();
        const auto r = (event.position.x - mapBounds.getX()) / float(mapBounds.getWidth());
        const auto b = (event.position.y - mapBounds.getY()) / float(mapBounds.getHeight());
        this->drawingNewScreenRange->setRight(r);
        this->drawingNewScreenRange->setBottom(b);
        this->repaint();
    }
    else
    {
        // simple dragging on mobile platforms to make it less awkward:
        const bool simplePanning = event.source.isTouch();
        const auto mapWidth = this->getMapBounds().getWidth();
        if (simplePanning || mapWidth > this->getWidth())
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            const auto viewWidth = this->roll->getViewport().getViewWidth();
            const auto dragDistance = float(event.getPosition().getX() - this->panningStart.getX());
            const auto dragSpeed = simplePanning ? 1.f :
                float(this->roll->getWidth() - viewWidth) / float(mapWidth - this->getWidth());
            const auto xOffset = jlimit(1, this->roll->getWidth() - viewWidth - 1,
                this->rollViewportPositionAtDragStart.x + int(-dragDistance * dragSpeed));
            this->roll->panByOffset(xOffset, this->rollViewportPositionAtDragStart.y);
        }
    }
}

void ProjectMapsScroller::mouseUp(const MouseEvent &event)
{
    this->setMouseCursor(MouseCursor::NormalCursor);

    if (this->isMultiTouchEvent(event))
    {
        return;
    }

    if (this->drawingNewScreenRange.hasValue() && !this->drawingNewScreenRange->isEmpty())
    {
        this->oldAreaBounds = this->getIndicatorBounds();
        this->oldMapBounds = this->getMapBounds().toFloat();

        this->roll->zoomAbsolute(*this->drawingNewScreenRange);

        this->updateAllChildrenBounds();
        this->repaint();

        if (this->animationsEnabled)
        {
            this->startTimerHz(60);
        }
    }

    this->drawingNewScreenRange = {};

    if (event.getOffsetFromDragStart().isOrigin())
    {
        App::Layout().broadcastCommandMessage(CommandIDs::ToggleBottomMiniMap);
    }
}

void ProjectMapsScroller::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    jassert(this->roll != nullptr);
    this->roll->mouseWheelMove(event.getEventRelativeTo(this->roll), wheel);
}

//===----------------------------------------------------------------------===//
// MultiTouchListener
//===----------------------------------------------------------------------===//

// because the roll is the upstream of move-resize events for all bottom panels,
// this will simply ensure that zooming and panning events are horizontal-only
// and pass them to the currently active roll;
// the same logic can be found in AutomationEditor and VelocityEditor

void ProjectMapsScroller::multiTouchStartZooming()
{
    this->roll->multiTouchStartZooming();
}

void ProjectMapsScroller::multiTouchContinueZooming(const Rectangle<float> &relativePositions,
    const Rectangle<float> &relativeAnchor, const Rectangle<float> &absoluteAnchor)
{
    this->roll->multiTouchContinueZooming(relativePositions, relativeAnchor, absoluteAnchor);
}

void ProjectMapsScroller::multiTouchEndZooming(const MouseEvent &anchorEvent)
{
    this->roll->multiTouchEndZooming(anchorEvent.getEventRelativeTo(this->roll));
    this->panningStart = anchorEvent.getEventRelativeTo(this).getPosition();
    this->rollViewportPositionAtDragStart = this->roll->getViewport().getViewPosition();
    // a hack to avoid the screen range jumping away instantly after multi-touch:
    this->horizontalRangeRectangle->disableDraggingUntilTap();
}

Point<float> ProjectMapsScroller::getMultiTouchRelativeAnchor(const MouseEvent &event)
{
    return this->roll->getMultiTouchRelativeAnchor(event
        .withNewPosition(Point<int>(event.getPosition().getX(), 0))
        .getEventRelativeTo(this->roll));
}

Point<float> ProjectMapsScroller::getMultiTouchAbsoluteAnchor(const MouseEvent &event)
{
    return this->roll->getMultiTouchAbsoluteAnchor(event
        .withNewPosition(Point<int>(event.getPosition().getX(), 0))
        .getEventRelativeTo(this->roll));
}

bool ProjectMapsScroller::isMultiTouchEvent(const MouseEvent &e) const noexcept
{
    return this->roll->isMultiTouchEvent(e) || this->multiTouchController->hasMultiTouch(e);
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void ProjectMapsScroller::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectStartIndicator->updatePosition(firstBeat);
    this->projectEndIndicator->updatePosition(lastBeat);

    if (this->isVisible())
    {
        this->updateAllChildrenBounds();
    }
}

void ProjectMapsScroller::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->projectStartIndicator->updateViewRange(firstBeat, lastBeat);
    this->projectEndIndicator->updateViewRange(firstBeat, lastBeat);

    if (this->isVisible())
    {
        this->updateAllChildrenBounds();
    }
}

//===----------------------------------------------------------------------===//
// RollListener
//===----------------------------------------------------------------------===//

void ProjectMapsScroller::onMidiRollMoved(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll && !this->isTimerRunning())
    {
        this->updateAllChildrenBounds();
    }
}

void ProjectMapsScroller::onMidiRollResized(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll && !this->isTimerRunning())
    {
        this->updateAllChildrenBounds();
    }
}

void ProjectMapsScroller::switchToRoll(SafePointer<RollBase> roll)
{
    this->oldAreaBounds = this->getIndicatorBounds();
    this->oldMapBounds = this->getMapBounds().toFloat();

    this->roll = roll;
    for (auto *map : this->trackMaps)
    {
        map->switchToRoll(roll);
    }

    this->screenRangeTargetBrightness =
        (nullptr != dynamic_cast<PianoRoll *>(roll.getComponent())) ? 1.f : 0.f;

    if (this->animationsEnabled)
    {
        this->startTimerHz(60);
    }
    else
    {
        this->screenRangeRectangle->setBrightness(this->screenRangeTargetBrightness);
        this->updateAllChildrenBounds();
    }
}

//===----------------------------------------------------------------------===//
// ScrolledComponent
//===----------------------------------------------------------------------===//

void ProjectMapsScroller::ScrolledComponent::switchToRoll(SafePointer<RollBase> roll)
{
    this->roll = roll;
}

bool ProjectMapsScroller::ScrolledComponent::isMultiTouchEvent(const MouseEvent &e) const noexcept
{
    jassert(this->roll != nullptr);
    return this->roll->isMultiTouchEvent(e);
}

//===----------------------------------------------------------------------===//
// Timer
//===----------------------------------------------------------------------===//

static Rectangle<float> lerpMapBounds(const Rectangle<float> &r1,
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

static float getMapBoundsDistance(const Rectangle<float> &r1,
    const Rectangle<float> &r2)
{
    return fabs(r1.getX() - r2.getX()) +
        fabs(r1.getY() - r2.getY()) +
        fabs(r1.getWidth() - r2.getWidth()) +
        fabs(r1.getHeight() - r2.getHeight());
}

void ProjectMapsScroller::timerCallback()
{
    const auto mb = this->getMapBounds().toFloat();
    const auto mbLerp = lerpMapBounds(this->oldMapBounds, mb, 0.4f);
    const auto ib = this->getIndicatorBounds();
    const auto ibLerp = lerpMapBounds(this->oldAreaBounds, ib, 0.4f);
    const bool shouldStop = getMapBoundsDistance(this->oldAreaBounds, ib) < 0.5f;
    const auto targetAreaBounds = shouldStop ? ib : ibLerp;
    const auto targetMapBounds = shouldStop ? mb : mbLerp;

    this->oldAreaBounds = targetAreaBounds;
    this->oldMapBounds = targetMapBounds;

    const auto screenRangeBrightness =
        (this->screenRangeRectangle->getBrightness() + this->screenRangeTargetBrightness) / 2.f;
    this->screenRangeRectangle->setBrightness(screenRangeBrightness);

    const auto helperBounds = targetAreaBounds.toNearestInt();
    this->horizontalRangeRectangle->setBounds(helperBounds.withTop(1).withBottom(this->getHeight()));
    this->screenRangeRectangle->setRealBounds(targetAreaBounds);

    const auto finalMapBounds = targetMapBounds.toNearestInt();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(finalMapBounds);
    }

    this->projectStartIndicator->updateBounds(finalMapBounds);
    this->projectEndIndicator->updateBounds(finalMapBounds);

    if (shouldStop)
    {
        this->screenRangeRectangle->setBrightness(this->screenRangeTargetBrightness);
        this->stopTimer();
    }
}

void ProjectMapsScroller::updateAllChildrenBounds()
{
    const auto p = this->getIndicatorBounds();
    const auto hp = p.toNearestInt();
    this->horizontalRangeRectangle->setBounds(hp.withTop(1).withBottom(this->getHeight()));
    this->screenRangeRectangle->setRealBounds(p);

    const auto mapBounds = this->getMapBounds();
    for (auto *map : this->trackMaps)
    {
        map->setBounds(mapBounds);
    }

    this->projectStartIndicator->updateBounds(mapBounds);
    this->projectEndIndicator->updateBounds(mapBounds);

    if (!this->playhead->isTimerRunning()) // avoid glitches when zooming during playback
    {
        this->playhead->updatePosition();
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Rectangle<float> ProjectMapsScroller::getIndicatorBounds() const noexcept
{
    jassert(this->roll != nullptr);

    const auto viewX = float(this->roll->getViewport().getViewPositionX());
    const auto viewWidth = float(jmax(1, this->roll->getViewport().getViewWidth()));
    const auto rollWidth = float(this->roll->getWidth());
    const auto rollInvisibleArea = rollWidth - viewWidth;
    const auto trackWidth = float(this->getWidth());
    const auto trackInvisibleArea = float(this->getWidth() - ProjectMapsScroller::screenRangeWidth);
    const auto mapWidth = (ProjectMapsScroller::screenRangeWidth * rollWidth) / viewWidth;

    constexpr auto rollHeaderHeight = float(Globals::UI::rollHeaderHeight);
    const auto zoomFactorY = this->roll->getZoomFactorY();
    const auto rollHeight = float(this->roll->getHeight() - rollHeaderHeight);
    const auto viewY = float(this->roll->getViewport().getViewPositionY() + rollHeaderHeight);
    const auto trackHeight = float(this->getHeight());
    const auto trackHeaderHeight = float(rollHeaderHeight * trackHeight / rollHeight);

    const auto rY = roundf(trackHeight * (viewY / rollHeight)) - trackHeaderHeight;
    const auto rH = trackHeight * zoomFactorY;

    if (mapWidth <= trackWidth || !this->isInStretchedMode())
    {
        const auto rX = (trackWidth * viewX) / rollWidth;
        const auto rW = trackWidth * this->roll->getZoomFactorX();
        return { rX, rY, rW, rH };
    }

    const auto rX = (trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth);
    constexpr auto rW = ProjectMapsScroller::screenRangeWidth;
    return { rX, rY, rW, rH };
}

Rectangle<int> ProjectMapsScroller::getMapBounds() const noexcept
{
    jassert(this->roll != nullptr);

    const auto viewX = float(this->roll->getViewport().getViewPositionX());
    const auto viewWidth = float(jmax(1, this->roll->getViewport().getViewWidth()));
    const auto rollWidth = float(this->roll->getWidth());
    const auto rollInvisibleArea = rollWidth - viewWidth;
    const auto trackInvisibleArea = float(this->getWidth() - ProjectMapsScroller::screenRangeWidth);
    const auto mapWidth = (ProjectMapsScroller::screenRangeWidth * rollWidth) / viewWidth;

    if (mapWidth <= this->getWidth() || !this->isInStretchedMode())
    {
        return { 0, 0, this->getWidth(), this->getHeight() };
    }

    const auto rX = (trackInvisibleArea * viewX) / jmax(rollInvisibleArea, viewWidth);
    const auto dX = (viewX * mapWidth) / rollWidth;
    return { roundToInt(rX - dX), 0, roundToInt(mapWidth), this->getHeight() };
}

void ProjectMapsScroller::setScrollerMode(ScrollerMode mode)
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

    this->updateAllChildrenBounds();
}

ProjectMapsScroller::ScrollerMode ProjectMapsScroller::getScrollerMode() const noexcept
{
    return this->scrollerMode;
}

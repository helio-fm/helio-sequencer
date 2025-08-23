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

#pragma once

class RollBase;
class Playhead;
class ProjectNode;
class TrackStartIndicator;
class TrackEndIndicator;
class MultiTouchController;

#include "ProjectListener.h"
#include "MultiTouchListener.h"
#include "RollListener.h"
#include "ColourIDs.h"

class ProjectMapsScroller final :
    public Component,
    public ProjectListener,
    public RollListener,
    public MultiTouchListener,
    private Timer // optionally animates transitions between rolls
{
public:

    ProjectMapsScroller(ProjectNode &project, SafePointer<RollBase> roll);
    ~ProjectMapsScroller() override;

    class ScrolledComponent : public Component
    {
    public:

        explicit ScrolledComponent(SafePointer<RollBase> roll) : roll(roll) {}

        void switchToRoll(SafePointer<RollBase> roll);
        bool isMultiTouchEvent(const MouseEvent &e) const noexcept;

    protected:

        SafePointer<RollBase> roll;
    };

    template <typename T, typename... Args> inline
    void addOwnedMap(Args &&... args)
    {
        auto *newTrackMap = this->trackMaps.add(new T(std::forward<Args>(args)...));
        this->addAndMakeVisible(newTrackMap);

        // playhead is always tied to the first map:
        if (this->trackMaps.size() == 1)
        {
            this->disconnectPlayhead();
            newTrackMap->addAndMakeVisible(this->playhead.get());
        }

        newTrackMap->toFront(false);
        this->horizontalRangeRectangle->toFront(false);
        this->screenRangeRectangle->toFront(false);
    }

    void switchToRoll(SafePointer<RollBase> roll);

    void setAnimationsEnabled(bool shouldBeEnabled)
    {
        this->animationsEnabled = shouldBeEnabled;
    }

    //===------------------------------------------------------------------===//
    // TrackScroller
    //===------------------------------------------------------------------===//

    void xyMoveByUser();
    void xMoveByUser();

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;

    //===------------------------------------------------------------------===//
    // RollListener
    //===------------------------------------------------------------------===//

    void onMidiRollMoved(RollBase *targetRoll) override;
    void onMidiRollResized(RollBase *targetRoll) override;

    //===------------------------------------------------------------------===//
    // MultiTouchListener
    //===------------------------------------------------------------------===//

    void multiTouchStartZooming() override;
    void multiTouchContinueZooming(
            const Rectangle<float> &relativePosition,
            const Rectangle<float> &relativePositionAnchor,
            const Rectangle<float> &absolutePositionAnchor) override;
    void multiTouchEndZooming(const MouseEvent &anchorEvent) override;

    Point<float> getMultiTouchRelativeAnchor(const MouseEvent &e) override;
    Point<float> getMultiTouchAbsoluteAnchor(const MouseEvent &e) override;

    bool isMultiTouchEvent(const MouseEvent &e) const noexcept;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;

public:

    enum class ScrollerMode : int
    {
        Map,
        Scroller
    };

    void setScrollerMode(ScrollerMode mode);
    ScrollerMode getScrollerMode() const noexcept;

private:

    ScrollerMode scrollerMode = ScrollerMode::Map;

    bool isInStretchedMode() const noexcept
    {
        // no stretching in the scroller mode
        return this->scrollerMode == ScrollerMode::Map;
    }

private:

    //===------------------------------------------------------------------===//
    // Two helpers for dragging screen range rectangles
    //===------------------------------------------------------------------===//

    class ScreenRangeRectangle final : public Component
    {
    public:

        // this is a smol rectangle indicating the beat&key screen range,
        // shown inside a larger rectangle indicating the beat range
        ScreenRangeRectangle() :
            currentColour(fillColour)
        {
            this->setPaintingIsUnclipped(true);
            this->setMouseClickGrabsKeyboardFocus(false);
            this->setAccessible(false);
            this->setInterceptsMouseClicks(false, false);
        }

        Rectangle<float> getRealBounds() const noexcept
        {
            return this->realBounds;
        }

        void setRealBounds(const Rectangle<float> &bounds)
        {
            this->realBounds = bounds;
            this->setBounds(this->realBounds.toNearestInt());
        }

        float getBrightness() const noexcept
        {
            return this->brightness;
        }

        void setBrightness(float newBrightness)
        {
            if (this->brightness == newBrightness)
            {
                return;
            }

            this->brightness = newBrightness;
            this->currentColour = this->fillColour.withMultipliedAlpha(this->brightness);

            this->setVisible(this->brightness != 0.f);
            this->repaint();
        }

        void paint(Graphics &g) override
        {
            g.setColour(this->currentColour);
            g.fillRect(this->getLocalBounds());
        }

    private:

        Rectangle<float> realBounds;

        const Colour fillColour = findDefaultColour(ColourIDs::TrackScroller::viewRangeFill);

        Colour currentColour;
        float brightness = 1.f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScreenRangeRectangle)
    };

    class HorizontalRangeRectangle final : public Component
    {
    public:

        explicit HorizontalRangeRectangle(ProjectMapsScroller &scroller) :
            scroller(scroller),
            moveConstrainer(scroller)
        {
            this->setPaintingIsUnclipped(true);
            this->setInterceptsMouseClicks(true, false);
            this->setMouseClickGrabsKeyboardFocus(false);
            this->setAccessible(false);

            this->toBack();

            this->moveConstrainer.setMinimumSize(4, 4);
            this->moveConstrainer.setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);
        }

        void disableDraggingUntilTap()
        {
            this->draggingDisabledUntilTap = true;
        }

        void mouseDown(const MouseEvent &e) override
        {
            if (!this->scroller.isMultiTouchEvent(e))
            {
                this->dragger.startDraggingComponent(this, e);
                this->draggingDisabledUntilTap = false;
            }
        }

        void mouseDrag(const MouseEvent &e) override
        {
            if (!this->scroller.isMultiTouchEvent(e) && !this->draggingDisabledUntilTap)
            {
                this->setMouseCursor(MouseCursor::DraggingHandCursor);
                this->dragger.dragComponent(this, e, &this->moveConstrainer);
            }
        }

        void mouseUp(const MouseEvent &e) override
        {
            this->setMouseCursor(MouseCursor::NormalCursor);
        }

        void paint(Graphics &g) override
        {
            g.setColour(this->fillColour);
            g.fillRect(this->getLocalBounds());

            g.setColour(this->borderColour);
            g.fillRect(0.f, 0.f, 1.f, float(this->getHeight()));
            g.fillRect(float(this->getWidth() - 1.f), 0.f, 1.f, float(this->getHeight()));
        }

    private:

        ProjectMapsScroller &scroller;

        ComponentDragger dragger;
        bool draggingDisabledUntilTap = false;

        struct BoundsConstrainer final : public ComponentBoundsConstrainer
        {
            explicit BoundsConstrainer(ProjectMapsScroller &scrollerRef) : scroller(scrollerRef) {}

            void applyBoundsToComponent(Component &component, Rectangle<int> bounds) override
            {
                ComponentBoundsConstrainer::applyBoundsToComponent(component, bounds);
                this->scroller.horizontalDragByUser(&component);
            }

            ProjectMapsScroller &scroller;
        };

        BoundsConstrainer moveConstrainer;

        const Colour borderColour =
            findDefaultColour(ColourIDs::TrackScroller::viewBeatRangeBorder);

        const Colour fillColour =
            findDefaultColour(ColourIDs::TrackScroller::viewBeatRangeFill);
    };

private:

    void updateAllChildrenBounds();

    void timerCallback() override;

    ProjectNode &project;
    SafePointer<RollBase> roll;
    Point<int> rollViewportPositionAtDragStart;
    Point<int> panningStart;

    Rectangle<float> oldAreaBounds;
    Rectangle<float> oldMapBounds;

    UniquePointer<HorizontalRangeRectangle> horizontalRangeRectangle;
    void horizontalDragByUser(Component *horizontalRangeRectangle);

    static constexpr auto screenRangeWidth = 150.f;
    UniquePointer<ScreenRangeRectangle> screenRangeRectangle;
    float screenRangeTargetBrightness = 1.f;

    Optional<Rectangle<float>> drawingNewScreenRange;

    UniquePointer<Playhead> playhead;

    OwnedArray<ScrolledComponent> trackMaps;

    void disconnectPlayhead();
    Rectangle<float> getIndicatorBounds() const noexcept;
    Rectangle<int> getMapBounds() const noexcept;

    UniquePointer<TrackStartIndicator> projectStartIndicator;
    UniquePointer<TrackEndIndicator> projectEndIndicator;

    UniquePointer<MultiTouchController> multiTouchController;

    const Colour borderLineDark =
        findDefaultColour(ColourIDs::TrackScroller::borderLineDark);
    const Colour borderLineLight =
        findDefaultColour(ColourIDs::TrackScroller::borderLineLight);
    const Colour beatRangeBorderColour =
        findDefaultColour(ColourIDs::TrackScroller::viewBeatRangeBorder);
    const Colour beatRangeFillColour =
        findDefaultColour(ColourIDs::TrackScroller::viewBeatRangeFill);

    bool animationsEnabled = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectMapsScroller)
};

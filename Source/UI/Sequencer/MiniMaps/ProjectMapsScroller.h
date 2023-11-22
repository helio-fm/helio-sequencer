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
class Transport;

#include "RollListener.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class ProjectMapsScroller final :
    public Component,
    public RollListener,
    private AsyncUpdater, // triggers batch move/resize events for children
    private Timer // optionally animates transitions between rolls
{
public:

    ProjectMapsScroller(Transport &transport, SafePointer<RollBase> roll);
    ~ProjectMapsScroller() override;

    class ScrolledComponent : public Component
    {
    public:

        explicit ScrolledComponent(SafePointer<RollBase> roll) : roll(roll) {}

        void switchToRoll(SafePointer<RollBase> roll);
        bool rollHasMultiTouch(const MouseEvent &e) const;

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
        this->helperRectangle->toFront(false);
        this->screenRange->toFront(false);
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
    // Component
    //===------------------------------------------------------------------===//
    
    void resized() override;
    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;
    
    //===------------------------------------------------------------------===//
    // RollListener
    //===------------------------------------------------------------------===//
    
    void onMidiRollMoved(RollBase *targetRoll) override;
    void onMidiRollResized(RollBase *targetRoll) override;
    
    //===------------------------------------------------------------------===//
    // Additional horizontal dragger
    //===------------------------------------------------------------------===//
    
    class HorizontalDragHelper final : public Component
    {
    public:
        
        explicit HorizontalDragHelper(ProjectMapsScroller &scrollerRef);

        void mouseDown(const MouseEvent &e) override;
        void mouseDrag(const MouseEvent &e) override;
        void mouseUp(const MouseEvent &e) override;
        void paint(Graphics &g) override;

        void setBrightness(float brightness);
        static constexpr auto defaultBrightness = 0.4f;

    private:
        
        Colour colour;
        ProjectMapsScroller &scroller;
        ComponentDragger dragger;
        UniquePointer<ComponentBoundsConstrainer> moveConstrainer;
    };

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

    bool stretchedMode() const noexcept
    {
        // no stretching in the scroller mode
        return this->scrollerMode == ScrollerMode::Map;
    }

private:

    class ScreenRange final : public Component
    {
    public:

        explicit ScreenRange(ProjectMapsScroller &scrollerRef) :
            colour(findDefaultColour(ColourIDs::TrackScroller::screenRangeFill)),
            scroller(scrollerRef)
        {
            this->setPaintingIsUnclipped(true);
            this->setMouseClickGrabsKeyboardFocus(false);

            this->moveConstrainer = make<ComponentBoundsConstrainer>();
            this->moveConstrainer->setMinimumSize(4, 4);
            this->moveConstrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);
        }

        Rectangle<float> getRealBounds() const noexcept
        {
            return this->realBounds;
        }

        void setRealBounds(const Rectangle<float> &bounds)
        {
            this->realBounds = bounds;
            this->setBounds(this->realBounds.toType<int>());
        }

        //===------------------------------------------------------------------===//
        // Component
        //===------------------------------------------------------------------===//

        void mouseDown(const MouseEvent &e) override
        {
            this->dragger.startDraggingComponent(this, e);
        }

        void mouseDrag(const MouseEvent &e) override
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            const auto lastPosition = this->getPosition().toFloat();
            this->dragger.dragComponent(this, e, this->moveConstrainer.get());
            const auto moveDelta = this->getPosition().toFloat() - lastPosition;
            this->realBounds.translate(moveDelta.getX(), moveDelta.getY());
            this->scroller.xyMoveByUser();
        }

        void mouseUp(const MouseEvent &e) override
        {
            this->setMouseCursor(MouseCursor::NormalCursor);
        }

        void paint(Graphics &g) override
        {
            g.setColour(this->colour);
            g.fillRect(this->getLocalBounds());
        }

    private:
        
        Rectangle<float> realBounds;

        Colour colour;
        ProjectMapsScroller &scroller;
        ComponentDragger dragger;

        UniquePointer<ComponentBoundsConstrainer> moveConstrainer;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScreenRange)
    };

private:

    void horizontalDragByUser(Component *component, const Rectangle<int> &bounds);
    friend class HorizontalDragHelperConstrainer;

    void handleAsyncUpdate() override;
    void timerCallback() override;
    void updateAllBounds();
    
    Transport &transport;
    SafePointer<RollBase> roll;
    Point<int> rollViewportPositionAtDragStart;
    
    Rectangle<float> oldAreaBounds;
    Rectangle<float> oldMapBounds;

    static constexpr auto screenRangeWidth = 150.f;
    UniquePointer<ProjectMapsScroller::ScreenRange> screenRange;

    Rectangle<float> drawingNewScreenRange;

    UniquePointer<Playhead> playhead;
   
    OwnedArray<ScrolledComponent> trackMaps;

    void disconnectPlayhead();
    Rectangle<float> getIndicatorBounds() const noexcept;
    Rectangle<int> getMapBounds() const noexcept;
    
    ComponentDragger helperDragger;
    UniquePointer<HorizontalDragHelper> helperRectangle;

    const Colour borderLineDark = findDefaultColour(ColourIDs::TrackScroller::borderLineDark);
    const Colour borderLineLight = findDefaultColour(ColourIDs::TrackScroller::borderLineLight);

    bool animationsEnabled = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectMapsScroller)
};

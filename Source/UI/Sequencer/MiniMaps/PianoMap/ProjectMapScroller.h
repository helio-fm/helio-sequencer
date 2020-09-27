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

#pragma once

class HybridRoll;
class TrackMap;
class ProjectMapScrollerScreen;
class Playhead;
class Transport;

#include "HelperRectangle.h"
#include "HybridRollListener.h"
#include "ComponentFader.h"

class ProjectMapScroller final :
    public Component,
    public HybridRollListener,
    private AsyncUpdater,
    private Timer
{
public:

    ProjectMapScroller(Transport &transport, SafePointer<HybridRoll> roll);
    ~ProjectMapScroller() override;

    void addOwnedMap(Component *newTrackMap, bool shouldBringToFront);
    void removeOwnedMap(Component *existingTrackMap);

    void switchToRoll(SafePointer<HybridRoll> roll);

    template<typename T>
    T *findOwnedMapOfType()
    {
        for (int i = 0; i < this->trackMaps.size(); ++i)
        {
            if (T *target = dynamic_cast<T *>(this->trackMaps.getUnchecked(i)))
            {
                return target;
            }
        }
        
        return nullptr;
    }

    //===------------------------------------------------------------------===//
    // TrackScroller
    //===------------------------------------------------------------------===//
    
    void xyMoveByUser();
    void xMoveByUser();
    void toggleStretchingMapAlaSublime();
    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void resized() override;
    void paint(Graphics &g) override;
    void mouseDrag(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;
    
    //===------------------------------------------------------------------===//
    // HybridRollListener
    //===------------------------------------------------------------------===//
    
    void onMidiRollMoved(HybridRoll *targetRoll) override;
    void onMidiRollResized(HybridRoll *targetRoll) override;
    
    //===------------------------------------------------------------------===//
    // Additional horizontal dragger
    //===------------------------------------------------------------------===//
    
    class HorizontalDragHelper final : public HelperRectangle
    {
    public:
        
        explicit HorizontalDragHelper(ProjectMapScroller &scrollerRef);
        void mouseDown(const MouseEvent &e) override;
        void mouseDrag(const MouseEvent &e) override;
        void paint(Graphics &g) override;
        
    private:
        
        Colour colour;
        ProjectMapScroller &scroller;
        ComponentDragger dragger;
        UniquePointer<ComponentBoundsConstrainer> moveConstrainer;
    };
    
protected:

    void horizontalDragByUser(Component *component, const Rectangle<int> &bounds);
    friend class HorizontalDragHelperConstrainer;

private:
    
    void handleAsyncUpdate() override;
    void timerCallback() override;
    
    Transport &transport;
    SafePointer<HybridRoll> roll;
    
    Rectangle<float> oldAreaBounds;
    Rectangle<float> oldMapBounds;

    UniquePointer<ProjectMapScrollerScreen> screenRange;
    UniquePointer<Playhead> playhead;
    
    OwnedArray<Component> trackMaps;

    void disconnectPlayhead();
    Rectangle<float> getIndicatorBounds() const noexcept;
    Rectangle<int> getMapBounds() const noexcept;
    
    ComponentDragger helperDragger;
    UniquePointer<HelperRectangle> helperRectangle;

    const Colour borderLineDark;
    const Colour borderLineLight;

    bool mapShouldBeStretched = true;
    
};

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
class LevelsMapScrollerScreen;
class Playhead;
class Transport;

#include "HelperRectangle.h"
#include "HybridRollListener.h"

class LevelsMapScroller final :
    public Component,
    public HybridRollListener,
    private AsyncUpdater
{
public:

    explicit LevelsMapScroller(SafePointer<HybridRoll> roll);
    
    void addOwnedMap(Component *newTrackMap);
    void removeOwnedMap(Component *existingTrackMap);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void resized() override;
    void paint(Graphics &g) override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;
    
    //===------------------------------------------------------------------===//
    // HybridRollListener
    //===------------------------------------------------------------------===//
    
    void onMidiRollMoved(HybridRoll *targetRoll) override;
    void onMidiRollResized(HybridRoll *targetRoll) override;
    
private:
    
    void handleAsyncUpdate() override;
    Rectangle<int> getMapBounds() const noexcept;

    SafePointer<HybridRoll> roll;
    OwnedArray<Component> trackMaps;
    
};

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

#include "RollListener.h"

class EditorPanelsScroller final :
    public Component,
    public RollListener,
    private AsyncUpdater
{
public:

    explicit EditorPanelsScroller(SafePointer<RollBase> roll);

    class ScrolledComponent : public Component
    {
    public:
        virtual void switchToRoll(SafePointer<RollBase> roll) = 0;
    };

    template <typename T, typename... Args> inline
        void addOwnedMap(Args &&... args)
    {
        auto *newTrackMap = this->trackMaps.add(new T(std::forward<Args>(args)...));
        this->addAndMakeVisible(newTrackMap);
        newTrackMap->toFront(false);
    }

    void switchToRoll(SafePointer<RollBase> roll);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void resized() override;
    void paint(Graphics &g) override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;
    
    //===------------------------------------------------------------------===//
    // RollListener
    //===------------------------------------------------------------------===//
    
    void onMidiRollMoved(RollBase *targetRoll) override;
    void onMidiRollResized(RollBase *targetRoll) override;
    
private:
    
    void handleAsyncUpdate() override;
    Rectangle<int> getMapBounds() const noexcept;

    SafePointer<RollBase> roll;
    OwnedArray<ScrolledComponent> trackMaps;
    
};
